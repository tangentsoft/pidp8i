/* pidp8i.h: Interface between PiDP-8/I additions and the stock SIMH PDP-8
   simulator

   Copyright © 2015-2017 by Oscar Vermeulen and Warren Young

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

#if !defined(PIDP8I_H)
#define PIDP8I_H

#include "pdp8_defs.h"

typedef enum {
    pft_normal,
    pft_halt,
    pft_stop,
} pidp8i_flow_t;

extern int awfulHackFlag;

extern int32 get_switch_register (void);

extern pidp8i_flow_t handle_flow_control_switches (uint16* pM,
        uint32 *pPC, uint32 *pMA, int32 *pMB, int32 *pLAC, int32 *pIF,
        int32 *pDF, int32* pint_req);

extern void set_pidp8i_leds (uint32 sPC, uint32 sMA, uint16 sMB,
        uint16 sIR, int32 sLAC, int32 sMQ, int32 sIF, int32 sDF,
        int32 sSC, int32 int_req, int Pause);
extern void set_stop_flag (int f);
extern int pdp8_cpu_running (void);

#endif // !defined(PIDP8I_H)
