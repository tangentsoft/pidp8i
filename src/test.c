/* test.c - Front panel LED and switch testing program, built as pidp8i-test

   Copyright © 2016-2017 Paul R. Bernard and Warren Young

   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
   THE AUTHORS LISTED ABOVE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the names of the authors above shall
   not be used in advertising or otherwise to promote the sale, use or other
   dealings in this Software without prior written authorization from those
   authors.
*/

#include "gpio-common.h"

#include <assert.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned int uint32;
typedef unsigned char uint8;

static uint16_t lastswitchstatus[3];    // to watch for switch changes

uint8 path[] = { 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x11, 0x12, 0x13, 0x14, 0x15,
    0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x2c, 0x2b, 0x2a, 0x29,
    0x28, 0x27, 0x26, 0x25, 0x24, 0x23, 0x22, 0x21, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x4c, 0x4b,
    0x4a, 0x49, 0x48, 0x47, 0x46, 0x45, 0x44, 0x43, 0x42, 0x41, 0x87,
    0x76, 0x77, 0x78, 0x79, 0x7a, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56,
    0x57, 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x68, 0x67, 0x66, 0x65, 0x64,
    0x63, 0x62, 0x61, 0x69, 0x6a, 0x6b, 0x6c, 0x71, 0x72, 0x75, 0x74,
    0x73
};

#define DISPLAY_HOLD_ITERATIONS 10
#define DISPLAY_HOLD \
        sleep_us (500 * 1000); \
        putchar('.');


int main (int argc, char *argv[])
{
    pthread_t thread1;
    int iret1, row, col, i, chr, dhi;
    int path_idx = 0, led_row = 0, delay = 0;

    // Tell the GPIO thread we're updating the display via direct
    // ledstatus[] manipulation instead of set_pidp8i_leds calls.
    pidp8i_simple_gpio_mode = 1;

    assert (sizeof (lastswitchstatus == switchstatus));

    // create GPIO thread
    int terminate = 0;
    iret1 = pthread_create (&thread1, NULL, gpio_thread, &terminate);
    if (iret1) {
        fprintf (stderr, "Error creating thread, return code %d\n", iret1);
        exit (EXIT_FAILURE);
    }
    sleep (2);                  // allow 2 sec for multiplex to start
    if (!pidp8i_gpio_present) {
        fprintf (stderr, "Cannot run the test while another PiDP-8/I program runs.\n");
        exit (EXIT_FAILURE);
    }

    printf ("\n\nTurn on ALL LEDs");
    for (dhi = 0; dhi < DISPLAY_HOLD_ITERATIONS; ++dhi) {
        for (row = 0; row < NLEDROWS; ++row)
            ledstatus[row] = 07777;
        DISPLAY_HOLD;
    }

    printf ("\n\nTurn off ALL LEDs");
    for (dhi = 0; dhi < DISPLAY_HOLD_ITERATIONS; ++dhi) {
        for (row = 0; row < NLEDROWS; ++row)
            ledstatus[row] = 0;
        DISPLAY_HOLD;
    }

    puts ("");
    for (row = 0; row < NLEDROWS; ++row) {
        printf ("\nTurning on LED row %d", row);

        for (dhi = 0; dhi < DISPLAY_HOLD_ITERATIONS / 8; ++dhi) {
            ledstatus[row] = 07777;
            DISPLAY_HOLD;
        }

        ledstatus[row] = 0;
    }

    puts ("");
    for (col = 0; col < NCOLS; ++col) {
        printf ("\nTurning on LED col %d", col);

        for (dhi = 0; dhi < DISPLAY_HOLD_ITERATIONS / 8; ++dhi) {
            for (row = 0; row < NLEDROWS; ++row)
                ledstatus[row] |= 1 << col;
            DISPLAY_HOLD;
        }

        for (row = 0; row < NLEDROWS; ++row)
            ledstatus[row] = 0;
    }

    printf ("\n\nReading the switches.  Toggle any pattern desired.  "
            "CTRL-C to quit.\n");

    for (i = 0; i < NROWS; ++i)
        lastswitchstatus[i] = switchstatus[i];

    for (;;) {
        if (delay++ >= 30) {
            delay = 0;

            ledstatus[led_row] = 0;
            ledstatus[led_row = (path[path_idx] >> 4) - 1] = 04000 >> ((path[path_idx] & 0x0F) - 1);
            sleep_us (1);

            if (++path_idx >= sizeof (path) / sizeof (path[0]))
                path_idx = 0;
        }

        if (lastswitchstatus[0] != switchstatus[0] ||
            lastswitchstatus[1] != switchstatus[1] || lastswitchstatus[2] != switchstatus[2]) {
            for (i = 0; i < NROWS; ++i) {
                printf ("%04o ", ~switchstatus[i] & 07777);
                lastswitchstatus[i] = switchstatus[i];
            }
            printf ("\n");
        }
        usleep (1000);
    }

    terminate = 1;
    if (pthread_join (thread1, NULL))
        printf ("\r\nError joining multiplex thread\r\n");
    return 0;
}
