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

#define unix

#include <stdio.h>
#include "defs.h"
#include "data.h"
#include <string.h>

/*	Define ASNM and LDNM to the names of the assembler and linker
	respectively */

/*
 *	Some predefinitions:
 *
 *	INTSIZE is the size of an integer in the target machine
 *	BYTEOFF is the offset of an byte within an integer on the
 *		target machine. (ie: 8080,pdp11 = 0, 6809 = 1,
 *		360 = 3)
 *	This compiler assumes that an integer is the SAME length as
 *	a pointer - in fact, the compiler uses INTSIZE for both.
 */
#define	INTSIZE	1
#define	BYTEOFF	0

/*
 *	print all assembler info before any code is generated
 *
 */
header ()
{
	outstr ("/	Small C PDP8 Coder (1.0:27/1/99)");
	nl();
	FEvers();
	nl ();
	ol ("OPDEF ANDI 0400");
	ol ("OPDEF TADI 1400");
	ol ("OPDEF ISZI 2400");
	ol ("OPDEF DCAI 3400");
	ol ("OPDEF JMSI 4400");
	ol ("OPDEF JMPI 5400");
	ol ("OPDEF MQL 7421");
	ol ("OPDEF MQA 7701");
	ol ("OPDEF MQO 7501");
	ol ("OPDEF SWP 7521");
	ol ("OPDEF CDF1 6211");
	ol ("OPDEF CDF0 6201");
	ol ("OPDEF RIF 6224");
	ol ("OPDEF CAF0 6203");
	ol ("OPDEF BSW 7002");
	ol ("OPDEF CAM 7621");
	ol ("/");
    return 0;
}

nl ()
{
	outbyte (EOL);
/*	outbyte (10); */
    return 0;
}
initmac()
{
	defmac("cpm\t1");
	defmac("I8080\t1");
	defmac("RMAC\t1");
	defmac("smallc\t1");
    return 0;
}

galign(t)
int	t;
{
	return(t);
}

/*
 *	return size of an integer
 */
intsize() {
	return(INTSIZE);
}

/*
 *	return offset of ls byte within word
 *	(ie: 8080 & pdp11 is 0, 6809 is 1, 360 is 3)
 */
byteoff() {
	return(BYTEOFF);
}

/*
 *	Output internal generated label prefix
 */
olprfix() {
	ot("CC");
    return 0;
}

/*
 *	Output a label definition terminator
 */
col ()
{
	outbyte (',');
    return 0;
}

/*
 *	begin a comment line for the assembler
 *
 */
comment ()
{
	outbyte ('/');
    return 0;
}

/*
 *	Emit user label prefix
 */
prefix ()
{
    return 0;
}

/* Stkbase output stack base->literals =stkp+2 ... ie 202(8) =130(10) + sizeof(globals) */
stkbase()
{
	ot("GBL");
    return 0;
}

/*
 *	print any assembler stuff needed after all code
 *
 */
trailer ()
{
//	ot("\tENTRY ");
//	outbyte('M');
//	printlabel (litlab);
//	nl();
	outbyte('M');
	printlabel (litlab);
	col();
	ot("\t0");
	nl();
	ol("\tCDF1");
	ot("\tTAD L");
	printlabel (litlab);
	nl();
	ol ("\tSNA CLA    / Any literals to push?");
	ot ("\tJMP I M");
	printlabel (litlab);
	nl();
	ot("\tTAD X");
	printlabel (litlab);
	nl();
	ol ("\tDCA JLC");
	outbyte('D');
	printlabel (litlab);
	col();
	ol("CDF0");
	ot("\tTADI JLC");
	nl();
	ol ("\tJMSI PSH");
	ol ("\tCLA");
	ol ("\tISZ JLC");
	ot("\tISZ L");
	printlabel (litlab);
	nl();
	ot("\tJMP D");
	printlabel (litlab);
	nl();
	ot ("\tJMP I M");
	printlabel (litlab);
	nl();
	ol("CCEND,\t0");
	ol ("END");
    return 0;
}


