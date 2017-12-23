/*
 * This file is part of the CC8 cross-compiler.
 *
 * The CC8 cross-compiler is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The CC8 cross-compiler is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the CC8 cross-compiler as ../GPL3.txt.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

#asm
/
/	PDP8/E Run time routines for the Small-C compiler.  SABR syntax.
/
ABSYM POP 160
ABSYM PSH 161
ABSYM JLC 162
ABSYM STKP 163
ABSYM PTSK 164
ABSYM POPR 165
ABSYM PCAL 166
ABSYM TMP 167
ABSYM GBL 170
ABSYM ZTMP 171
/
	DECIM
/
STK,	COMMN 3840
/
/
/
		ENTRY MAIN
MAIN,	BLOCK 2
		TAD GBLS
		DCA STKP
		TAD GBLS
		DCA GBL
		ISZ GBL		/ LOCAL LITERALS = STKP+1
		TAD PVL
		DCA PSH
		TAD OVL
		DCA POP
		TAD MVL
		DCA PTSK
		TAD PVR
		DCA POPR
		TAD PVC
		DCA PCAL
		RIF
		TAD (3201
		DCA PCL1
		TAD PCL1
		DCA DCC0
		JMS MCC0
		CLA CMA
		MQL
		CALL 1,LIBC
		ARG STKP
		CALL 0,OPEN
		JMSI PCAL
		XMAIN
		CALL 0,EXIT
/
PUSH,	0
		CDF1
		ISZ STKP
		DCAI STKP
		TADI STKP
		JMPI PUSH
PPOP,	0
		CDF1
		DCA TMP
		TADI STKP
		MQL
		CMA
		TAD STKP
		DCA STKP
		TAD TMP
		JMPI PPOP
PUTSTK,	0
		JMSI POP
		SWP
		DCA JLC
		SWP
		DCAI JLC
		TADI JLC
		JMPI PUTSTK
POPRET,	JMSI POP
		SWP
		DCA ZTMP
		SWP
		JMPI ZTMP
PCALL,	0
		CLA CLL
PCL1,	0
		TADI PCALL
		DCA ZTMP
		TAD PCALL
		IAC
		JMSI PSH		/ PUSH RETURN
		CLA
		JMPI ZTMP
PVL,	PUSH
OVL,	PPOP
MVL,	PUTSTK
SVL,	STK
PVR,	POPRET
PVC,	PCALL
/
#endasm
