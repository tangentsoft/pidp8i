/*
 * gpio-ils.c: implements gpio_core () for Ian Schofield's incandescent
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
#include <math.h>
#include <unistd.h>
#include <pidp8i.h>
#include <sim_defs.h>
#include "pinctrl/gpiolib.h"

#include <time.h>

//// CONSTANTS /////////////////////////////////////////////////////////

// Brightness range is [0, MAX_BRIGHTNESS] truncated.
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

// default values that fit for a Raspberry 2B
#define RISING_FACTOR_DEFAULT  0.2775
#define FALLING_FACTOR_DEFAULT 0.0975

//// gpio_core  ////////////////////////////////////////////////////////
// The GPIO module's main loop core, called from thread entry point in
// gpio-common.c.


static int is_init = 0;

static float RISING_FACTOR = RISING_FACTOR_DEFAULT;
static float FALLING_FACTOR = FALLING_FACTOR_DEFAULT;

void gpio_core (int* terminate)
{
    // The ILS version uses an iteration rate a few times faster than the NLS
    // version, depending on execution speed of the Raspberry Pi.
    // The overall refresh rate sould be between 100 and a few 100 per sec.
    const us_time_t intervl = 10;

    // on-time for each brightness level increment, allows to stretch or
    // shrink the dynamic range of the brightness levels
    // the first choice results in more distinguishable brightness levels.
    // the second, commented out line is the legacy implementation with equal delay times per
    //             brightness level. You can check the difference visually with the "test_pattern" 
    //             feature below

    static us_time_t intervl_loop[MAX_BRIGHTNESS+1] = {1,1,1,1,1,1,1,2,2,2,3,3,3,4,5,5,6,7,7,8,10,11,12,14,15,17,20,22,25,28,31,35,40};
//  static us_time_t intervl_loop[MAX_BRIGHTNESS+1] = {10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10};

    const float test_pattern[]={0,3,6,9,12,15,17,20,23,26,29,32};

    // Current brightness level for each LED.  It goes from 0 to
    // MAX_BRIGHTNESS, but we keep it as a float because the decay
    // function smoothly ramps from the current value to the ever-
    // changing target value.
    float brightness[NLEDROWS][NCOLS];
    static float filtered_cycle_ms = 0;
    static long timing_cnt = 0;
    static struct timespec last_timing_stamp;
    static int manual_ILS_tweaking = 0;
    static int test_pattern_active = 0;

    // execute only once: check if we get manual ILS tweaking values from the env. variable
    // PIDP8I_ILS_TWEAK or PIDP8I_ILS_RAMP

    if(! is_init) {
        char* tweaking = getenv("PIDP8I_ILS_TWEAK");
        char* tweaking_ramp = getenv("PIDP8I_ILS_RAMP");
        char* tweaking_test = getenv("PIDP8I_ILS_TEST");

        if (tweaking) {
    	  int n = sscanf(tweaking, "%f,%f",&RISING_FACTOR,&FALLING_FACTOR);
          if (n!=2) {
            RISING_FACTOR = RISING_FACTOR_DEFAULT;
            FALLING_FACTOR = FALLING_FACTOR_DEFAULT;
          } else {
            manual_ILS_tweaking=1;
          }
        }
        if (tweaking_ramp) {
          us_time_t iv[MAX_BRIGHTNESS+1];
          int n = sscanf(tweaking_ramp, "%d,%d,%d,%d,%d,%d,%d,%d,"
                                   "%d,%d,%d,%d,%d,%d,%d,%d,"
                                   "%d,%d,%d,%d,%d,%d,%d,%d,"
                                   "%d,%d,%d,%d,%d,%d,%d,%d,%d",
                                   iv+0,iv+1,iv+2,iv+3,iv+4,iv+5,iv+6,iv+7,
                                   iv+8,iv+9,iv+10,iv+11,iv+12,iv+13,iv+14,iv+15,
                                   iv+16,iv+17,iv+18,iv+19,iv+20,iv+21,iv+22,iv+23,
                                   iv+24,iv+25,iv+26,iv+27,iv+28,iv+29,iv+30,iv+31,iv+32);
          if( n == MAX_BRIGHTNESS+1 ) {
            for (n=0; n <= MAX_BRIGHTNESS; n++) {
               intervl_loop[n]=iv[n];
            }
          }
        }

        test_pattern_active=(tweaking_test!=NULL);

        is_init=1;
    }

    memset(brightness, 0, sizeof (brightness));

    // Brightness target for each LED, updated at the start of each PWM
    // cycle when we get fresh "on" counts from the CPU thread.
    uint8 br_targets[NLEDROWS][NCOLS];
    memset(br_targets, 0, sizeof (br_targets));


    while (*terminate == 0) {
    	// Prepare for lighting LEDs by setting col pins to output
	for (size_t i = 0; i < NCOLS; ++i) {
		gpio_set_fsel(cols[i], GPIO_FSEL_OUTPUT);
        }

        // Go get the current LED "on" times, and give the SIMH
        // CPU thread a blank copy to begin updating.  Because we're
        // in control of the swap timing, we don't need to copy the
        // pdis_paint pointer: it points to the same thing between
        // these swap_displays() calls.
        swap_displays();

        // Recalculate the brightness target values based on the
        // "on" counts in *pdis_paint and the quantized brightness
        // level, which is based on the number of instructions
        // executed for this display update.

        const size_t inst_count =
              (pdis_paint->inst_count >0) ? pdis_paint->inst_count : 1;
        const float one_div_inst_count = 1.0 / (float) inst_count;
        for (int row = 0; row < NLEDROWS; ++row) {
            size_t *prow = pdis_paint->on[row];
            for (int col = 0; col < NCOLS; ++col) {
                // this gives range from [0 .. 32] incl (!)
                // using ceil will boost even low but nonzero counts into
                // brightness bin no 1 => 1 short pulse
                br_targets[row][col] = ceilf((prow[col] << 5) * one_div_inst_count);
            }
        }

        // Hard-code the Fetch and Execute brightnesses; in running
        // mode, they're both on half the instruction time, so we
        // just set them to 50% brightness.  Execute differs in STOP
        // mode, but that's handled in update_led_states () because
        // we fall back to NLS in STOP mode.
        br_targets[5][2] = br_targets[5][3] = MAX_BRIGHTNESS / 2;

        // Update the brightness values.
        for (int row = 0; row < NLEDROWS; ++row) {
            //size_t *prow = pdis_paint->on[row];
            for (int col = 0; col < NCOLS; ++col) {
                uint8 br_target = br_targets[row][col];
                float *p = brightness[row]+col;
                if (*p <= br_target) {
                    *p += (br_target - *p) * RISING_FACTOR;
                }
                else {
                    *p -= (*p - br_target) * FALLING_FACTOR;
                }
            }
        }

        // tweaking support: show test pattern of brightness ramp
        if (test_pattern_active) {
            // fill one panel row of 12 lights with test pattern
            for (int col=0; col < 12 ; col++) {
                brightness[2][col]=test_pattern[col];
            }
        }

        // Light up LEDs
        extern int swStop, swSingInst, suppressILS;
        if (swStop || swSingInst || suppressILS) {

	    // The CPU is in STOP mode or someone has suppressed the ILS,
            // so show the current LED states full-brightness using the
            // same mechanism NLS uses.  Force a display swap if the next
            // in case this isn't STOP mode.
            update_led_states (intervl * 60);
            swap_displays();
        }
        else {
            // Normal case: PWM display using the on-count values
             for (size_t row = 0; row < NLEDROWS; ++row) {
                // Output 0 (CLR) for LEDs in this row which should be on
                // size_t *prow = pdis_paint->on[row];
                uint8 br[ NCOLS ];

                // for each row, perfrom one PWM cycle

                // convert brightness as integers just once
                // and not for each pulse width step

		for (size_t col = 0; col < NCOLS; col++ ) {
                   int b=(int)brightness[row][col];
                   if (b < 0)  b=0;
                   if (b > MAX_BRIGHTNESS) b=MAX_BRIGHTNESS;
                   br[col]=b;
                }

                // Toggle this LED row on
                // Note: The legacy code always switched pins to INPUT
                //       before changing their output level, for unknown 
                //        reasons, we do the same here.

                gpio_set_fsel(ledrows[row], GPIO_FSEL_INPUT);
                gpio_set_drive(ledrows[row], DRIVE_HIGH);
                gpio_set_fsel(ledrows[row], GPIO_FSEL_OUTPUT);

                // initial switching : on or off

                for (size_t col = 0; col < NCOLS; ++col) {
                    if (br[col] > 0) {
                        gpio_set_drive(cols[col], DRIVE_LOW);
                    } else {
                        gpio_set_drive(cols[col], DRIVE_HIGH);
                    }
                }

                for(int steps = 1; steps <= MAX_BRIGHTNESS  ; ++steps) {
                  sleep_us(intervl_loop[steps]);
                    // turn off those LEDS that are in the respective
                    // brightness bin, leave the others on
                    // creating a pulse width proportional to
                    // br[col]
                  for (size_t col = 0; col < NCOLS; ++col) {
                    if (br[col] == steps) {
                        gpio_set_drive(cols[col], DRIVE_HIGH);
                    }
                  }
		}

		// at this point every LED must be turned off again
                // as we stepped thru all possible values of br[col]


                // Toggle this LED row off
                gpio_set_drive(ledrows[row], DRIVE_LOW);
                gpio_set_fsel(ledrows[row], GPIO_FSEL_INPUT);

                sleep_ns(10);
            } //  end row loop
        } // end normal PWM case

#if 0   // debugging
        static time_t last = 0, now;
        if (time(&now) != last) {
            float* p = brightness[0];
            #define B(n) (p[n] / MAX_BRIGHTNESS * 100.0)
            printf("\r\nPC:"
                    " [%3.0f%%][%3.0f%%][%3.0f%%]"
                    " [%3.0f%%][%3.0f%%][%3.0f%%]"
                    " [%3.0f%%][%3.0f%%][%3.0f%%]"
                    " [%3.0f%%][%3.0f%%][%3.0f%%]",
                    B(11), B(10), B(9),
                    B(8),  B(7),  B(6),
                    B(5),  B(4),  B(3),
                    B(2),  B(1),  B(0));
            last = now;
        }
#endif

        // Same value as in NLS simulator, see
        // in gpio-nls.c.
        read_switches(12000);

        // unless the FALLING_FACTOR AND RISING_FACTOR are explicitly set
        // by the user externally, we try to self-calibrate those values
        // once every 1000 cycles which should be about once every few seconds,
        // roughly

        if (!manual_ILS_tweaking && timing_cnt++ == 1000){
            struct timespec time_now;
            clock_gettime(CLOCK_MONOTONIC,&time_now);
            if(last_timing_stamp.tv_sec || last_timing_stamp.tv_nsec) {
		long diff_sec = time_now.tv_sec - last_timing_stamp.tv_sec;
		double diff_ns =  1e9*diff_sec + (double) (time_now.tv_nsec - last_timing_stamp.tv_nsec);
                double cycle_ms = diff_ns/1e9;
#if 0
                printf("\n\rTime per iteration %f ms\n\r", cycle_ms);
#endif
		// is this value plausible?
		if(cycle_ms >0.5 && cycle_ms < 50.0) {
		   // adjust the RISING_FACTOR and FALLING_FACCTOR
		   // the defaults are calibrated for a ca 7ms refresh time
                   filtered_cycle_ms=(filtered_cycle_ms==0) ? cycle_ms : filtered_cycle_ms * 0.7 + cycle_ms * 0.3;
                   RISING_FACTOR  = 1.0L - pow((1.0L - RISING_FACTOR_DEFAULT) , filtered_cycle_ms / 7.0);
                   FALLING_FACTOR = 1.0L - pow((1.0L - FALLING_FACTOR_DEFAULT), filtered_cycle_ms / 7.0);
		}


            }
            last_timing_stamp=time_now;
            timing_cnt=0;
        };


#if defined(HAVE_SCHED_YIELD)
        sched_yield();
#endif
    }

}