/*
 *	function prologue
 */
prologue (sym)
char *sym;
{
    return 0;
}

/*
 *	text (code) segment
 */
gtext ()
{
/*	ol ("cseg"); */
    return 0;
}

/*
 *	data segment
 */
gdata ()
{
/*	ol ("dseg"); */
    return 0;
}

/*
 *  Output the variable symbol at scptr as an extrn or a public
 */
ppubext(scptr) char *scptr; {
	if (scptr[STORAGE] == STATIC) return 0;
//	ot (scptr[STORAGE] == EXTERN ? "extrn\t" : "public\t");
//	prefix ();
//	outstr (scptr);
//	nl();
    return 0;
}

/*
 * Output the function symbol at scptr as an extrn or a public
 */
fpubext(scptr) char *scptr; {
/*	if (scptr[STORAGE] == STATIC) return;
//	ot (scptr[OFFSET] == FUNCTION ? "public\t" : "extrn\t");
//	prefix ();
//	outstr (scptr);
//	nl (); */
    return 0;
}

/*
 *  Output a decimal number to the assembler file
 */
onum(num) int num; {
	outdec(num);	/* pdp11 needs a "." here */
    return 0;
}


/*
 *	fetch a static memory cell into the primary register
getmem (sym)
char	*sym;
{
	int adr;
		ol ("\tCLA");
		immd3 ();
		adr=glint(sym)+128;
		onum(glint(sym)+128);
		nl();
		ol("\tDCA JLC");
		ol("\tTADI JLC");
}*/

getmem (sym)
char	*sym;
{
	int adr;
	ol ("\tCLA");
	immd4 ();
	adr=glint(sym)+128;
	onum(glint(sym)+128);
    nl();
    return 0;
}
/*
 *	fetch a static memory cell into the primary register (pre-increment*/

getincmem (sym)
char	*sym;
{
	int adr;
	ol ("\tCLA");
	adr=glint(sym)+128;
	ot ("\tISZI (");
	onum(adr);
	nl();
	immd4 ();
	onum(adr);
	nl();
    return 0;
}


/*
 *	fetch the address of the specified symbol into the primary register
 *
 */
getloc (sym)
char	*sym;
{
	ol("\tCLA");
	ol("\tTAD STKP");
	if (sym[STORAGE] == LSTATIC) {
		immd3 ();
		printlabel(-1-glint(sym));
		nl();
	} else {
		if (stkp-glint(sym)==0) outstr("/");
		immd3 ();
		outdec (stkp-glint(sym));
		nl ();
	}
    return 0;
}

/*
 *	store the primary register into the specified static memory cell
 *

putmem (sym)
char	*sym;
{
		ol("\tMQL");
		immd3 ();
		onum(glint(sym)+128);
		nl();
		ol("\tDCA JLC");
		ol("\tMQA");
		ol("\tDCAI JLC");
		ol("\tTADI JLC");
}
*/

putmem (sym)
char	*sym;
{
	ot("\tDCAI (");
	onum(glint(sym)+128);
	nl();
	immd4 ();
	onum(glint(sym)+128);
	nl();
    return 0;
}

/*
 *	store the specified object type in the primary register
 *	at the address on the top of the stack
 *
 */
