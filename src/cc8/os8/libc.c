/*
 * This file is part of the CC8 OS/8 C compiler.
 *
 * The CC8 OS/8 C compiler is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The CC8 OS/8 C compiler is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the CC8 OS/8 C compiler as ../GPL3.txt.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

/*
 * PDP8/E LIBC routines for Small-C compiler.
 *
 * This is a complex collection of mixed C and SABR assembly routines.
 * Some functions have been substantially shortened to save space
 * relative to the original versions.  Eventually, most of the C will
 * need to be rewritten in SABR and hand optimised; e.g. atoi().
 */

#asm
ABSYM POP 147
ABSYM PSH 150
ABSYM JLC 151
ABSYM STKP 152
ABSYM PTSK 153
ABSYM POPR 154
ABSYM PCAL 155
ABSYM TMP 156
ABSYM GBL 157
ABSYM ZTMP 146
ABSYM ZPTR 145
ABSYM ZCTR 144
ABSYM FPTR 160
/
	DECIM
/
/
/
/
		DUMMY ARGST
		DUMMY ARGNM
ARGST,	BLOCK 2
ARGNM,	BLOCK 2
/
		ENTRY LIBC
LIBC,	BLOCK 2
		CLA CLL
		TAD I LIBC
		DCA ARGST
		INC LIBC#
		TAD I LIBC
		DCA ARGST#
		INC LIBC#
		TAD I ARGST
		DCA STKP
		IAC
		TAD LCALL	/ INIT ? LCALL==-1 
		SZA CLA
		JMP LB1
		TAD STKP
		DCA GBL		/ SET LOCAL GBL(LITPTR)
		ISZ GBL
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
		TAD STKP
		DCA I ARGST		/ UPDATE MASTER STKP
		DCA ZPTR		/ INIT PRINTF FLAG
		DCA FPTR		/ INIT FPRINTF FLAG
LB1,	MQA				/ CALL INDEX IN MQ
		SPA
		JMP LRET
		TAD CPNT
		DCA LCALL
		TAD I LCALL
		DCA LCALL
		JMSI PCAL
LCALL,	-1
		RETRN LIBC
LRET,	CLA MQL
		DCA LCALL		/ INIT OK
		RETRN LIBC
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
		TAD STKP
		DCA I ARGST		/ UPDATE MASTER STKP
		SWP
		CDF1
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
		TAD STKP
		DCA I ARGST		/ UPDATE MASTER STKP
		CDF1
		JMPI ZTMP
PVL,	PUSH
OVL,	PPOP
MVL,	PUTSTK
PVR,	POPRET
PVC,	PCALL
/
CPNT,	CLIST
		CPAGE 24
/
/		THIS IS THE DISPATCH LIST FOR THIS LIBRARY
/		MAKE SURE LIBC.H MATCHES
/
CLIST,	ITOA
		PUTS
		DISPXY
		GETC
		GETS
		ATOI
		SSCANF
		XINIT
		MEMCPY
		KBHIT
		PUTC
		STRCPY
		STRCAT
		STRSTR
		EXIT
		ISNUM
		ISALPHA
		SPRINTF
		MEMSET
		FGETC
		FOPEN
		FPUTC
		FCLOSE
		PRINTF
		ISALNUM
		ISSPACE
		FPRINTF
		FPUTS
		STRCMP
		CUPPER
		FGETS
		REVCPY
		TOUPPER
#endasm

#define stdout 0
#define NULL 0
#define isdigit isnum



