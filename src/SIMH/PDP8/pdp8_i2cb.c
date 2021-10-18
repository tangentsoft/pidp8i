/* pdp8_i2cb.c: PDP-8 I2C bridge pseudo device

   Copyright (c) 2021, Heinz-Bernd Eggenstein

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
   ROBERT M SUPNIK BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

   Except as contained in this notice, the name of Robert M Supnik shall not be
   used in advertising or otherwise to promote the sale, use or other dealings
   in this Software without prior written authorization from Robert M Supnik.

   i2cb        I2C pseudo device

   17-Oct-2021	HBE	initial version

  Unlike other PDP-8 SIMH devices, this device does not try to emulate a
  historical real world device. It rather allows IOT instructions to be forwarded
  over I2C to (typically) a microcontroller that would then implement the desired
  logic (Arduino, Raspberry Pi Pico, FPGA,...).

  The user is able to configure several device uinits, where each unit maps a
  PDP-8 device address to an address on the I2C bus. Of course the same device
  on the I2C bus can handle multiple PDP-8 devices as the complte IOT instruction
  is tranferred via I2C.

  The device is diabled by default.

  So let's say we want to implement a PDP-8 device 030 (that's in the range
  reserved for user applications so it should not clash with common DEC product
  device numbers) with a microcontroller on the I2C bus with address (say) 0x11.
  The startup script would then read

  set I2CB enabled
  set I2CB0 enabled
  set I2CB0 devno=30 ; devno parsed as octal
  set I2CB0 i2caddr=11 ; I2C address parsed as hex

  Then the IOT instruction
  6301
  would transmit a data packet consisting of the 12 bit instruction code plus
  12 bit AC (so 3 bytes) to the I2C device 0x11 which would then need to compute
  and send back a potentially changed AC value and a flag indicating whether to
  skip the next instruction or not, in addition to whatever function should be
  triggered by the microcontroller.


  Complications & TODO:
  - Handling errors on the I2C bus
  - Interrupts are not supported for now.
  - Booting and resetting devices must also be possible, this could be done by
    transmitting special messages that do not start with a "6" for the IOT command

*/

/* TODO have a configure option here */
/* TODO ignore the entire module if not requested but do not leave it completely
   TODO empty (some compilers might complain)*/
#include "pdp8_defs.h"

#ifdef I2C_BR_DEVICE


/* TODO decide which I2C abstraction to use */
/* TODO for prototyping we use this one here:
   TODO https://github.com/sstallion/libi2cd
   TODO which was intended for a PiDP-11 GPIO
   TODO extender. There is hope it might be included in the future in the PDP-8 
   TODO sim as well.
*/

#include <i2cd.h>

#define I2C_BR_DEV_PATH "/dev/i2c-1"


extern int32 stop_inst;


static uint8 I2C_device_map[DEV_MAX] = { };

static struct i2cd *i2cdev;
static t_stat rebuild_i2c_addr_map (DEVICE * dptr);



int32 i2c_bridge (int32 IR, int32 AC);
t_stat i2cb_show_dev (FILE * st, UNIT * uptr, int32 val, CONST void *desc);
t_stat i2cb_set_dev (UNIT * uptr, int32 val, CONST char *cptr, void *desc);

t_stat i2cb_show_addr (FILE * st, UNIT * uptr, int32 val, CONST void *desc);
t_stat i2cb_set_addr (UNIT * uptr, int32 val, CONST char *cptr, void *desc);


#define I2CB_MAX 8              /* max number of bridges */

#define I2C_RETRIES 5           /* I2C retries */
#define I2C_TIMEOUT_10MS 2      /* I2C timeout in units of 10ms */


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
    { DEV_I2CB0, &i2c_bridge },
    { DEV_I2CB1, &i2c_bridge },
    { DEV_I2CB2, &i2c_bridge },
    { DEV_I2CB3, &i2c_bridge },
    { DEV_I2CB4, &i2c_bridge },
    { DEV_I2CB5, &i2c_bridge },
    { DEV_I2CB6, &i2c_bridge },
    { DEV_I2CB7, &i2c_bridge }
};





REG i2cb_reg[] = {
    { NULL }                    /* TODO ?? any registers that we need? a state is held by I2C device, probably not */
};



