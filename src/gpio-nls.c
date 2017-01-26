/*
 * gpio-nls.c: implements blink_core() with the original simple LED driver
 *
 * This file differs from gpio.c in that it does not include the
 * incandescent lamp simulator feature by Ian Schofield.  It is
 * more directly descended from the original gpio.c by Oscar Vermeulen.
 * 
 * Copyright © 2015-2017 Oscar Vermeulen and Warren Young
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


//// blink_core ////////////////////////////////////////////////////////
// The GPIO module's main loop core, called from thread entry point in
// gpio-common.c.

void blink_core(struct bcm2835_peripheral* pgpio, int* terminate)
{
    int i, j, k;
    const us_time_t intervl = 300;  // light each row of leds 300 µs
    ms_time_t now_ms;

    while (*terminate == 0) {
        // prepare for lighting LEDs by setting col pins to output
        for (i = 0; i < ncols; i++) {
            INP_GPIO(cols[i]);
            OUT_GPIO(cols[i]);          // Define cols as output
        }
        
        // light up LEDs
        for (i = 0; i < nledrows; i++) {
            // Toggle columns for this ledrow (which LEDs should be on (CLR = on))
            for (k = 0; k <ncols; k++) {
                if ((ledstatus[i] & (1 << k)) == 0)
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
        for (i = 0; i < ncols; i++)
            INP_GPIO(cols[i]);          // flip columns to input. Need internal pull-ups enabled.

        // read three rows of switches
        for (i = 0; i < nrows; i++) {
            INP_GPIO(rows[i]);          
            OUT_GPIO(rows[i]);          // turn on one switch row
            GPIO_CLR = 1 << rows[i];    // and output 0V to overrule built-in pull-up from column input pin

            sleep_us(intervl / 100);

            for (j = 0; j < ncols; j++) {      // ncols switches in each row
                int ss = GPIO_READ(cols[j]);
                debounce_switch(i, j, !!ss, now_ms);
            }

            INP_GPIO(rows[i]);          // stop sinking current from this row of switches
        }

        fflush(stdout);
        gss_initted = 1;

#if defined(HAVE_SCHED_YIELD)
        sched_yield();
#endif
    }
}
