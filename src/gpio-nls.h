/*
 * gpio.h: public interface for the PiDP-8/I's GPIO module
 *
 * This file differs from gpio-nls.h in that it does not include the
 # incandescent lamp simulator feature by Ian Schofield.  It is
 * more directly descended from the original gpio.h by Oscar Vermeulen.
 * 
 * Copyright (c) 2015-2017 Oscar Vermeulen and Warren Young
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
*/

#if !defined(PIDP8I_GPIO_H)
#define PIDP8I_GPIO_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
 
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define INP_GPIO(g)   *(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET  *(gpio.addr + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR  *(gpio.addr + 10) // clears bits which are 1 ignores bits which are 0

#define GPIO_READ(g)  *(gpio.addr + 13) &= (1<<(g))

#define GPIO_PULL *(gpio.addr + 37) // pull up/pull down
#define GPIO_PULLCLK0 *(gpio.addr + 38) // pull up/pull down clock

 
struct bcm2835_peripheral {
    uint32_t addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

typedef uint64_t   ns_time_t;
typedef useconds_t us_time_t;
typedef uint32_t   ms_time_t;

extern uint16_t switchstatus[];
extern uint16_t ledstatus[];
extern uint8_t cols[];
extern uint8_t ledrows[];
extern uint8_t rows[];
extern const size_t ncols, nledrows, nrows;

extern void *blink(void *ptr);	// thread entry point to the gpio module
extern unsigned bcm_host_get_peripheral_address(void);
extern int map_peripheral(struct bcm2835_peripheral *p);
extern void sleep_ns(ns_time_t ns);
#define sleep_us(us) usleep(us)
#define sleep_ms(ms) usleep(ms * 1000)
void unmap_peripheral(struct bcm2835_peripheral *p);
 
#endif // !defined(PIDP8I_GPIO_H)
