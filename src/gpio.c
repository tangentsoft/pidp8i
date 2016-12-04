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

#define BLOCK_SIZE (4*1024)


struct bcm2835_peripheral gpio;	// needs initialisation

uint16_t switchstatus[3];	// bitfields: 3 rows of up to 12 switches
uint16_t ledstatus[8];		// bitfields: 8 ledrows of up to 12 LEDs


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


// PART 2 - the multiplexing logic driving the front panel -------------

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


void *blink(void *terminate)
{
	int i,j,k, tmp;
	const us_time_t intervl = 300;	// light each row of leds 300 µs

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

	for (i=0;i<8;i++)					// Define ledrows as input
	{	INP_GPIO(ledrows[i]);
		GPIO_CLR = 1 << ledrows[i];		// so go to Low when switched to output
	}
	for (i=0;i<12;i++)					// Define cols as input
	{	INP_GPIO(cols[i]);
	}
	for (i=0;i<3;i++)					// Define rows as input
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
		for (i=0;i<12;i++)
		{	INP_GPIO(cols[i]);			//
			OUT_GPIO(cols[i]);			// Define cols as output
		}
		
		// light up 8 rows of 12 LEDs each
		for (i=0;i<8;i++)
		{

			// Toggle columns for this ledrow (which LEDs should be on (CLR = on))
			for (k=0;k<12;k++)
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
		for (i=0;i<12;i++)
			INP_GPIO(cols[i]);			// flip columns to input. Need internal pull-ups enabled.

		// read three rows of switches
		for (i=0;i<3;i++)
		{
			uint16_t switchscan = 0;

			INP_GPIO(rows[i]);//			GPIO_CLR = 1 << rows[i];	// and output 0V to overrule built-in pull-up from column input pin
			OUT_GPIO(rows[i]);			// turn on one switch row
			GPIO_CLR = 1 << rows[i];	// and output 0V to overrule built-in pull-up from column input pin

			sleep_us(intervl / 100);

			for (j=0;j<12;j++)			// 12 switches in each row
			{	tmp = GPIO_READ(cols[j]);
				if (tmp!=0)
					switchscan += 1<<j;
			}
			INP_GPIO(rows[i]);			// stop sinking current from this row of switches

			switchstatus[i] = switchscan;
		}
	}

	//printf("\nFP off\n");
	// at this stage, all cols, rows, ledrows are set to input, so elegant way of closing down.

	return 0;
}
