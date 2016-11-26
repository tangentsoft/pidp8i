#ifdef PIDP8

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
 
#include <fcntl.h>
#include <stdio.h>

#define BLOCK_SIZE (4*1024)
 
struct bcm2835_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};
 
#endif
