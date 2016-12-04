#if !defined(PIDP8I_GPIO_H)
#define PIDP8I_GPIO_H

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
 
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>

#define BLOCK_SIZE (4*1024)
 
struct bcm2835_peripheral {
    uint32_t addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};

extern uint16_t switchstatus[];
extern uint16_t ledstatus[];

extern void *blink(void *ptr);	// thread entry point to the gpio module
 
#endif // !defined(PIDP8I_GPIO_H)
