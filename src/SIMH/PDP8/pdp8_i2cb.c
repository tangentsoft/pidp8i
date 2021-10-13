/* TODO header*/

/* ---PiDP add--------------------------------------------------------------------------------------------- */
/* well the whole file is new anyway */

/* TODO have a configure option here */ 
/* ignore the entire module if not requested */

#ifdef I2C_BR_DEVICE


#include "pdp8_defs.h"


/* TODO decide which I2C abstraction to use */ 
/* for prototyping we use this one here:
   https://github.com/sstallion/libi2cd

   which was intended for PiDP-11 GPIO
   extender
*/

#include <i2cd.h>

#define I2C_BR_DEV_PATH "/dev/i2c-1"


extern int32 stop_inst;


static uint8 I2C_device_map[64] = {};
static struct i2cd * i2cdev;



int32 i2c_forward_IOT(uint8 i2c_addr, int32 IR, int32 AC, int32 * AC_new_out, int32 * skip_flag_out );
int32 check_i2c_open();

int32 check_i2c_open() {

if (i2cdev == NULL) {
/* Open I2C character device */
    i2cdev = i2cd_open(I2C_BR_DEV_PATH);
    if (i2cdev == NULL) {
        /* TODO logging? but maybe only once */
      return 0;
    }
}

/* TODO configure things like timeouts and retries...
        perhaps we want to have this user configurable even ? */

return 1;
}



int32 i2c_forward_IOT(uint8 i2c_addr, int32 IR, int32 AC, int32 * AC_new_out, int32 * skip_flag_out );
int32 check_i2c_open();




/*
  I2C bridge device added for PiDP8

  No code yet, so let's explain the idea, to be replaced later with a decription of what was actually
  implemented.

  Unlike other SIMH devices, this device would not try to emulate a historic real world device. It would
  rather allow IOT  instructions to be forwaded over I2C to (typically) a microcontroller that would then
  implement the desired logic.

  The user would be able to configure several instances, where each instance maps a PDP-8 device address
  to an address on the I2C bus.

  So let's say we want to implement a PDP-8 device 30 (that's in the range reserved for user applications
  so it should not clash with DEC product device numbers) with a microcontroller on the I2C bas with address
  0x11 .

  Then the instruction
  6301
  would transmit a data packet consisting of the 12 bit instruction code plus 12 bit AC (so 3 bytes)
  to the I2C device 0X11 which would then need to compute anf send back a potentially changed AC value and
  a flag indicating whether to skip the next instruction or not. That's all.


  Complications:
  - Handling errors on the I2C bus
  - No idea how to model devices that cause interrupts in this scheme. Not supported for now.
  - Booting and resetting devices must also be possible, we could do that by transmitting special
    messages that do not start with a "6" for the IOT command

*/


int32 i2c_forward_IOT(uint8 i2c_addr, int32 IR, int32 AC, int32 * AC_new_out, int32 * skip_flag_out ){

unsigned char msg_tx[3];
unsigned char msg_rx[2];

 /* TODO ... do the actual I2C comms */
/* check if i2c device already opened */
check_i2c_open();
/* create tx message ... */
/* send it ... */
/* receive response ...  */
/* parse message ... */

return 0;
}

/* IOT routine */

int32 i2c_bridge (int32 IR, int32 AC)
{

int32 device;
uint8 i2c_addr;
int32 res;
int32 AC_new=AC;
int32 skip_flag=0;

device = (IR >> 3) & 077;


/* lookup I2C device that is configured to handle this device */
i2c_addr=I2C_device_map[device];

res=i2c_forward_IOT(i2c_addr, IR, AC, &AC_new, &skip_flag);



if(res) {
    return ( skip_flag ? IOT_SKP + AC_new : AC_new);
}


/* fall through TODO ill instr is probably not we want here but for prototyping it's ok */

return (stop_inst << IOT_V_REASON) + AC;                /* ill inst */

}


#endif

/* ---PiDP end---------------------------------------------------------------------------------------------- */

