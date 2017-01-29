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

#include "PDP8/pidp8i.h"


//// blink_core ////////////////////////////////////////////////////////
// The GPIO module's main loop core, called from thread entry point in
// gpio-common.c.

void blink_core(struct bcm2835_peripheral* pgpio, int* terminate)
{
    const us_time_t intervl = 300;  // light each row of leds 300 µs
    ms_time_t now_ms;

    while (*terminate == 0) {
        // Prepare for lighting LEDs by setting col pins to output
        for (size_t i = 0; i < NCOLS; ++i) OUT_GPIO(cols[i]);
  
        // Go get the current LED "on" times, and give the SIMH CPU
        // thread a blank copy to begin updating.
        const size_t inst_count = swap_displays();

        // Here in the NLS version, the "on" threshold is half the
        // instruction count, meaning we turn the LED on if the bit
        // controlling that LED was set at least half the time since the
        // last display update.  Don't do it if inst_count == 1 because
        // that mans there was only one display update since the last
        // iteration of this loop.  That can happen at low simulator
        // throttle values or when called from pidp8i-test.
        const int on_threshold = inst_count > 1 ? inst_count >> 1 : 1;

        // Halve the Execute and Fetch values, because they're only on
        // for half of the instruction fetch-and-execute cycle.  Skip
        // halving for == 1 case for same reason as above.
        if (pdis_paint->on[5][2] > 1) pdis_paint->on[5][2] >>= 1;
        if (pdis_paint->on[5][3] > 1) pdis_paint->on[5][3] >>= 1;
        
        // Light up LEDs
        for (size_t row = 0; row < NLEDROWS; ++row) {
            // Output 0 (CLR) for LEDs in this row which should be on
            size_t *prow = pdis_paint->on[row];
            for (size_t col = 0; col < NCOLS; ++col) {
                if (prow[col] >= on_threshold) {
                    GPIO_CLR = 1 << cols[col];
                }
                else {
                    GPIO_SET = 1 << cols[col];
                }
            }

            // Toggle this LED row on
            INP_GPIO(ledrows[row]);
            GPIO_SET = 1 << ledrows[row];
            OUT_GPIO(ledrows[row]);

            sleep_us(intervl);

            // Toggle this LED row off
            GPIO_CLR = 1 << ledrows[row]; // superstition
            INP_GPIO(ledrows[row]);

            // Small delay to reduce UDN2981 ghosting
            sleep_ns(10000);
        }

        read_switches(intervl);
#if defined(HAVE_SCHED_YIELD)
        sched_yield();
#endif
    }
}