MTAB i2cb_mod[] = {
    { MTAB_XTD | MTAB_VUN | MTAB_VALR, 0, "I2C address", "I2CADDR",
        &i2cb_set_addr, &i2cb_show_addr, NULL },
    { MTAB_XTD | MTAB_VUN | MTAB_VALR, 0, "DEVNO", "DEVNO",
        &i2cb_set_dev, &i2cb_show_dev, NULL },
    { 0 }
};





const char *i2cb_description (DEVICE * dptr);


t_stat i2cb_reset (DEVICE * dp);        /* reset routine */
t_stat i2cb_boot (int32 u, DEVICE * dp);        /* boot routine  */


DIB i2cb_dib =
    { DEV_I2CB0, I2CB_MAX, { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }, i2cb_dsp };


/* TODO for now, all "action" is taking place in the IOT dispatch
   TODO routines, so no unit service code is needed, and also no
   TODO registers. Will change with interrupt support?? 
   TODO Also for now we don't manage saving device state by SIMH.
*/



UNIT i2cb_unit[] = {
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) },
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) },
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) },
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) },
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) },
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) },
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) },
    { UDATA (NULL, UNIT_DISABLE | UNIT_DIS, 0) }
};



DEVICE i2cb_dev = {
    "I2CB", i2cb_unit, i2cb_reg, i2cb_mod,
    8, 10, 31, 1, 8, 12,
    NULL, NULL, &i2cb_reset,
    &i2cb_boot, NULL, NULL,
    &i2cb_dib, DEV_DISABLE | DEV_DIS | DEV_NOSAVE, 0,
    NULL, NULL, NULL, NULL, NULL, NULL,
    &i2cb_description
};



int32 i2c_forward_IOT (uint8 i2c_addr, int32 IR, int32 AC, int32 * AC_new_out,
    int32 * skip_flag_out);
int32 check_i2c_open ();
void makeCmd (uint8 * buf, int32 iot, int32 ac);

int32 check_i2c_open ()
{

    if (i2cdev == NULL) {
/* Open I2C character device */
        i2cdev = i2cd_open (I2C_BR_DEV_PATH);
        if (i2cdev == NULL) {
            /* TODO logging? but maybe only once */
            return 0;
        }
    }

    i2cd_set_retries (i2cdev, I2C_RETRIES);
    i2cd_set_timeout (i2cdev, I2C_TIMEOUT_10MS);

/* TODO perhaps we want to have this user configurable even ? */

    return 1;
}


void makeCmd (unsigned char *buf, int32 iot, int32 ac)
{

    int32 instr, device, pulse, data;

    buf[0] = (iot >> 4) & 0xff;
    buf[1] = (iot & 31) << 4 | ((ac >> 8) & 0xf);
    buf[2] = (ac & 0xff);

    instr = buf[0] >> 5;
    device = (buf[0] & 31) << 1 | (buf[1] >> 7);
    pulse = (buf[1] >> 4) & 7;
    data = buf[2] | (buf[1] & 15) << 8;

/* TODO DEBUG logging ?? */

}


int32 i2c_forward_IOT (uint8 i2c_addr, int32 IR, int32 AC, int32 * AC_new_out,
    int32 * skip_flag_out)
{

    unsigned char msg_tx[3];
    unsigned char msg_rx[2];

    uint32 devno;
    uint32 i2caddr;

/* check if i2c device already opened */
    check_i2c_open ();
    makeCmd (msg_tx, IR, AC);
    devno = (IR >> 3) & 077;
    i2caddr = I2C_device_map[devno];
    if (i2cd_write_read (i2cdev, i2caddr, msg_tx, 3, msg_rx, 2) < 0) {
        /* TODO handle I2C problems that are not handled     */
        /* TODO by libi2cd timeouts and retries              */
        /* TODO for initial testing we signal an IO error    */
        /* TODO best way would be to let the user decide via */
        /* TODO modifier what to do here                     */

        return SCPE_IOERR << IOT_V_REASON;
    }

    *AC_new_out = (uint32) msg_rx[0] | (((uint32) msg_rx[1] & 0x0f) << 8);
    *skip_flag_out = ((uint32) msg_rx[1] & 0x10) >> 4;

    return SCPE_OK;
}