putstk (typeobj)
char	typeobj;
{
	ol("\tJMSI PTSK");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	fetch the specified object type indirect through the primary
 *	register into the primary register
 *
 */
indirect (typeobj)
char	typeobj;
{
	ol("\tDCA JLC");
/*	ol("\tCDF1"); */
	ol("\tTADI JLC");
    return 0;
}

/*
 *	fetch the specified object type indirect through the primary
 *	register into the primary register (pre-increment)
 *
 */
incdirect (typeobj)
char	typeobj;
{
	ol("\tDCA JLC");
	ol("\tISZI JLC");
	ol("\tTADI JLC");
    return 0;
}


/*
 *	swap the primary and secondary registers
 *
 */
swap ()
{
	ol ("\tSWP");
    return 0;
}
/*
*	Clear primary reg
*/
cpri()
{
	ol("\tCLA");
    return 0;
}
/*
 *	print partial instruction to get an immediate value into
 *	the primary register
 *
 */
immed ()
{
	ol ("\tCLA");
	ot ("\tTAD (");
    return 0;
}
immd2 ()
{
	ol ("\tCLA");
	ot ("\tTAD ");
    return 0;
}
immd3 ()
{
	ot ("\tTAD (");
    return 0;
}

immd4 ()
{
	ot("\tTADI (");
    return 0;
}
/*
 *	push the primary register onto the stack
 *
 */
gpush ()
{
	ol ("\tJMSI PSH");
	stkp = stkp - INTSIZE;
    return 0;
}

/*
 *	pop the top of the stack into the secondary register
 *
 */
gpop ()
{
	ol ("\tJMSI POP");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	swap the primary register and the top of the stack
 *
 */
swapstk ()
{
	ol ("\tMQL");
	gpop();
	ol ("\tSWP");
	gpush();
	ol ("\tSWP");
    return 0;
}

/*
 *	call the specified subroutine name
 *	varag is allowed for libc functions using a v prefix. In this case, the arg count+1 is pushed onto the stack as well.
 *  For the actual routine, the declaration should be a single arg eg printf(int args) in this case, the value of args is the count and &args-args point to the first arg in the caller's list.
 */
gcall (sname,nargs)
char	*sname;
int		*nargs;
{
	char tm[10];

	if (strstr(sname,"vlibc")) {
	immed();
	sname++;
	outdec(*nargs);
	outstr("\t/ PUSH ARG COUNT");
	nl();
	ol("\tJMSI PSH");
	stkp = stkp - INTSIZE;
	(*nargs)++;
	}
	if (strstr(sname,"libc"))
	{
		strcpy(tm,sname);
		immed();
		outstr(tm+4);
		nl();
		ol("\tMQL");
		ol("\tCALL 1,LIBC");
		ol("\tARG STKP");
		ol("\tCDF1");		/* Make sure DF is correct */
		return 0;
	}
	ol("\tCPAGE 2");
	ol("\tJMSI PCAL");
	ot ("\t");
	outstr (sname);
	nl ();
    return 0;
}

stri()
{
	ol("\tDCAI 10");
    return 0;
}
iinit()
{
	ol("\tCIA;CMA");
	ol("\tDCA 10");
    return 0;
}

/*
 *	return from subroutine
 *
 */
gret (sym)
char *sym;
{
	ol ("\tJMPI POPR");
    return 0;
}

/*
 *	perform subroutine call to value on top of stack
 *
 */
callstk ()
{
	immed ();
	outstr ("$+5");
	nl ();
	swapstk ();
	ol ("pchl");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	jump to specified internal label number
 *
 */
jump (label)
int	label;
{
	ot ("\tJMP\t");
	printlabel (label);
	nl ();
    return 0;
}

/*
 *	test the primary register and jump if false to label
 *
 */
testjump (label, ft)
int	label,
	ft;
{
	if (ft)
		ol ("\tSZA");
	else
		ol ("\tSNA");
	jump (label);
    return 0;
}

casejump()
{
	ol("\tTAD TMP");
	ol("\tSNA CLA");
    return 0;
}
/*
 *	print pseudo-op  to define a byte
 *
 */
defbyte ()
{
	ot ("\t");
    return 0;
}

/*
 *	print pseudo-op to define storage
 *
 */
defstorage ()
{
	ot ("COMMN\t");
    return 0;
}

/*
 *	print pseudo-op to define a word
 *
 */
defword ()
{
	ot ("\t");
    return 0;
}

/*
 *	modify the stack pointer to the new value indicated
 *
 */
modstk (newstkp)
int	newstkp;
{
	int	k;

	k = galign(stkp-newstkp);
	if (k == 0)
		return (newstkp);
	if (k>0 && k<5) {
		while (k--) ol ("\tISZ STKP");
		return (newstkp);
	}
	ol ("\tMQL");
	immd3 ();
	outdec (k);
	nl ();
	ol ("\tTAD STKP");
	ol ("\tDCA STKP");
	swap ();
	return (newstkp);
}

/*
 *	multiply the primary register by INTSIZE
 */
gaslint ()
{
    return 0;
}

/*
 *	divide the primary register by INTSIZE
 */
gasrint()
{
    return 0;
}

/*
 *	Case jump instruction
 */
gjcase() {
	ol ("\tCIA");
	ol ("\tDCA TMP");
    return 0;
}

/*
 *	add the primary and secondary registers
 *	if lval2 is int pointer and lval is not, scale lval
 */
gadd (lval,lval2) int *lval,*lval2;
{
/*	if (lval==0) ol("\tCIA");*/
	ol("\tDCA JLC");
	ol("\tJMSI POP");
	ol("\tMQA");
	ol("\tTAD JLC");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	subtract the primary register from the secondary
 *
 */
gsub ()
{
	ol("\tCIA");
	ol("\tDCA JLC");
	ol("\tJMSI POP");
	ol("\tMQA");
	ol("\tTAD JLC");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	multiply the primary and secondary registers
 *	(result in primary)
 *
 */
gmult ()
{
	ol("\tDCA JLC");
	ol("\tJMSI POP");
	ol("\tMQA");
	ol("\tCALL 1,MPY");
	ol("\tARG JLC");
	ol("\tCDF1");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	divide the secondary register by the primary
 *	(quotient in primary, remainder in secondary)
 *
 */
gdiv ()
{
	ol("\tDCA JLC");
	ol("\tJMSI POP");
	ol("\tMQA");
	ol("\tCALL 1,DIV");
	ol("\tARG JLC");
	ol("\tCDF1");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	compute the remainder (mod) of the secondary register
 *	divided by the primary register
 *	(remainder in primary, quotient in secondary)
 *
 */
gmod ()
{
	ol("\tDCA JLC");
	ol("\tJMSI POP");
	ol("\tMQA");
	ol("\tCALL 1,DIV");
	ol("\tARG JLC");
	ol("\tCALL 1,IREM");
	ol("\tARG 0");
	ol("\tCDF1");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	inclusive 'or' the primary and secondary registers
 *
 */
gor ()
{
	ol("\tJMSI POP");
	ol("\tMQA");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	exclusive 'or' the primary and secondary registers
 *
 */
gxor ()
{
	gpop();
	gcall ("?xor");
    return 0;
}

/*
 *	'and' the primary and secondary registers
 *
 */
gand ()
{
	ol("\tDCA JLC");
	ol("\tJMSI POP");
	ol("\tMQA");
	ol("\tAND JLC");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	arithmetic shift right the secondary register the number of
 *	times in the primary register
 *	(results in primary register)
 *
 */
gasr ()
{
	int lbl;

	lbl=getlabel();
	ol("\tCIA");
	ol("\tJMSI POP");
	gnlabel(lbl);
	ol("\tSWP");
	ol("\tCLL RAR");
	ol("\tSWP");
	ol("\tIAC");
	ol("\tSZA");
	jump(lbl);
	ol("\tSWP");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	arithmetic shift left the secondary register the number of
 *	times in the primary register
 *	(results in primary register)
 *
 */
gasl ()
{
	int lbl;

	lbl=getlabel();
	ol("\tCIA");
	ol("\tJMSI POP");
	gnlabel(lbl);
	ol("\tSWP");
	ol("\tCLL RAL");
	ol("\tSWP");
	ol("\tIAC");
	ol("\tSZA");
	jump(lbl);
	ol("\tSWP");
	stkp = stkp + INTSIZE;
    return 0;
}

/*
 *	two's complement of primary register
 *
 */
gneg ()
{
	ol("\tCIA");
    return 0;
}

/*
 *	logical complement of primary register
 *
 */
glneg ()
{
	ol("\tSNA CLA");
	ol("\tCMA");
    return 0;
}

/*
 *	one's complement of primary register
 *
 */
gcom ()
{
	ol("\tCMA");
    return 0;
}

/*
 *	Convert primary value into logical value (0 if 0, 1 otherwise)
 *
 */
gbool ()
{
	ol("\tSZA CLA");
	ol("\tIAC");
    return 0;
}

/*
 *	increment the primary register by 1 if char, INTSIZE if
 *      int
 */
ginc (lval) int lval[];
{
	ol ("\tIAC");
/*	if (lval[2] == CINT)
//		ol ("inx\th"); */
    return 0;
}
/*
 * Shortened INC
*/

gisz (lval)
int *lval;
{
	int adr;
	char *sym=lval[0];

	if (lval[1]) {
		ol ("\tISZI JLC");
		return 0;
	}

	ot ("\tISZI (");
	adr=stkp-glint(sym);
//	if (lval[STORAGE] == PUBLIC)
		adr=glint(sym)+128;
	onum(adr);
	nl();
    return 0;
}
/*
 *	decrement the primary register by one if char, INTSIZE if
 *	int
 */
gdec (lval) int lval[];
{
	ol ("\tTAD (-1");
/*	if (lval[2] == CINT)
//		ol("dcx\th"); */
    return 0;
}

/*
 *	following are the conditional operators.
 *	they compare the secondary register against the primary register
 *	and put a literl 1 in the primary if the condition is true,
 *	otherwise they clear the primary register
 *
 */

/*
 *	equal
 *
 */
geq ()
{
	ol("\tCIA");
	ol("\tTADI STKP");
	gpop();
	ol("\tSNA CLA");
	ol("\tCMA");
    return 0;
}

/*
 *	not equal
 *
 */
gne ()
{
	gpop();
	ol("\tCIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
    return 0;
}

/*
 *	less than (signed)
 *
 */
glt ()
{
	gpop();
	ol("\tCIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tAND (2048");
    return 0;
}

/*
 *	less than or equal (signed)
 *
 */
gle ()
{
	gpop();
	ol("\tCIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tSNA");
	ol("\tCLA CMA");
	ol("\tAND (2048");
    return 0;
}

/*
 *	greater than (signed)
 *
 */
ggt ()
{
	gpop();
	ol("\tSWP");
	ol("\tCIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tAND (2048");
    return 0;
}

/*
 *	greater than or equal (signed)
 *
 */
gge ()
{
	gpop();
	ol("\tSWP");
	ol("\tCIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tSNA");
	ol("\tCLA CMA");
	ol("\tAND (2048");
    return 0;
}

/*
 *	less than (unsigned)
 *
 */
gult ()
{
	gpop();
	ol("\tCLL CIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tSNL CLA");
	ol("\tIAC");
    return 0;
}

/*
 *	less than or equal (unsigned)
 *
 */
gule ()
{
	gpop();
	ol("\tCLL CIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tSNL CLA");
	ol("\tIAC");
    return 0;
}

/*
 *	greater than (unsigned)
 *
 */
gugt ()
{
	gpop();
	ol("\tCLL CIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tSNA SZL CLA");
	ol("\tIAC");
    return 0;
}

/*
 *	greater than or equal (unsigned)
 *
 */
guge ()
{
	gpop();
	ol("\tSWP");
	ol("\tCLL CIA");
	ol("\tDCA JLC");
	ol("\tMQA");
	ol("\tTAD JLC");
	ol("\tSNL CLA");
	ol("\tIAC");
    return 0;
}

/*	Squirrel away argument count in a register that modstk
	doesn't touch.
*/

gnargs(d)
int	d; {
/*	ot ("mvi\ta,");
//	onum(d);
//	nl (); */
    return 0;
}

assemble(s)
char	*s; {
#ifdef	ASNM
	char buf[100];
	strcpy(buf, ASNM);
	strcat(buf, " ");
	strcat(buf, s);
	buf[strlen(buf)-1] = 's';
	return(system(buf));
#else
	return(0);
#endif
}
