/*
 * gpio.c: the real-time process that handles multiplexing
 * 
 * www.obsolescenceguaranteed.blogspot.com
 * 
 * The only communication with the main program (simh):
 * - external variable ledstatus is read to determine which leds to light.
 * - external variable switchstatus is updated with current switch settings.
 * 
*/

#include "gpio.h"

#include "config.h"

#ifdef HAVE_TIME_H
#	include <time.h>
#endif

#include <pthread.h>
#include <sys/time.h>

#define BLOCK_SIZE (4*1024)


struct bcm2835_peripheral gpio;	// needs initialisation


// A constant meaning "indeterminate milliseconds", used for error
// returns from ms_time() and for the case where the switch is in the
// stable state in the switch_state array.
static const ms_time_t na_ms = (ms_time_t)-1;


// Adjust columns to scan based on whether the serial mod was done, as
// that affects the free GPIOs for our use, and how the PCB connects
// them to the LED matrix.
#ifdef SERIALSETUP
uint8_t cols[] = {13, 12, 11,    10, 9, 8,    7, 6, 5,    4, 3, 2};
#else
uint8_t cols[] = {13, 12, 11,    10, 9, 8,    7, 6, 5,    4, 15, 14};
#endif

uint8_t ledrows[] = {20, 21, 22, 23, 24, 25, 26, 27};

uint8_t rows[] = {16, 17, 18};

// Array sizes.  Must be declared twice because we need to export them,
// and C doesn't have true const, as C++ does.
#define NCOLS    (sizeof(cols)    / sizeof(cols[0]))
#define NLEDROWS (sizeof(ledrows) / sizeof(ledrows[0]))
#define NROWS    (sizeof(rows)    / sizeof(rows[0]))
const size_t ncols    = NCOLS;
const size_t nledrows = NLEDROWS;
const size_t nrows    = NROWS;

// The public switch and LED API: other threads poke values into
// ledstatus to affect our GPIO LED pin driving loop and read values
// from switchstatus to discover our current published value of the
// switch states.  The latter may differ from the *actual* switch
// states due to the debouncing procedure.
uint16_t switchstatus[NROWS];	// bitfield: sparse nrows x ncols switch matrix
uint16_t ledstatus[NLEDROWS];	// bitfield: sparse nledrows x ncols LED matrix

// Time-delayed reaction to switch changes to debounce the contacts.
// This is especially important with the incandescent lamp simulation
// feature enabled since that speeds up the GPIO scanning loop, making
// it more susceptible to contact bounce.
struct switch_state {
	// switch state currently reported via switchstatus[]
	int stable_state;

	// ms the switch state has been != stable_state, or na_ms
	// if it is currently in that same state
	ms_time_t last_change;		
};
static struct switch_state gss[NROWS][NCOLS];
static int gss_initted = 0;
static const ms_time_t debounce_ms = 50;	// time switch state must remain stable


// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(struct bcm2835_peripheral *p)
{
   if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("Failed to open /dev/mem, try checking permissions.\n");
      return -1;
   }
   p->map = mmap(
      NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED,
      p->mem_fd,      	// File descriptor to physical memory virtual file '/dev/mem'
      p->addr_p);       // Address in physical map that we want this memory block to expose
   if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
   }
   p->addr = (volatile unsigned int *)p->map;
   return 0;
}


void unmap_peripheral(struct bcm2835_peripheral *p)
{	munmap(p->map, BLOCK_SIZE);
	close(p->mem_fd);
}


unsigned bcm_host_get_peripheral_address(void)		// find Pi's GPIO base address
{
	unsigned address = ~0;
	FILE *fp = fopen("/proc/device-tree/soc/ranges", "rb");
	if (fp)
	{	unsigned char buf[4];
		fseek(fp, 4, SEEK_SET);
		if (fread(buf, 1, sizeof buf, fp) == sizeof buf)
			address = buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3] << 0;
		fclose(fp);
	}

	return address == ~0 ? 0x20000000 : address;
}


void sleep_ns(ns_time_t ns)
{
	struct timespec ts = { 0, ns };
#if defined(HAVE_CLOCK_NANOSLEEP)
	clock_nanosleep(CLOCK_REALTIME, 0, &ts, NULL);
#elif defined(HAVE_NANOSLEEP)
	nanosleep(&ts, NULL);
#elif defined(HAVE_USLEEP)
	usleep(ns / 1000);
#else
#	error Cannot build GPIO controller without high-res "sleep" function!
#endif
}