/* IOT routine */

int32 i2c_bridge (int32 IR, int32 AC)
{

    int32 device;
    uint8 i2c_addr;
    int32 res;
    int32 AC_new = AC;
    int32 skip_flag = 0;

    device = (IR >> 3) & 077;


/* lookup I2C device that is configured to handle this device */
    i2c_addr = I2C_device_map[device];

    res = i2c_forward_IOT (i2c_addr, IR, AC, &AC_new, &skip_flag);

    if (res == SCPE_OK) {
        return (skip_flag ? IOT_SKP + AC_new : AC_new);
    }


/* fall through TODO ill instr is probably not we want here but for prototyping it's ok */

    return (stop_inst << IOT_V_REASON) + AC;    /* ill inst */

}

const char *i2cb_description (DEVICE * dptr)
{
    return "I2C bridge prototyping device";
}

t_stat i2cb_reset (DEVICE * dp)
{
/* TODO implement reset function */
    return SCPE_OK;
}

t_stat i2cb_boot (int32 u, DEVICE * dp)
{
/* TODO implement boot function  */
    return SCPE_OK;
}




/* set device number for the specific unit (!) not the device */

t_stat i2cb_set_dev (UNIT * uptr, int32 val, CONST char *cptr, void *desc)
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
    for (i = 0; i < dibp->num; i++) {
        if (dptr->units + i == uptr)
            break;
    }
    if (i == dibp->num) {
        return SCPE_IERR;
    }

    newdev = get_uint (cptr, 8, DEV_MAX - 1, &r);       /* get new */
    if ((r != SCPE_OK) || (newdev == dibp->dsp_tbl[i].dev))
        return r;
    dibp->dsp_tbl[i].dev = newdev;      /* store */
    rebuild_i2c_addr_map (dptr);
    return SCPE_OK;
}



t_stat i2cb_show_dev (FILE * st, UNIT * uptr, int32 val, CONST void *desc)
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
    for (i = 0; i < dibp->num; i++) {
        if (dptr->units + i == uptr)
            break;
    }
    if (i == dibp->num) {
        return SCPE_IERR;
    }

    fprintf (st, "devno=%02o", dibp->dsp_tbl[i].dev);

    return SCPE_OK;
}



/* Set I2C addr */

t_stat i2cb_set_addr (UNIT * uptr, int32 val, CONST char *cptr, void *desc)
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


    if ((cptr == NULL) || (*cptr == 0))
        return SCPE_ARG;
    if (uptr->flags & UNIT_ATT)
        return SCPE_ALATT;

    addr = get_uint (cptr, 16, 63, &r);
    if (r != SCPE_OK)
        return SCPE_ARG;
    uptr->u3 = (int32) addr;
    rebuild_i2c_addr_map (dptr);
    return SCPE_OK;
}

/* Show I2C address */

t_stat i2cb_show_addr (FILE * st, UNIT * uptr, int32 val, CONST void *desc)
{

    if (uptr == NULL)
        return SCPE_IERR;

    fprintf (st, " I2c addr=%02x", uptr->u3);

    return SCPE_OK;
}

t_stat rebuild_i2c_addr_map (DEVICE * dptr)
{
    UNIT *uptr;
    DIB *dibp;

    int32 i;
    int32 i2caddr;
    uint32 devno;

    for (i = 0; i < DEV_MAX; i++) {
        I2C_device_map[i] = 0;
    }

/* We don't need to be concerned with enable/disable state */
/* of the unit here, as disabled units will not have their dispatch functions called */
/* see build_dev_tab function in CPU module */

    dibp = (DIB *) dptr->ctxt;
    if (dibp == NULL || dibp->dsp_tbl == NULL)
        return SCPE_IERR;

    for (i = 0; i < dptr->numunits; i++) {
        i2caddr = dptr->units[i].u3;
        devno = dibp->dsp_tbl[i].dev;

        if (devno >= DEV_MAX)
            return SCPE_IERR;

        I2C_device_map[devno] = i2caddr;
    }
    return SCPE_OK;
}



#endif
