/*	File main.c: 2.7 (84/11/28,10:14:56) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"
#include "data.h"

main (argc, argv)
int	argc;
char** argv;        /* OS/8 CC8 can't cope, but bootstrapping CC8 doesn't work, either */
{
	char	*p,*bp;
	int smacptr;
	macptr = 0;
	ctext = 0;
	argc--; argv++;
	errs = 0;
	aflag = 1;
	while (p = *argv++)
		if (*p == '-') while (*++p)
			switch(*p) {
				case 't': case 'T':
					ctext = 1;
					break;
				case 's': case 'S':
					sflag = 1;
					break;
				case 'c': case 'C':
					cflag = 1;
					break;
				case 'a': case 'A':
					aflag = 0;
					break;
				case 'd': case 'D':
					bp = ++p;
					if (!*p) usage();
					while (*p && *p != '=') p++;
					if (*p == '=') *p = '\t';
					while (*p) p++;
					p--;
					defmac(bp);
					break;
				default:
					usage();
			}
			else break;

	smacptr = macptr;
	if (!p)
		usage();
	while (p) {
		errfile = 0;
		if (typof(p) == 'c' || typof(p) =='C') {
			glbptr = STARTGLB;
			locptr = STARTLOC;
			wsptr = ws;
			inclsp =
			iflevel =
			skiplevel =
			swstp =
			litptr =
			stkp =
			errcnt =
			ncmp =
			lastst =
			quote[1] =
			gsize =
			inbreak =
			0;
			macptr = smacptr;
			input2 = NULL;
			quote[0] = '"';
			cmode = 1;
			glbflag = 1;
			nxtlab = 0;
			litlab = getlabel ();
			defmac("end\tmemory");
			rglbptr = glbptr;
			defmac("short\tint");
			initmac();
			/*
			 *	compiler body
			 */
			if (!openin (p))
				return 0;
			if (!openout ())
				return 0;
			header ();
			gtext ();
			parse ();
			fclose (input);
			gdata ();
			dumplits ();
			dumpglbs ();
			errorsummary ();
			trailer ();
			fclose (output);
			pl ("");
			errs = errs || errfile;
#ifndef	NOASLD
		}
		if (!errfile && !sflag)
			errs = errs || assemble(p);
#else
		} else {
			fputs("Don't understand file ", stderr);
			fputs(p, stderr);
			errs = 1;
		}
#endif
		p = *argv++;
	}
	exit(errs != 0);
    return 0;
}

FEvers()
{
	outstr("/\tFront End (1.0:27/1/99)");
    return 0;
}

usage()
{
	fputs("usage: sccXXXX [-tcsa] [-dSYM[=VALUE]] files\n", stderr);
	exit(1);
}

/*
 *	process all input text
 *
 *	at this level, only static declarations, defines, includes,
 *	and function definitions are legal.
 *
 */
parse ()
{
	while (!feof (input)) {
		if (amatch ("extern", 6))
			dodcls(EXTERN);
		else if (amatch ("static",6))
			dodcls(STATIC);
		else if (dodcls(PUBLIC)) ;
		else if (match ("#asm"))
			doasm ();
		else if (match ("#include"))
			doinclude ();
		else if (match ("#define"))
			dodefine();
		else if (match ("#undef"))
			doundef();
		else
			newfunc ();
		blanks ();
	}
    return 0;
}

/*
 *		parse top level declarations
	*/

dodcls(stclass)
int stclass; {
	blanks();
	if (amatch("char", 4))
		declglb(CCHAR, stclass);
	else if (amatch("int", 3))
		declglb(CINT, stclass);
	else if (stclass == PUBLIC)
		return(0);
	else
		declglb(CINT, stclass);
	ns ();
	return(1);
}


/*
 *	dump the literal pool
 */
dumplits ()
{
	int	j, k;

/*	A loc containing the size */
	ol("\tLAP");
	ot ("\tCPAGE ");
	onum (2+litptr);
	nl();
	outbyte('L');
	printlabel (litlab);
	col();
	ot("\t");
	onum (-litptr);
	nl();
	if (litptr == 0)
		return 0;
/*	Generate a loc containing the address of the literals */
	outbyte('X');
	printlabel (litlab);
	col();
	ot("\t");
	printlabel (litlab);
	nl();
	printlabel (litlab);
	col ();
	k = 0;
	while (k < litptr) {
		defbyte ();
		j = 8;
		while (j--) {
			onum (litq[k++] & 127);
			if ((j == 0) | (k >= litptr)) {
				nl ();
				break;
			}
			outbyte (';');
		}
	}
	ol("\tEAP");
    return 0;
}

/*
 *	dump all static variables
 */
dumpglbs ()
{
	int	j;

	if (!glbflag) {
		ot("GBLS,\t0");
		nl();
		return 0;
	}
	cptr = rglbptr;
	while (cptr < glbptr) {
		if (cptr[IDENT] != FUNCTION) {
			ppubext(cptr);
			if (cptr[STORAGE] != EXTERN) {
				//prefix ();
				//outstr (cptr);
				//col ();
				//defstorage ();
				j = glint(cptr);
				if ((cptr[TYPE] == CINT) ||
				    (cptr[IDENT] == POINTER))
					j = j * intsize();
				//onum (j);
				//nl ();
			}
		} else {
			fpubext(cptr);
		}
		cptr = cptr + SYMSIZ;
	}
  ot("GBLS,\t");
  onum(gsize+128);				// Beginning of stack after globals
  nl();
  return 0;
}

/*
 *	report errors
 */
errorsummary ()
{
	if (ncmp)
		error ("missing closing bracket");
	nl ();
	comment ();
	outdec (errcnt);
	if (errcnt) errfile = YES;
	outstr (" error(s) in compilation");
	nl ();
	comment();
	ot("literal pool:");
	outdec(litptr);
	nl();
	comment();
	ot("global pool:");
	outdec(glbptr-rglbptr);
	nl();
	comment();
	ot("Macro pool:");
	outdec(macptr);
	nl();
	pl (errcnt ? "Error(s)" : "No errors");
    return 0;
}

typof(s)
char	*s; {
	s += strlen(s) - 2;
	if (*s == '.')
		return(*(s+1));
	return(' ');
}
