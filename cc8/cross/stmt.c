/*	File stmt.c: 2.1 (83/03/20,16:02:17) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"
#include "data.h"

/*
 *	statement parser
 *
 *	called whenever syntax requires a statement.  this routine
 *	performs that statement and returns a number telling which one
 *
 *	'func' is true if we require a "function_statement", which
 *	must be compound, and must contain "statement_list" (even if
 *	"declaration_list" is omitted)
 */

statement (func)
int	func;
{
	if ((ch () == 0) & feof (input))
		return (0);
	lastst = 0;
	if (func)
		if (match ("{")) {
			compound (YES);
			return (lastst);
		} else
			error ("function requires compound statement");
	if (match ("{"))
		compound (NO);
	else
		stst ();
	return (lastst);
}

/*
 *	declaration
 */
stdecl ()
{
	if (amatch("register", 8))
		doldcls(DEFAUTO);
	else if (amatch("auto", 4))
		doldcls(DEFAUTO);
	else if (amatch("static", 6))
		doldcls(LSTATIC);
	else if (doldcls(AUTO)) ;
	else
		return (NO);
	return (YES);
}

doldcls(stclass)
int	stclass;
{
	blanks();
	if (amatch("char", 4))
		declloc(CCHAR, stclass);
	else if (amatch("int", 3))
		declloc(CINT, stclass);
	else if (stclass == LSTATIC || stclass == DEFAUTO)
		declloc(CINT, stclass);
	else
		return(0);
	ns();
	return(1);
}


/*
 *	non-declaration statement
 */
stst ()
{
	if (amatch ("if", 2)) {
		doif ();
		lastst = STIF;
	} else if (amatch ("while", 5)) {
		dowhile ();
		lastst = STWHILE;
	} else if (amatch ("switch", 6)) {
		doswitch ();
		lastst = STSWITCH;
	} else if (amatch ("do", 2)) {
		dodo ();
		ns ();
		lastst = STDO;
	} else if (amatch ("for", 3)) {
		dofor ();
		lastst = STFOR;
	} else if (amatch ("return", 6)) {
		doreturn ();
		ns ();
		lastst = STRETURN;
	} else if (amatch ("break", 5)) {
		dobreak ();
		ns ();
		lastst = STBREAK;
	} else if (amatch ("continue", 8)) {
		docont ();
		ns ();
		lastst = STCONT;
	} else if (match (";"))
		;
	else if (amatch ("case", 4)) {
		docase ();
		lastst = statement (NO);
	} else if (amatch ("default", 7)) {
		dodefault ();
		lastst = statement (NO);
	} else if (match ("#asm")) {
		doasm ();
		lastst = STASM;
	} else if (match ("{"))
		compound (NO);
	else {
		expression (YES);
/*		if (match (":")) {
			dolabel ();
			lastst = statement (NO);
		} else {
*/			ns ();
			lastst = STEXP;
/*		}
*/	}
}

/*
 *	compound statement
 *
 *	allow any number of statements to fall between "{" and "}"
 *
 *	'func' is true if we are in a "function_statement", which
 *	must contain "statement_list"
 */
compound (func)
int	func;
{
	int	decls;

	decls = YES;
	ncmp++;
	while (!match ("}")) {
		if (feof (input))
			return;
		if (decls) {
			if (!stdecl ())
				decls = NO;
		} else
			stst ();
	}
	ncmp--;
}

/*
 *	"if" statement
 */
doif ()
{
	int	fstkp, flab1, flab2;
	char	*flev;

	flev = locptr;
	fstkp = stkp;
	flab1 = getlabel ();
	test (flab1, FALSE);
	statement (NO);
	stkp = modstk (fstkp);
	locptr = flev;
	if (!amatch ("else", 4)) {
		gnlabel (flab1);
		return;
	}
	jump (flab2 = getlabel ());
	gnlabel (flab1);
	statement (NO);
	stkp = modstk (fstkp);
	locptr = flev;
	gnlabel (flab2);
}

/*
 *	"while" statement
 */
dowhile ()
{
	int	ws[7];

	ws[WSSYM] = locptr;
	ws[WSSP] = stkp;
	ws[WSTYP] = WSWHILE;
	ws[WSTEST] = getlabel ();
	ws[WSEXIT] = getlabel ();
	addwhile (ws);
	gnlabel (ws[WSTEST]);
	test (ws[WSEXIT], FALSE);
	statement (NO);
	jump (ws[WSTEST]);
	gnlabel (ws[WSEXIT]);
	locptr = ws[WSSYM];
	stkp = modstk (ws[WSSP]);
	delwhile ();
}

/*
 *	"do" statement
 */
