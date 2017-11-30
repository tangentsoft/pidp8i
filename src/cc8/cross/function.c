/*	File function.c: 2.1 (83/03/20,16:02:04) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"
#include "data.h"

/*
 *	begin a function
 *
 *	called from "parse", this routine tries to make a function out
 *	of what follows
 *	modified version.  p.l. woods
 *
 */
int argtop;
newfunc ()
{
	char	n[NAMESIZE], *ptr, rtn[NAMESIZE];
	fexitlab = getlabel();

	if (!symname (n) ) {
		error ("illegal function or declaration");
		kill_line ();
		return 0;
	}
	if (ptr = findglb (n)) {
		if (ptr[IDENT] != FUNCTION)
			multidef (n);
		else if (ptr[OFFSET] == FUNCTION)
			multidef (n);
		else
			ptr[OFFSET] = FUNCTION;
	} else
		addglb (n, FUNCTION, CINT, 0, PUBLIC);  // Do not allocate any storage to global functions
	if (!match ("("))
		error ("missing open paren");
	prefix ();
	if (astreq(n,"main",4)) {
		if (inbreak) {
		ol("\tEND");
		output=bfile;
		}
		outstr("xmain");
	}
	else outstr (n);
	strcpy(rtn,n);
	col ();
//	outstr("\t0");
	nl ();
	if (inbreak) {
		ol("\tCLA CLL");
		ol("\tCALL 2,PGINIT");
		ol("\tARG STKP");
		ol("\tARG GBL");
	}
	prologue (rtn);
	locptr = STARTLOC;
	argstk = 0;
	while (!match (")")) {
		if (symname (n)) {
			if (findloc (n))
				multidef (n);
			else {
				addloc (n, 0, 0, argstk, AUTO);
				argstk = argstk + intsize();
			}
		} else {
			error ("illegal argument name");
			junk ();
		}
		blanks ();
		if (!streq (line + lptr, ")")) {
			if (!match (","))
				error ("expected comma");
		}
		if (endst ())
			break;
	}
	stkp = 0;
	argtop = argstk;
	while (argstk) {
		if (amatch ("register", 8)) {
			if (amatch("char", 4)) 
				getarg(CCHAR);
			else if (amatch ("int", 3))
				getarg(CINT);
			else
				getarg(CINT);
			ns();
		} else if (amatch ("char", 4)) {
			getarg (CCHAR);
			ns ();
		} else if (amatch ("int", 3)) {
			getarg (CINT);
			ns ();
		} else {
			error ("wrong number args");
			break;
		}
	}
	statement(YES);
	printlabel(fexitlab);
	col();
	nl();
	if (astreq(n,"main",4))	/* On exit from main pop literal table as well */
		modstk(0);
	else 
		modstk (0); 
	gret (rtn);
	stkp = 0;
	locptr = STARTLOC;
    return 0;
}

/*
 *	declare argument types
 *
 *	called from "newfunc", this routine add an entry in the local
 *	symbol table for each named argument
 *	completely rewritten version.  p.l. woods
 *
 */
getarg (t)
int	t;
{
    int	j, legalname, address;
    char	n[NAMESIZE], c, *argptr;

	FOREVER {
		if (argstk == 0)
			return 0;
		if (match ("*"))
			j = POINTER;
		else
			j = VARIABLE;
		if (!(legalname = symname (n)))
			illname ();
		if (match ("[")) {
			while (inbyte () != ']')
				if (endst ())
					break;
			j = POINTER;
		}
		if (legalname) {
			if (argptr = findloc (n)) {
				argptr[IDENT] = j;
				argptr[TYPE] = t;
				address = argtop - glint(argptr);
				if (t == CCHAR && j == VARIABLE)
					address = address + byteoff();
				argptr[OFFSET] = (address) & 0xff;
				argptr[OFFSET + 1] = (address >> 8) & 0xff;
			} else
				error ("expecting argument name");
		}
		argstk = argstk - intsize();
		if (endst ())
			return 0;
		if (!match (","))
			error ("expected comma");
	}
}
