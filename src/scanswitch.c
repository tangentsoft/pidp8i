/*
 * Scan switches for PiDP-8/I front panel
 * 
 * www.obsolescenceguaranteed.blogspot.com
 * 
*/

#include "gpio.h"

#define short_wait() sleep_ms(100)


int main()
{
	int i,j,k,switchscan[2], tmp;
	struct bcm2835_peripheral gpio;
	
	// ------------ Find gpio address (different for Pi 2) -------------

	gpio.addr_p = bcm_host_get_peripheral_address() + 0x200000;

	if (gpio.addr_p== 0x20200000) printf("scanswitch - RPi Plus\n");
	else printf("scanswitch - RPi 2\n");

	if(map_peripheral(&gpio) == -1) 
	{	printf("Failed to map the physical GPIO registers into the virtual memory space.\n");
		return -1;
	}

	// initialise GPIO (all pins used as inputs, with pull-ups enabled on cols)
	for (i=0;i<8;i++)			// Define ledrows as input
		INP_GPIO(ledrows[i]);
	for (i=0;i<12;i++)			// Define cols as input
		INP_GPIO(cols[i]);
	for (i=0;i<3;i++)			// Define rows as input
		INP_GPIO(rows[i]);

	// BCM2835 ARM Peripherals PDF p 101 & elinux.org/RPi_Low-level_peripherals#Internal_Pull-Ups_.26_Pull-Downs
	GPIO_PULL = 2;				// pull-up
	short_wait();       		// must wait 150 cycles
	GPIO_PULLCLK0 = 0x0fff0; 	// selects GPIO pins 4..15 (assumes we avoid pins 2 and 3!)
	short_wait();
	GPIO_PULL = 0;  			// reset GPPUD register
	short_wait();
	GPIO_PULLCLK0 = 0; 			// remove clock
	short_wait(); 	        	// probably unnecessary

	// BCM2835 ARM Peripherals PDF p 101 & elinux.org/RPi_Low-level_peripherals#Internal_Pull-Ups_.26_Pull-Downs
	GPIO_PULL = 0;				// no pull-up no pull-down just float
	short_wait();	        	// must wait 150 cycles
	GPIO_PULLCLK0 = 0x0ff00000;	// selects GPIO pins 20..27
	short_wait();
	GPIO_PULL = 0; 				// reset GPPUD register
	short_wait();
	GPIO_PULLCLK0 = 0; 			// remove clock
	short_wait();       		// probably unnecessary

	// BCM2835 ARM Peripherals PDF p 101 & elinux.org/RPi_Low-level_peripherals#Internal_Pull-Ups_.26_Pull-Downs
	GPIO_PULL = 0;				// no pull-up no pull down just float
	short_wait();	        	// must wait 150 cycles
	GPIO_PULLCLK0 = 0x070000; 	// selects GPIO pins 16..18
	short_wait();
	GPIO_PULL = 0; 				// reset GPPUD register
	short_wait();
	GPIO_PULLCLK0 = 0; 			// remove clock
	short_wait();       		// probably unnecessary
	// --------------------------------------------------


	// prepare for reading switches		

	for (uint8_t row=1;row<=2;row++)		// do rows 2 (for IF switches) and 3 (for STOP switch)
	{		
		INP_GPIO(rows[row]);
		OUT_GPIO(rows[row]);			// turn on one switch row
		GPIO_CLR = 1 << rows[row];		// and output 0V to overrule built-in pull-up from column input pin
	
		sleep_us(10);                   // unnecessarily long?
		switchscan[row-1]=0;

		for (j=0;j<12;j++)		// 12 switches in each row
		{	tmp = GPIO_READ(cols[j]);
			if (tmp==0)
				switchscan[row-1] += 1<<j;
		}
		INP_GPIO(rows[row]);			// stop sinking current from this row of switches
	}

	unmap_peripheral(&gpio);

	if ( ((switchscan[1] >> 6) & 1) == 1 )	// STOP switch enabled,
		return 8;				// 8: STOP enabled, no bootscript
	else
		return (switchscan[0] >> 6) & 07;	// 0-7: x.script to be used in PiDP-8/I
}

