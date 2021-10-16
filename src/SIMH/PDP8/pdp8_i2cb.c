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


int32 i2c_bridge (int32 IR, int32 AC);
t_stat i2cb_show_dev (FILE *st, UNIT *uptr, int32 val, CONST void *desc);
t_stat i2cb_set_dev (UNIT *uptr, int32 val, CONST char *cptr, void *desc);

t_stat i2cb_show_addr (FILE *st, UNIT *uptr, int32 val, CONST void *desc);
t_stat i2cb_set_addr (UNIT *uptr, int32 val, CONST char *cptr, void *desc);


#define I2CB_MAX 8                               /* max number of bridges */


/* Initial device numbers for 8 bridges, reserved for customer
   applications by DEC so conflict free wrt DEC products */


#define DEV_I2CB0 030
#define DEV_I2CB1 031
#define DEV_I2CB2 032
#define DEV_I2CB3 033
#define DEV_I2CB4 034
#define DEV_I2CB5 035
#define DEV_I2CB6 036
#define DEV_I2CB7 037



DIB_DSP i2cb_dsp[I2CB_MAX] = {
    { DEV_I2CB0,  &i2c_bridge},
    { DEV_I2CB1,  &i2c_bridge},
    { DEV_I2CB2,  &i2c_bridge},
    { DEV_I2CB3,  &i2c_bridge},
    { DEV_I2CB4,  &i2c_bridge},
    { DEV_I2CB5,  &i2c_bridge},
    { DEV_I2CB6,  &i2c_bridge},
    { DEV_I2CB7,  &i2c_bridge}
    };





REG i2cb_reg[] = {
    { NULL } /* TODO ?? any registers that we need? */
    };



MTAB i2cb_mod[] = {
    { MTAB_XTD|MTAB_VUN|MTAB_VALR, 0, "I2C address", "I2CADDR",
      &i2cb_set_addr, &i2cb_show_addr, NULL },
    { MTAB_XTD|MTAB_VUN|MTAB_VALR, 0, "DEVNO", "DEVNO",
      &i2cb_set_dev, &i2cb_show_dev, NULL },
    { 0 }
    };





const char *i2cb_description (DEVICE *dptr);



/* TODO move into unit specific data, but this is faster so maybe
   regenerate this table each time the dev or addr is changed 
   for a unit */
 
static uint8 I2C_device_map[64] = {};
static struct i2cd * i2cdev;


t_stat i2cb_reset(DEVICE *dp);                /* reset routine */
t_stat i2cb_boot(int32 u, DEVICE *dp);         /* boot routine */
t_stat i2cb_attach(UNIT *up, CONST char *cp);  /* attach routine */
t_stat i2cb_detach(UNIT *up);                     /* detach routine */


DIB i2cb_dib = { DEV_I2CB0, I2CB_MAX, { NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL }, i2cb_dsp };


/* TODO for now, all "action" i taking place in the IOT dipatch
   routines, so no unit service code is needed , and also no
   registers. Will change with interrupt support?? */



UNIT i2cb_unit[] = {
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) },
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) },
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) },
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) },
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) },
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) },
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) },
    { UDATA (NULL, UNIT_ATTABLE+UNIT_DISABLE, 0) }
    };



DEVICE i2cb_dev = {
    "I2CB", i2cb_unit, i2cb_reg, i2cb_mod,
    8, 10, 31, 1, 8, 12,
    NULL, NULL, &i2cb_reset,
    &i2cb_boot, &i2cb_attach, &i2cb_detach,
    &i2cb_dib, DEV_DISABLE, 0,
    NULL, NULL, NULL, NULL, NULL, NULL,
    &i2cb_description
    };







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
  so it should not clash with DEC product device numbers) with a microcontroller on the I2C bus with address
  (say) 0x11.

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

const char *i2cb_description (DEVICE *dptr)
{
return "I2C bridge prototyping device";
}

t_stat i2cb_reset(DEVICE *dp) {
/* TODO */
  return SCPE_OK;
}

t_stat i2cb_boot(int32 u, DEVICE *dp){
/* TODO */
  return SCPE_OK;
}