fgetc()
{
#asm

	CLA CLL
	CALL 2,CHRIO
	ARG (-4
	ARG FRSL
	TAD FRSL
	TAD (-26		/^Z
	SNA CLA
	DCA FRSL
	TAD FRSL
	CDF1
	JMPI POPR
FRSL,BLOCK 2

//	CHRIO - CHARACTER I/O.
/
/	CALL CHRIO(IDEVN,ICHAR)
/
/	IDEV = FORT II DEVICE NUMBER.
/
/	ICHAR = 7 OR 8 BIT CHARACTER.
/
/	IF IDEV IS POSITIVE, THE CHAR IS OUTPUTTED.
/
/	IF IDEV IS NEGATIVE, THE NEXT CHAR IS
/	READ FROM THE DEVICE, AND PUT IN ICHAR.
/
//
		ENTRY CHRIO
CHRIO,	BLOCK 2
	JMS GETP
	SPA		/WHAT IS DEVICE SIGN?
	JMP RCHAR	/NEG DEV.   MEANS READ.
	JMS SETDEV	/POS DEV.  MEANS WRITE.
	0000
	JMS GETP
	DCA ICHAR
	JMS CHSUB
	JMP XIT

IDEV,	0
ICHAR,	0
ADDR,	0

RCHAR,	CIA		/READ A CHAR.
	JMS SETDEV
	1024		/SET BIT FOR READ. (8 UNITS NOW!)
	JMS GETP
	CLA
	TAD CDFB
	DCA CDFCH
	JMS CHSUB
CDFCH,	HLT
	AND (127	/ 7 BIT FOR NOW
	DCAI ADDR
XIT,	CLA
	RETRN CHRIO

SETDEV,	0
	TAD (-1
	AND (7
	CLL RAR;RTR;RTR
	TAD I SETDEV
	INC SETDEV
	DCA IDEV
	JMP I SETDEV

CHSUB,	0
	TAD ICHAR
	AND (255
	TAD IDEV
	CALL 0,GENIO
	JMP I CHSUB

GETP,	0
	TAD CHRIO
	DCA CDFA
CDFA,	HLT
	TADI CHRIO#
	DCA CDFB
	INC CHRIO#
	TADI CHRIO#
	DCA ADDR
	INC CHRIO#
CDFB,	HLT
	TADI ADDR
	JMP I GETP
#endasm
}

fputc(ch)
int ch;
{
	ch;
#asm
	DCA FRSL
	CALL 2,CHRIO
	ARG (4
	ARG FRSL
	CDF1
	TAD FRSL
#endasm
}
		sixbit(p)
char *p;
{
	*p++;
#asm
		AND (63
		BSW
		MQL
#endasm
	*p;
#asm
		AND (63
		MQO
#endasm
}

fputs(p)
int *p;
{
	while (*p++)
#asm
		DCA FRSL
		CALL 2,CHRIO
		ARG (4
		ARG FRSL
		CDF1
#endasm
}


fopen (fnm,flg)
char *fnm;
int flg;
{
	char *p;
	p=fnm;
	p=strstr(fnm,".");
	if (p==0)
		return(-1);
	if (*flg=='w') {
#asm
		CLA
		TAD FC1#
		DCA FBSE#
		JMP FC3
FC1,	CALL 0,OOPEN
FC2,	CALL 0,IOPEN
#endasm
	}
	if (*flg=='r') {
#asm
		CLA
		TAD FC2#
		DCA FBSE#
FC3,	CDF1
#endasm
	*p++=0;
	sixbit(p);
#asm
		PAGE
		/ OFFSET IOPEN+81 = FILEEX
		DCA ZTMP
		TAD FC2#		/ CODE
		AND (63
		TAD (128
		DCA FDCT
		CDF0
		TADI FDCT
		DCA FEX1
		TAD FDCT
		TAD (64
		DCA FDCT
		TADI FDCT
		TAD (81			/ OFFSET OF EXTENSION
		DCA FDCT
FEX1,	HLT
		TAD ZTMP
		DCAI FDCT
		CDF1
#endasm
	fnm;
#asm
		DCA ZTMP	/ PACK 6 8BIT CHARS INTO FILENAME
		TAD (-3
		DCA FDCT
		TAD FDCA
		DCA FP4
FP1,	CAM
		TADI ZTMP
		SNA
		JMP FP2
		AND (63
		BSW
		MQL
		ISZ ZTMP
FP2,	TADI ZTMP	/ WILL USE STACK FIELD
		AND (63
		SZA
		ISZ ZTMP
		MQO
FP4,	DCA FFNM
		ISZ FP4
		ISZ FDCT
		JMP FP1
		TAD (46
		DCAI ZTMP	/ PUT . BACK INTO FNM
		CLA CLL CMA
		TAD STKP
		DCA STKP
FBSE,	CALL 2,IOPEN
		ARG FDEV
		ARG FFNM
		JMPI POPR
FDCA,	DCA FFNM
FDCT,	0
FFNM,	TEXT /TM@@@@/
FDEV,	TEXT /DSK@@@/
#endasm
	}
}

fclose()
{
#asm
		CALL 0,OCLOS
#endasm
}


puts(p)
char *p;
	{
		while (*p++) 
#asm
		TLS
XC1,	TSF
		JMP XC1
#endasm
	}

dispxy(x,y)
int x,y;
{
	x;      /* put x param in AC */
#asm
	3115	/ 6053 octal (we're in DECIM mode!) so DILX, load display X reg
#endasm
	y;
#asm
	3116	/ 6054 octal, DILY, load display Y reg from AC = y param
	3117	/ 6055 octal, DIXY: pulse display at loaded X,Y coordinate
#endasm
}

getc()
{
#asm
	 CLA CLL
GT1, KSF
	 JMP GT1
	 KRB
	 TAD (-254
	 CLA
	 KRB
	 SNL			/ DO NOT ECHO BS
	 TLS
	 TAD (-131		/ ? ^C
	 SNA CLA
	 JMP OSRET
	 KRB
	 AND (127		/ 7 BITS!
#endasm
}

gets(p)
char *p;
{
int q,tm;
		tm=1;
		q=p;
		while (tm) {
		getc();
#asm
		AND (127
		TAD (-13	/ CR IS END OF STRING -> 0
		SZA
		TAD (13
	    DCAI STKP
#endasm
		if (tm-127)	/* Handle BS */
		  *p++=tm;
		else
			if (p-q) {
		   puts("\b \b");
		   p--;
			}
	}
	putc(10);	/* newline */
	return q;		
}


atoi(p,rsl)
char *p;
int *rsl;
{
#asm
	DCA ZTMP
	DCA ZCTR
	TAD (3584		/ NOP
	DCA XINV
	CDF1			/ Change DF back to 1 in case SABR changes it!
#endasm
	while (*p==' ')
	 p++;
	if (*p=='-') {
#asm
	CLA
	TAD (3617
	DCA XINV		/ CIA
	CDF1
#endasm
	p++;
	}
	while (*p++) {
#asm
	TAD (-48		/ '0' ... SEE CODE
	DCA JLC
	TAD JLC
	SPA CLA
	JMP XRET
	TAD (-10
	TAD JLC
	SMA CLA
	JMP XRET		/ EXIT IF NOT NUMBER
	TAD ZTMP
	CLL RTL			/ *4
	TAD ZTMP		/ *5
	CLL RAL			/ *10
	TAD JLC
	DCA ZTMP
	ISZ ZCTR		/ CHAR COUNTER
#endasm
	}
#asm
XRET,	TAD ZCTR
		MQL
		CMA
		TAD STKP	/ ->RSL
		DCA TMP
		TADI TMP
		DCA TMP
		TAD ZTMP
XINV,	NOP
		DCAI TMP	/ WRITE RSL
		MQA			/ RETURN LENGTH
#endasm
}


xinit()
{
	puts("PDP-8 C Compiler V1.0:\r\n");
}


memcpy(dst,src,cnt)
int dst,src,cnt;
{
#asm
	CLA
	TAD STKP
	TAD (-4
	DCA 12
	CMA
	TADI 12
	DCA 11
	CMA
	TADI 12
	DCA 10
	TADI 12
	CIA
	DCA ZTMP
CP1,	TADI 10
		DCAI 11
		ISZ ZTMP
		JMP CP1
#endasm

}

kbhit()
{
#asm
		CLA CMA
		KSF
		CLA
#endasm	
}

putc(p)
char p;
{
	p;
#asm
		TLS
MP1,	TSF
		JMP MP1
#endasm
}

strcmp( dm , sm )
char *dm,*sm;
{
	int rsl;

	rsl=0;
	while (*dm)
		rsl+=(*sm++-*dm++);
	return rsl;
}

strcpy( dm , sm )
char *dm,*sm;
{
	while (*dm++=*sm++);
}

strcat( dm , sm )
char *dm,*sm;
{
	int qm;
	qm=dm;
	while(*dm) dm++;
	strcpy(dm,sm);
	return qm;
}

strstr ( s , o )
char *s , *o ;
{
char *x , *y , *z ;
 for ( x = s ; * x ; x ++ ) {
  for ( y = x , z = o ; * z && * y == * z ; y ++ ) z ++ ;
  if ( z > o && ! * z ) return x ;
 } return 0 ;
}

exit(retval)
int retval;
{
#asm
OSRET,	CALL 0,EXIT
		HLT
#endasm
}

isalnum(vl)
int vl;
{
	return (isnum(vl) + isalpha(vl));
}

isnum(vl)
int vl;
{
		vl;
#asm
		TAD (-48
		SPA
		JMP XNO
		TAD (-10
		SMA CLA
XNO,	CLA SKP
		IAC
#endasm
}

isspace(vl)
int vl;
{
		vl;
#asm
		SNA
		JMP YNO
		TAD (-33
		SMA CLA
YNO,	CLA SKP
		IAC
#endasm
}


isalpha(vl)
int vl;
{
		vl;				/* Include '?' and '@' as alpha vars */
#asm
		TAD (-65
		SPA
		JMP ANO
		TAD (-26
		SPA
		JMP BNO
		TAD (-6
		SPA
		JMP ANO
		TAD (-26
BNO,	SMA CLA
ANO,	CLA SKP
		IAC
#endasm
}

cupper(p)				/* In place convert to uppercase */
int p;
{
	p;
#asm
		DCA ZTMP
CPP1,	CLA
		TADI ZTMP
		SNA
		JMP CPP2
		TAD (-97
		SPA
		JMP CPP3
		TAD (-26
		SMA
		JMP CPP3
		TAD (91
		DCAI ZTMP
CPP3,	ISZ ZTMP
		JMP CPP1
CPP2,
#endasm
}

toupper(p)
int p;
{
	p;
#asm
		DCA ZTMP
		TAD ZTMP
		TAD (-97
		SPA
		JMP TPP3
		TAD (-26
		SMA
		JMP TPP3
		TAD (91
		JMP TPP2
TPP3,	CLA CLL
		TAD ZTMP
TPP2,
#endasm
}


/* Arbitrary fgets(). Read until LF, CR/LF are retained*/
/* EOF returns null, else strlen(*p) */

fgets(p)
char *p;
{
char *q;
	q=p;
	while(*p=fgetc()) {
		if (*p++==10)
			break;
	}
	*p=0;
	return (p-q);
}

memset(dst, dt, sz)
char *dst;
int dt,sz;
{
	int i;
	for (i=0;i<sz;i++)
		*dst++=dt;
}

/*
** reverse string in place 
*/
reverse(s) char *s; {
  char *j;
  int c;
  j = s + strlen(s) - 1;
  while(s < j) {
    c = *s;
    *s++ = *j;
    *j-- = c;
    }
  }

/*
	This is somewhat involved in that the vararg system in SmallC is rather limited.
	For printf and sprintf, a char buffer is required supplied by the user or,
	as below, located at the end of the stack (7500 .. 64 locs). In addition,
	another page zero location (ZPTR) is required. This is always risky as the
	SABR/LOADER system uses a lot of locations here. See how this goes as it is possible
	to use arbitrary localions on the stack as well.
*/

fprintf(nxtarg) int nxtarg;
{
#asm
		ISZ FPTR
		JMP PRINTF
#endasm
}


printf(nxtarg) int nxtarg;
{
#asm
	TAD (3904	/ THIS IS THE PRINT BUFFER AT 7500 ON THE STACK
	DCA ZPTR
	JMP SPRINTF
#endasm
}

/*
** sprintf(obfr, ctlstring, arg, arg, ...)
** Called by printf().
*/
sprintf(nxtarg) int nxtarg; {
  int  arg, left, pad, cc, len, maxchr, width;
  char *ctl, *sptr, str[17],*obfr,zptr;

#asm
	TAD ZPTR
	DCAI STKP	/ POINTS TO ZPTR
#endasm
  cc = 0;
  nxtarg = &nxtarg-nxtarg;
  if (zptr)
    obfr=zptr;
  else
	obfr = *nxtarg++;
  ctl = *nxtarg++;                          
  while(*ctl) {
    if(*ctl!='%') {*obfr++=*ctl++; ++cc; continue;}
    else ++ctl;
    if(*ctl=='%') {*obfr++=*ctl++; ++cc; continue;}
    if(*ctl=='-') {left = 1; ++ctl;} else left = 0;       
    if(*ctl=='0') pad = '0'; else pad = ' ';
	width=0;
    if(isdigit(*ctl)) {
      ctl+=atoi(ctl, &width);
      }
	maxchr=0;
    if(*ctl=='.') {            
      ctl+=atoi(++ctl,&maxchr)+1;
      }
    arg = *nxtarg++;
    sptr = str;
    switch(*ctl++) {
      case 'c': str[0] = arg; str[1] = NULL; break;
      case 's': sptr = arg;        break;
      case 'd': itoa(arg,str);     break;
      case 'b': itoab(arg,str,2);  break;
      case 'o': itoab(arg,str,8);  break;
      case 'u': itoab(arg,str,10); break;
      case 'x': itoab(arg,str,16); break;
      default:  return (cc);
      }
    len = strlen(sptr);
    if(maxchr && maxchr<len) len = maxchr;
    if(width>len) width = width - len; else width = 0; 
    if(!left) while(width--) {*obfr++=pad; ++cc;}
    while(len--) {*obfr++=*sptr++; ++cc; }
    if(left) while(width--) {*obfr++=pad; ++cc;}  
    }
  *obfr=0;
  zptr;
#asm
		SNA				/ IF ZPTR, EITHER USE PUTS OR FPUTS
		JMP PF1
		JMSI PSH
		CLA
		TAD FPTR
		SNA CLA
		JMP PF2
		JMSI PCAL
		FPUTS
		JMP PF3
PF2,	JMSI PCAL
		PUTS
PF3,	JMSI POP
PF1,	CLA
		DCA ZPTR
		DCA FPTR
#endasm

  return(cc);
  }

/*
** itoa(n,s) - Convert n to characters in s 

itoa(n, s) char *s; int n; {
  int sign;
  char *ptr;
  ptr = s;
  if ((sign = n) < 0) n = -n;
  do {
    *ptr++ = n % 10 + '0';
    } while ((n = n / 10) > 0);
  if (sign < 0) *ptr++ = '-';
  *ptr = '\0';
  reverse(s);
  }
*/

itoa(n, s) char *s; int n; {
  char *ptr;
  ptr = s;
  if (n < 0) {
	  n = -n;
	  *ptr++='-';
  }
  itoab(n,ptr,10);
}


/*
** itoab(n,s,b) - Convert "unsigned" n to characters in s using base b.
**                NOTE: This is a non-standard function.
*/
itoab(n, s, b) int n; char *s; int b; {
  char *ptr;
  int lowbit;
  ptr = s;
  b >>= 1;
  do {
    lowbit = n & 1;
    n = (n >> 1) & 4095;
    *ptr = ((n % b) << 1) + lowbit;
    if(*ptr < 10) *ptr += '0'; else *ptr += 55;
    ++ptr;
    } while(n /= b);
  *ptr = 0;
  reverse (s);
  }


strlen(p)
char *p;
{
	int n;

	n=0;
	while (*p++)
		n++;
	return n;
}

#define EOF 0

sscanf(nxtarg) int nxtarg; {
  char *ctl;
  int u;
  int  *narg, wast, ac, width, ch, cnv, base, ovfl, sign, *ibfr;
  ac = 0;
  nxtarg = &nxtarg-nxtarg;
  ibfr = *nxtarg++;
  ctl = *nxtarg++;
  while(*ctl) {
    if(*ctl++ != '%') continue;
    narg = *nxtarg++;
    ctl += atoi(ctl, &width);
	if (!width)
		width=-1;
    if(!(cnv = *ctl++)) break;
    switch(cnv) {
      case 'c':
        *narg = *ibfr++;
        break;
      case 's':
        while(width--)
          if((*narg++ = *ibfr++) == 0) break;
        *narg = 0;
        break;
      default:
        switch(cnv) {
          case 'b': base =  2; break;
          case 'd': base = 10; break;
          case 'o': base =  8; break;
          case 'x': base = 16; break;
          default:  return (ac);
          }
        *narg = u = 0;
		sign = 1;
        while(width-- && (ch=*ibfr++)>32) {
          if(ch == '-') {sign = -1; continue;}
          if(ch < '0') break;
          if(ch >= 'a')      ch -= 87;
          else if(ch >= 'A') ch -= 55;
          else               ch -= '0';
          u = u * base + ch;
          }
        *narg = sign * u;
      }
    ++ac;                          
    }
  return (ac);
  }

revcpy(dst,src,cnt)
int *dst,*src,cnt;
{
	dst+=cnt;
	src+=cnt;
	while (cnt--)
		*dst--=*src--;
}