// Like time(2) except that it returns milliseconds since the Unix epoch
static ms_time_t ms_time(ms_time_t* pt)
{
	struct timeval tv;
	if (gettimeofday(&tv, 0) == 0) {
		ms_time_t t = (ms_time_t)(tv.tv_sec / 1000.0 + tv.tv_usec * 1000.0);
		if (pt) *pt = t;
		return t;
	}
	else {
		return na_ms;
	}
}


// The multiplexing logic driving the front panel -------------

// Save given switch state ss into the exported switchstatus bitfield
// so the simulator core will see it.  (Constrast the gss matrix,
// which holds our internal view of the unstable truth.)
static void report_ss(int row, int col, int ss,
		struct switch_state* pss)
{
	pss->stable_state = ss;
	pss->last_change = na_ms;

	int mask = 1 << col;
	if (ss) switchstatus[row] |=  mask;
	else    switchstatus[row] &= ~mask;

	//printf("%cSS[%d][%02d] = %d  ", gss_initted ? 'N' : 'I',
	//		row, col, ss);
}


// Given the state of the switch at (row,col), work out if this requires
// a change in our exported switch state.
static void debounce_switch(int row, int col, int ss, ms_time_t now_ms)
{
	struct switch_state* pss = &gss[row][col];

	if (!gss_initted) {
		// First time thru, so set this switch's module-global and
		// exported state to its defaults now that we know the switch's
		// initial state.
		report_ss(row, col, ss, pss);
	}
	else if (ss == pss->stable_state) {
		// This switch is still/again in the state we consider "stable",
		// which we are reporting in our switchstatus bitfield.  Reset
		// the debounce timer in case it is returning to its stable
		// state from a brief blip into the other state.
		pss->last_change = na_ms;
	}
	else if (pss->last_change == na_ms) {
		// This switch just left what we consider the "stable" state, so
		// start the debounce timer.
		pss->last_change = now_ms;
	}
	else if ((now_ms - pss->last_change) > debounce_ms) {
		// Switch has been in the new state long enough for the contacts
		// to have stopped bouncing: report its state change to outsiders.
		report_ss(row, col, ss, pss);
	}
	// else, switch was in the new state both this time and the one prior,
	// but it hasn't been there long enough to report it
}


// The GPIO module's main loop, and its thread entry point

void *blink(void *terminate)
{
	int i,j,k;
	const us_time_t intervl = 300;	// light each row of leds 300 µs
	ms_time_t now_ms;

	// Find gpio address (different for Pi 2) ----------
	gpio.addr_p = bcm_host_get_peripheral_address() + 0x200000;
	if (gpio.addr_p== 0x20200000) printf("RPi Plus detected - ");
	else printf("RPi 2 detected - ");
#ifdef SERIALSETUP
	printf(" Serial mod version\n");
#else
	printf(" Default version\n");
#endif

	// set thread to real time priority -----------------
	struct sched_param sp;
	sp.sched_priority = 98; // maybe 99, 32, 31?
	if (pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp))
	{ fprintf(stderr, "warning: failed to set RT priority\n"); }
	// --------------------------------------------------
	if(map_peripheral(&gpio) == -1)
	{	printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
		return (void *)-1;
	}

	// initialise GPIO (all pins used as inputs, with pull-ups enabled on cols)
	//	INSERT CODE HERE TO SET GPIO 14 AND 15 TO I/O INSTEAD OF ALT 0.
	//	AT THE MOMENT, USE "sudo ./gpio mode 14 in" and "sudo ./gpio mode 15 in". "sudo ./gpio readall" to verify.

	for (i=0;i<nledrows;i++)			// Define ledrows as input
	{	INP_GPIO(ledrows[i]);
		GPIO_CLR = 1 << ledrows[i];		// so go to Low when switched to output
	}
	for (i=0;i<ncols;i++)				// Define cols as input
	{	INP_GPIO(cols[i]);
	}
	for (i=0;i<nrows;i++)				// Define rows as input
	{	INP_GPIO(rows[i]);
	}

	// BCM2835 ARM Peripherals PDF p 101 & elinux.org/RPi_Low-level_peripherals#Internal_Pull-Ups_.26_Pull-Downs
	GPIO_PULL = 2;	// pull-up
	usleep(1);	// must wait 150 cycles