t_stat i2cb_attach(UNIT *up, CONST char *cp){
/* TODO */
  return SCPE_OK;
}


t_stat i2cb_detach(UNIT *up){
/* TODO */
  return SCPE_OK;
}


/* set device number for the specific unit (!) not the device */

t_stat i2cb_set_dev (UNIT *uptr, int32 val, CONST char *cptr, void *desc)
{
DEVICE *dptr;
DIB *dibp;
uint32 newdev;
t_stat r;
uint32 i;

if (cptr == NULL)
    return SCPE_ARG;
if (uptr == NULL)
    return SCPE_IERR;
dptr = find_dev_from_unit (uptr);
if (dptr == NULL)
    return SCPE_IERR;
dibp = (DIB *) dptr->ctxt;
if (dibp == NULL)
    return SCPE_IERR;

/* find unit index */
for (i=0; i < dibp->num ; i++) {
    if  (dptr->units +i  == uptr) 
        break;
}
if(i == dibp->num) {
    return SCPE_IERR;
}

newdev = get_uint (cptr, 8, DEV_MAX - 1, &r);           /* get new */
if ((r != SCPE_OK) || (newdev == dibp->dsp_tbl[i].dev))
    return r;
dibp->dsp_tbl[i].dev = newdev;                          /* store */
return SCPE_OK;
}



t_stat i2cb_show_dev (FILE *st, UNIT *uptr, int32 val, CONST void *desc)
{
DEVICE *dptr;
DIB *dibp;
uint32 i;

if (uptr == NULL)
    return SCPE_IERR;
dptr = find_dev_from_unit (uptr);
if (dptr == NULL)
    return SCPE_IERR;
dibp = (DIB *) dptr->ctxt;
if (dibp == NULL)
    return SCPE_IERR;

/* find unit index */ 
for (i=0; i < dibp->num ; i++) {
    if  (dptr->units + i  == uptr) 
        break;
}
if(i == dibp->num) {
    return SCPE_IERR;
}

fprintf (st, "devno=%02o", dibp->dsp_tbl[i].dev);

return SCPE_OK;
}



/* Set I2C addr */

t_stat i2cb_set_addr (UNIT *uptr, int32 val, CONST char *cptr, void *desc)
{

DEVICE *dptr;
DIB *dibp;
uint32 i;
t_stat r;
uint32 addr;
if (uptr == NULL)
    return SCPE_IERR;
dptr = find_dev_from_unit (uptr);
if (dptr == NULL)
    return SCPE_IERR;
dibp = (DIB *) dptr->ctxt;
if (dibp == NULL)
    return SCPE_IERR;

/* find unit index */ 
for (i=0; i < dibp->num ; i++) {
    if  (dptr->units + i  == uptr) 
        break;
}
if(i == dibp->num) {
    return SCPE_IERR;
}

if ((cptr == NULL) || (*cptr == 0))
    return SCPE_ARG;
if (uptr->flags & UNIT_ATT)
    return SCPE_ALATT;

addr =  get_uint (cptr, 16, 63 , &r);
if (r != SCPE_OK)
    return SCPE_ARG;
I2C_device_map[dibp->dsp_tbl[i].dev]=(uint8) addr;
return SCPE_OK;
}

/* Show I2C address */

t_stat i2cb_show_addr (FILE *st, UNIT *uptr, int32 val, CONST void *desc)
{
DEVICE *dptr;
DIB *dibp;
uint32 i;

if (uptr == NULL)
    return SCPE_IERR;
dptr = find_dev_from_unit (uptr);
if (dptr == NULL)
    return SCPE_IERR;
dibp = (DIB *) dptr->ctxt;
if (dibp == NULL)
    return SCPE_IERR;

/* find unit index */ 
for (i=0; i < dibp->num ; i++) {
    if  (dptr->units + i  == uptr) 
        break;
}
if(i == dibp->num) {
    return SCPE_IERR;
}

fprintf (st, "I2c addr=%02x", I2C_device_map[dibp->dsp_tbl[i].dev]);

return SCPE_OK;
}





#endif

/* ---PiDP end---------------------------------------------------------------------------------------------- */