dodo ()
{
	int	ws[7];

	ws[WSSYM] = locptr;
	ws[WSSP] = stkp;
	ws[WSTYP] = WSDO;
	ws[WSBODY] = getlabel ();
	ws[WSTEST] = getlabel ();
	ws[WSEXIT] = getlabel ();
	addwhile (ws);
	gnlabel (ws[WSBODY]);
	statement (NO);
	if (!match ("while")) {
		error ("missing while");
		return;
	}
	gnlabel (ws[WSTEST]);
	test (ws[WSBODY], TRUE);
	gnlabel (ws[WSEXIT]);
	locptr = ws[WSSYM];
	stkp = modstk (ws[WSSP]);
	delwhile ();
}

/*
 *	"for" statement
 */
dofor ()
{
	int	ws[7],
		*pws;

	ws[WSSYM] = locptr;
	ws[WSSP] = stkp;
	ws[WSTYP] = WSFOR;
	ws[WSTEST] = getlabel ();
	ws[WSINCR] = getlabel ();
	ws[WSBODY] = getlabel ();
	ws[WSEXIT] = getlabel ();
	addwhile (ws);
	pws = readwhile ();
	needbrack ("(");
	if (!match (";")) {
		expression (YES);
		ns ();
	}
	gnlabel (pws[WSTEST]);
	if (!match (";")) {
		expression (YES);
		testjump (pws[WSBODY], TRUE);
		jump (pws[WSEXIT]);
		ns ();
	} else
		pws[WSTEST] = pws[WSBODY];
	gnlabel (pws[WSINCR]);
	if (!match (")")) {
		expression (YES);
		needbrack (")");
		jump (pws[WSTEST]);
	} else
		pws[WSINCR] = pws[WSTEST];
	gnlabel (pws[WSBODY]);
	statement (NO);
	jump (pws[WSINCR]);
	gnlabel (pws[WSEXIT]);
	locptr = pws[WSSYM];
	stkp = modstk (pws[WSSP]);
	delwhile ();
}

/*
 *	"switch" statement
 */
doswitch ()
{
	int	ws[7];
	int	*ptr;

	ws[WSSYM] = locptr;
	ws[WSSP] = stkp;
	ws[WSTYP] = WSSWITCH;
	ws[WSCASEP] = swstp;
	ws[WSTAB] = getlabel ();
	ws[WSDEF] = ws[WSEXIT] = getlabel ();
	addwhile (ws);
//	immed ();
//	printlabel (ws[WSTAB]);
//	nl ();
//	gpush ();
	needbrack ("(");
	expression (YES);
	needbrack (")");
//	stkp = stkp + intsize();  /* '?case' will adjust the stack */
	gjcase ();
	jump (ws[WSTAB]);
	statement (NO);
	ptr = readswitch ();
//	if (ptr[WSDEF]!=ptr[WSEXIT]) jump (ptr[WSDEF]);
	jump (ptr[WSEXIT]);
	dumpsw (ptr);
	gnlabel (ptr[WSEXIT]);
	locptr = ptr[WSSYM];
//	stkp = modstk (ptr[WSSP]);
	swstp = ptr[WSCASEP];
	delwhile ();
}

/*
 *	"case" label
 */
docase ()
{
	int	val;

	val = 0;
	if (readswitch ()) {
		if (!number (&val))
			if (!pstr (&val))
				error ("bad case label");
		addcase (val);
		if (!match (":"))
			error ("missing colon");
	} else
		error ("no active switch");
}

/*
 *	"default" label
 */
dodefault ()
{
	int	*ptr,
		lab;

	if (ptr = readswitch ()) {
		ptr[WSDEF] = lab = getlabel ();
		gnlabel (lab);
		if (!match (":"))
			error ("missing colon");
	} else
		error ("no active switch");
}

/*
 *	"return" statement
 */
doreturn ()
{
	if (endst () == 0)
		expression (YES);
	jump(fexitlab);
}

/*
 *	"break" statement
 */
dobreak ()
{
	int	*ptr;

	if ((ptr = readwhile ()) == 0)
		return;
	modstk (ptr[WSSP]);
	jump (ptr[WSEXIT]);
}

/*
 *	"continue" statement
 */
docont ()
{
	int	*ptr;

	if ((ptr = findwhile ()) == 0)
		return;
/*	modstk (ptr[WSSP]); */
	if (ptr[WSTYP] == WSFOR)
		jump (ptr[WSINCR]);
	else
		jump (ptr[WSTEST]);
}

/*
 *	dump switch table
 */
dumpsw (ws)
int	ws[];
{
	int	i,j;

	gdata ();
	gnlabel (ws[WSTAB]);
	if (ws[WSCASEP] != swstp) {
		j = ws[WSCASEP];
		while (j < swstp) {
			i = 4;
			while (i--) {
				immd3();
				onum (swstcase[j]);
				nl();
				casejump();
				jump (swstlab[j++]);
				if ((i == 0) | (j >= swstp)) {
					nl ();
					break;
				}
				nl();
			}
		}
		jump(ws[WSDEF]);
	}
	gtext ();
}