#ifdef SERIALSETUP
	GPIO_PULLCLK0 = 0x03ffc; // selects GPIO pins 2..13 (frees up serial port on 14 & 15)
#else
	GPIO_PULLCLK0 = 0x0fff0; // selects GPIO pins 4..15 (assumes we avoid pins 2 and 3!)
#endif
	usleep(1);
	GPIO_PULL = 0; // reset GPPUD register
	usleep(1);
	GPIO_PULLCLK0 = 0; // remove clock
	usleep(1); // probably unnecessary

	// BCM2835 ARM Peripherals PDF p 101 & elinux.org/RPi_Low-level_peripherals#Internal_Pull-Ups_.26_Pull-Downs
	GPIO_PULL = 1;	// pull-down to avoid ghosting (dec2015)
	usleep(1);	// must wait 150 cycles
	GPIO_PULLCLK0 = 0x0ff00000; // selects GPIO pins 20..27
	usleep(1);
	GPIO_PULL = 0; // reset GPPUD register
	usleep(1);
	GPIO_PULLCLK0 = 0; // remove clock
	usleep(1); // probably unnecessary

	// BCM2835 ARM Peripherals PDF p 101 & elinux.org/RPi_Low-level_peripherals#Internal_Pull-Ups_.26_Pull-Downs
	GPIO_PULL = 0;	// no pull-up no pull down just float
// not the reason for flashes it seems:
//GPIO_PULL = 2;	// pull-up - letf in but does not the reason for flashes
	usleep(1);	// must wait 150 cycles
	GPIO_PULLCLK0 = 0x070000; // selects GPIO pins 16..18
	usleep(1);
	GPIO_PULL = 0; // reset GPPUD register
	usleep(1);
	GPIO_PULLCLK0 = 0; // remove clock
	usleep(1); // probably unnecessary
	// --------------------------------------------------

	//printf("\nFP on\n");

	while (*((int*)terminate) == 0)
	{
		// prepare for lighting LEDs by setting col pins to output
		for (i=0;i<ncols;i++)
		{	INP_GPIO(cols[i]);			//
			OUT_GPIO(cols[i]);			// Define cols as output
		}
		
		// light up LEDs
		for (i=0;i<nledrows;i++)
		{

			// Toggle columns for this ledrow (which LEDs should be on (CLR = on))
			for (k=0;k<ncols;k++)
			{	if ((ledstatus[i]&(1<<k))==0)
					GPIO_SET = 1 << cols[k];
				else 
					GPIO_CLR = 1 << cols[k];
			}

			// Toggle this ledrow on
			INP_GPIO(ledrows[i]);
			GPIO_SET = 1 << ledrows[i]; // test for flash problem
			OUT_GPIO(ledrows[i]);

			sleep_us(intervl);

			// Toggle ledrow off
			GPIO_CLR = 1 << ledrows[i]; // superstition
			INP_GPIO(ledrows[i]);
			sleep_ns(10000); // waste of cpu cycles but may help against udn2981 ghosting, not flashes though

		}

		// prepare for reading switches
		ms_time(&now_ms);
		for (i=0;i<ncols;i++)
			INP_GPIO(cols[i]);			// flip columns to input. Need internal pull-ups enabled.

		// read three rows of switches
		for (i=0;i<nrows;i++)
		{	INP_GPIO(rows[i]);			
			OUT_GPIO(rows[i]);			// turn on one switch row
			GPIO_CLR = 1 << rows[i];	// and output 0V to overrule built-in pull-up from column input pin

			sleep_us(intervl / 100);

			for (j=0;j<ncols;j++)		// ncols switches in each row
			{	int ss = GPIO_READ(cols[j]);
				debounce_switch(i, j, !!ss, now_ms);
			}

			INP_GPIO(rows[i]);			// stop sinking current from this row of switches
		}

		fflush(stdout);
		gss_initted = 1;
	}

	//printf("\nFP off\n");
	// at this stage, all cols, rows, ledrows are set to input, so elegant way of closing down.

	return 0;
}
