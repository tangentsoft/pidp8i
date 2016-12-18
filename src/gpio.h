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


// Switch masks, SSn, used against switchstatus[n]
#define SS0_SR_B11 04000
#define SS0_SR_B10 02000
#define SS0_SR_B09 01000
#define SS0_SR_B08 00400
#define SS0_SR_B07 00200
#define SS0_SR_B06 00100
#define SS0_SR_B05 00040
#define SS0_SR_B04 00020
#define SS0_SR_B03 00010
#define SS0_SR_B02 00004
#define SS0_SR_B01 00002
#define SS0_SR_B00 00001

#define SS1_DF_B2  04000
#define SS1_DF_B1  02000
#define SS1_DF_B0  01000
#define SS1_IF_B2  00400
#define SS1_IF_B1  00200
#define SS1_IF_B0  00100

#define SS2_START  04000
#define SS2_L_ADD  02000
#define SS2_DEP    01000
#define SS2_EXAM   00400
#define SS2_CONT   00200
#define SS2_STOP   00100
#define SS2_S_STEP 00040
#define SS2_S_INST 00020
 
// Info for accessing the GPIO peripheral on the SoC
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
extern uint8_t pidp8i_gpio_present;

extern void *blink(void *ptr);	// thread entry point to the gpio module
extern unsigned bcm_host_get_peripheral_address(void);
extern int map_peripheral(struct bcm2835_peripheral *p);
extern void sleep_ns(ns_time_t ns);
#define sleep_us(us) usleep(us)
#define sleep_ms(ms) usleep(ms * 1000)
void unmap_peripheral(struct bcm2835_peripheral *p);
 
#endif // !defined(PIDP8I_GPIO_H)
