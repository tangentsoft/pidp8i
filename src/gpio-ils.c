/*
 * gpio-ils.c: implements blink_core() for Ian Schofield's incandescent
 *             lamp simulator
 * 
 * Copyright © 2015-2017 Oscar Vermeulen, Ian Schofield, and Warren Young
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS LISTED ABOVE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the names of the authors above shall
 * not be used in advertising or otherwise to promote the sale, use or other
 * dealings in this Software without prior written authorization from those
 * authors.
 *
 * www.obsolescenceguaranteed.blogspot.com
*/

#include "gpio-common.h"

#include "sim_defs.h"


//// CONSTANTS /////////////////////////////////////////////////////////

// Brightness range is [0, MAX_BRIGHTNESS) truncated.
#define MAX_BRIGHTNESS 32

// On each iteration, we add or subtract a proportion of the current LED
// value back to it as its new brightness, based on the LED's current
// internal state.  This gives a nonlinear increase/decrease behavior,
// where rising from "off" or dropping from "full-on" is fast to start
// and slows down as it approaches its destination.
#define DECAY 0.01


//// blink_core ////////////////////////////////////////////////////////
// The GPIO module's main loop core, called from thread entry point in
// gpio-common.c.

void blink_core(struct bcm2835_peripheral* pgpio, int* terminate)
{
    int i, j, k;
    const us_time_t intervl = 5;    // light each row of leds 5 µs
    ms_time_t now_ms;

	float brtval[96];
	uint8 brctr[96], bctr = 0, ndx;
	memset(brtval, 0, sizeof (brtval));

    while (*terminate == 0) {
        // prepare for lighting LEDs by setting col pins to output
        for (i = 0; i < ncols; i++) {
            INP_GPIO(cols[i]);
            OUT_GPIO(cols[i]);          // Define cols as output
        }
        if (bctr == 0) {
			memset(brctr, 0, sizeof (brctr));
            bctr = MAX_BRIGHTNESS;
        }

		// Increase or decrease each LED's brightness based on whether
		// it is currently enabled.  These values affect the duty cycle
		// of the signal put out by the GPIO thread to each LED, thus
		// controlling brightness.
		float *p = brtval;
		for (int row = 0; row < nledrows; ++row) {
			for (int col = 0, msk = 1; col < ncols; ++col, ++p, msk <<= 1) {
				if (ledstatus[row] & msk)
					*p += (MAX_BRIGHTNESS - *p) * DECAY;
				else
					*p -= *p * DECAY;
			}
		}

        // light up LEDs
        for (i = ndx = 0; i < nledrows; i++) {
            // Toggle columns for this ledrow (which LEDs should be on (CLR = on))
            for (k = 0; k < ncols; k++, ndx++) {
                if (++brctr[ndx] < brtval[ndx])
                    GPIO_CLR = 1 << cols[k];
                else
                    GPIO_SET = 1 << cols[k];
            }

            // Toggle this ledrow on
            INP_GPIO(ledrows[i]);
            GPIO_SET = 1 << ledrows[i]; // test for flash problem
            OUT_GPIO(ledrows[i]);

            sleep_us(intervl);

            // Toggle ledrow off
            GPIO_CLR = 1 << ledrows[i]; // superstition
            INP_GPIO(ledrows[i]);

            sleep_us(intervl);
        }

        // prepare for reading switches
        ms_time(&now_ms);
        for (i = 0; i < ncols; i++) {
            INP_GPIO(cols[i]);          // flip columns to input. Need internal pull-ups enabled.
        }

        // read three rows of switches
        for (i = 0; i < nrows; i++) {
            INP_GPIO(rows[i]);          
            OUT_GPIO(rows[i]);          // turn on one switch row
            GPIO_CLR = 1 << rows[i];    // and output 0V to overrule built-in pull-up from column input pin

            sleep_ns(intervl * 1000 / 100);

            for (j = 0; j < ncols; j++) {   // ncols switches in each row
                int ss = GPIO_READ(cols[j]);
                debounce_switch(i, j, !!ss, now_ms);
            }

            INP_GPIO(rows[i]);          // stop sinking current from this row of switches
        }

        fflush(stdout);
        gss_initted = 1;
        bctr--;

#if defined(HAVE_SCHED_YIELD)
        sched_yield();
#endif
    }
}
