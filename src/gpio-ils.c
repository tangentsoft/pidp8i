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

#include "PDP8/pidp8i.h"

#include <math.h>


//// CONSTANTS /////////////////////////////////////////////////////////

// Brightness range is [0, MAX_BRIGHTNESS) truncated.
#define MAX_BRIGHTNESS 32

// On each iteration, we add or subtract a proportion of the LED's "on"
// time back to it as its new brightness, so that it takes several
// iterations at that same "on" time for the LED to achieve that
// brightness level.  Because the delta is based on the prior value, we
// get nonlinear asymptotic increase/decrease behavior.
//
// We use an asymmetric function depending on whether the LED is turning
// on or off to better mimic the behavior of an incandescent lamp, which
// reaches full brightness faster than it turns fully off.
#define RISING_FACTOR 0.2
#define FALLING_FACTOR 0.08

// If the CPU core is executing a static mix of instructions, the actual
// brightness will eventually get close enough to the ideal brightness
// that there's no point continuing to adjust the value because we'll
// just end up oscillating around the ideal value.  If that oscillation
// happens to cross one of the brightness quantization thresholds, the
// LED will pulse when it should be rock-steady.
//
// Therefore, once we're within half of a brightness step, leave it be.
#define BRIGHTNESS_EPSILON (1.0 / MAX_BRIGHTNESS / 2.0)


//// blink_core ////////////////////////////////////////////////////////
// The GPIO module's main loop core, called from thread entry point in
// gpio-common.c.

void blink_core(struct bcm2835_peripheral* pgpio, int* terminate)
{
    // The ILS version uses an iteration rate 60x faster than the NLS
    // version because we have to get through 32 PWM steps, each of
    // which takes roughly 2 * intervl µs.  There's a bit of extra delay
    // over intervl in the NLS version, so the while loop iteration time
    // is about the same for both versions.
    const us_time_t intervl = 5;

	float brtval[96];
	uint8 bctr = MAX_BRIGHTNESS;
	memset(brtval, 0, sizeof (brtval));

    while (*terminate == 0) {
        // Prepare for lighting LEDs by setting col pins to output
        for (size_t i = 0; i < NCOLS; ++i) OUT_GPIO(cols[i]);

        // Restart PWM cycle if prior one is complete
        if (bctr == MAX_BRIGHTNESS) {
            // Reset PWM step counter
            bctr = 0;
      
            // Go get the current LED "on" times, and give the SIMH
            // CPU thread a blank copy to begin updating.  We save the
            // instructions-executed count as a float to avoid a cast in
            // the calculation that uses it, where we want an FP result.
            const float inst_count = swap_displays();

            // Update the brightness values used in the next PWM cycle.
            // Increase the LED's brightness where the target brightness
            // is greater than the actual brightness, else decrease it.
            float *p = brtval;
            for (int row = 0; row < NLEDROWS; ++row) {
                size_t *prow = pdis_paint->on[row];
                for (int col = 0; col < NCOLS; ++col, ++p) {
                    float new_brtval = prow[col] / inst_count;
                    if (fabs(new_brtval - *p) > BRIGHTNESS_EPSILON) {
                        if (new_brtval > *p) {
                            *p += (MAX_BRIGHTNESS - *p) * RISING_FACTOR;
                        }
                        else {
                            *p -= *p * FALLING_FACTOR;
                        }
                    }
                }
            }
        }
        ++bctr;

        // Halve the Execute and Fetch values, because they're only on
        // for half of the instruction fetch-and-execute cycle.  We have
        // to do this in FP math in case inst_count == 1, because only
        // one display update happened since the last iteration.
        float execute = pdis_paint->on[5][2] / 2.0;
        float fetch   = pdis_paint->on[5][3] / 2.0;

        // Light up LEDs
        for (size_t row = 0, ndx = 0; row < NLEDROWS; ++row) {
            // Output 0 (CLR) for LEDs in this row which should be on
            size_t *prow = pdis_paint->on[row];
            for (size_t col = 0; col < NCOLS; ++col, ++ndx) {
                if (brtval[ndx] >= bctr) {
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

            sleep_us(intervl);
        }

        read_switches(intervl);
#if defined(HAVE_SCHED_YIELD)
        sched_yield();
#endif
    }
}
