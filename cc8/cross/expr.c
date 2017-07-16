/*	File expr.c: 2.2 (83/06/21,11:24:26) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"
#include "data.h"

/*
 *	lval[0] - symbol table address, else 0 for constant
 *	lval[1] - type indirect object to fetch, else 0 for static object
 *	lval[2] - type pointer or array, else 0
 */

expression (comma)
int	comma;
{
	int	lval[3];

	do {
		if (heir1 (lval))
			rvalue (lval);
		if (!comma)
			return;
	} while (match (","));
}

heir1 (lval)
int	lval[];
{
	int	k, lval2[3];
	char	fc;

	k = heir1a (lval);
	if (match ("=")) {
		if (k == 0) {
			needlval ();
			return (0);
		}
		if (lval[1])
			gpush ();
		if (heir1 (lval2))
			rvalue (lval2);
		store (lval);
		return (0);
	} else
	{	
		fc = ch();
		if  (match ("-=") ||
		    match ("+=") ||
		    match ("*=") ||
		    match ("/=") ||
		    match ("%=") ||
		    match (">>=") ||
		    match ("<<=") ||
		    match ("&=") ||
		    match ("^=") ||
		    match ("|=")) {
			if (k == 0) {
				needlval ();
				return (0);
			}
			if (lval[1])
				gpush ();
			rvalue (lval);
			gpush ();
			if (heir1 (lval2))
				rvalue (lval2);
			switch (fc) {
				case '-':	{
					if (dbltest(lval,lval2))
						gaslint();
					gsub();
					result (lval, lval2);
					break;
				}
				case '+':	{
					if (dbltest(lval,lval2))
						gaslint();
					gadd (lval,lval2);
					result(lval,lval2);
					break;
				}
				case '*':	gmult (); break;
				case '/':	gdiv (); break;
				case '%':	gmod (); break;
				case '>':	gasr (); break;
				case '<':	gasl (); break;
				case '&':	gand (); break;
				case '^':	gxor (); break;
				case '|':	gor (); break;
			}
			store (lval);
			return (0);
		} else
			return (k);
	}
}

heir1a (lval)
int	lval[];
{
	int	k, lval2[3], lab1, lab2;

	k = heir1b (lval);
	blanks ();
	if (ch () != '?')
		return (k);
	if (k)
		rvalue (lval);
	FOREVER
		if (match ("?")) {
			testjump (lab1 = getlabel (), FALSE);
			if (heir1b (lval2))
				rvalue (lval2);
			jump (lab2 = getlabel ());
			printlabel (lab1);
			col ();
			nl ();
			blanks ();
			if (!match (":")) {
				error ("missing colon");
				return (0);
			}
			if (heir1b (lval2))
				rvalue (lval2);
			printlabel (lab2);
			col ();
			nl ();
		} else
			return (0);
}

heir1b (lval)
int	lval[];
{
	int	k, lval2[3], lab;

	k = heir1c (lval);
	blanks ();
	if (!sstreq ("||"))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER
		if (match ("||")) {
			testjump (lab = getlabel (), TRUE);
			if (heir1c (lval2))
				rvalue (lval2);
			printlabel (lab);
			col ();
			nl ();
			gbool();
		} else
			return (0);
}

heir1c (lval)
int	lval[];
{
	int	k, lval2[3], lab;

	k = heir2 (lval);
	blanks ();
	if (!sstreq ("&&"))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER
		if (match ("&&")) {
			testjump (lab = getlabel (), FALSE);
			if (heir2 (lval2))
				rvalue (lval2);
			printlabel (lab);
			col ();
			nl ();
			gbool();
		} else
			return (0);
}

heir2 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir3 (lval);
	blanks ();
	if ((ch() != '|') | (nch() == '|') | (nch() == '='))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if ((ch() == '|') & (nch() != '|') & (nch() != '=')) {
			inbyte ();
			gpush ();
			if (heir3 (lval2))
				rvalue (lval2);
			gor ();
			blanks();
		} else
			return (0);
	}
}

heir3 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir4 (lval);
	blanks ();
	if ((ch () != '^') | (nch() == '='))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if ((ch() == '^') & (nch() != '=')){
			inbyte ();
			gpush ();
			if (heir4 (lval2))
				rvalue (lval2);
			gxor ();
			blanks();
		} else
			return (0);
	}
}

heir4 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir5 (lval);
	blanks ();
	if ((ch() != '&') | (nch() == '|') | (nch() == '='))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if ((ch() == '&') & (nch() != '&') & (nch() != '=')) {
			inbyte ();
			gpush ();
			if (heir5 (lval2))
				rvalue (lval2);
			gand ();
			blanks();
		} else
			return (0);
	}
}

heir5 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir6 (lval);
	blanks ();
	if (!sstreq ("==") &
	    !sstreq ("!="))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if (match ("==")) {
			gpush ();
			if (heir6 (lval2))
				rvalue (lval2);
			geq ();
		} else if (match ("!=")) {
			gpush ();
			if (heir6 (lval2))
				rvalue (lval2);
			gne ();
		} else
			return (0);
	}
}

heir6 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir7 (lval);
	blanks ();
	if (!sstreq ("<") &&
	    !sstreq ("<=") &&
	    !sstreq (">=") &&
	    !sstreq (">"))
		return (k);
	if (sstreq ("<<") || sstreq (">>"))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if (match ("<=")) {
			gpush ();
			if (heir7 (lval2))
				rvalue (lval2);
			if (lval[2] || lval2[2]) {
				gule ();
				continue;
			}
			gle ();
		} else if (match (">=")) {
			gpush ();
			if (heir7 (lval2))
				rvalue (lval2);
			if (lval[2] || lval2[2]) {
				guge ();
				continue;
			}
			gge ();
		} else if ((sstreq ("<")) &&
			   !sstreq ("<<")) {
			inbyte ();
			gpush ();
			if (heir7 (lval2))
				rvalue (lval2);
			if (lval[2] || lval2[2]) {
				gult ();
				continue;
			}
			glt ();
		} else if ((sstreq (">")) &&
			   !sstreq (">>")) {
			inbyte ();
			gpush ();
			if (heir7 (lval2))
				rvalue (lval2);
			if (lval[2] || lval2[2]) {
				gugt ();
				continue;
			}
			ggt ();
		} else
			return (0);
		blanks ();
	}
}

heir7 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir8 (lval);
	blanks ();
	if (!sstreq (">>") &&
	    !sstreq ("<<") || sstreq(">>=") || sstreq("<<="))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if (sstreq(">>") && ! sstreq(">>=")) {
			inbyte(); inbyte();
			gpush ();
			if (heir8 (lval2))
				rvalue (lval2);
			gasr ();
		} else if (sstreq("<<") && ! sstreq("<<=")) {
			inbyte(); inbyte();
			gpush ();
			if (heir8 (lval2))
				rvalue (lval2);
			gasl ();
		} else
			return (0);
		blanks();
	}
}

heir8 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir9 (lval);
	blanks ();
	if ((ch () != '+') & (ch () != '-') | nch() == '=')
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if (match ("+")) {
			gpush ();
			if (heir9 (lval2))
				rvalue (lval2);
			/* if left is pointer and right is int, scale right */
			if (dbltest (lval, lval2))
				gaslint ();
			/* will scale left if right int pointer and left int */
			gadd (lval,lval2);
			result (lval, lval2);
		} else if (match ("-")) {
			gpush ();
			if (heir9 (lval2))
				rvalue (lval2);
			/* if dbl, can only be: pointer - int, or
						pointer - pointer, thus,
				in first case, int is scaled up,
				in second, result is scaled down. */
			if (dbltest (lval, lval2))
				gaslint ();
			gsub ();
			/* if both pointers, scale result */
			if ((lval[2] == CINT) && (lval2[2] == CINT)) {
				gasrint(); /* divide by intsize */
			}
			result (lval, lval2);
		} else
			return (0);
	}
}

heir9 (lval)
int	lval[];
{
	int	k, lval2[3];

	k = heir10 (lval);
	blanks ();
	if (((ch () != '*') && (ch () != '/') &&
		(ch () != '%')) || (nch() == '='))
		return (k);
	if (k)
		rvalue (lval);
	FOREVER {
		if (match ("*")) {
			gpush ();
			if (heir10 (lval2))
				rvalue (lval2);
			gmult ();
		} else if (match ("/")) {
			gpush ();
			if (heir10 (lval2))
				rvalue (lval2);
			gdiv ();
		} else if (match ("%")) {
			gpush ();
			if (heir10 (lval2))
				rvalue (lval2);
			gmod ();
		} else
			return (0);
	}
}

heir10 (lval)
int	lval[];
{
	int	k;
	unsigned char	*ptr;

	if (match ("++")) {
		if ((k = heir10 (lval)) == 0) {
			needlval ();
			return (0);
		}
//		if (lval[1])
//		gpush ();
		rivalue (lval);
//		ginc (lval);
//		store (lval);
		return (0);
	} else if (match ("--")) {
		if ((k = heir10 (lval)) == 0) {
			needlval ();
			return (0);
		}
		if (lval[1])
			gpush ();
		rvalue (lval);
		gdec (lval);
		store (lval);
		return (0);
	} else if (match ("-")) {
		k = heir10 (lval);
		if (k)
			rvalue (lval);
		gneg ();
		return (0);
	} else if (match ("~")) {
		k = heir10 (lval);
		if (k)
			rvalue (lval);
		gcom ();
		return (0);
	} else if (match ("!")) {
		k = heir10 (lval);
		if (k)
			rvalue (lval);
		glneg ();
		return (0);
	} else if (ch()=='*' && nch() != '=') {
		inbyte();
		k = heir10 (lval);
		if (k)
			rvalue (lval);
		if (ptr = lval[0])
			lval[1] = ptr[TYPE];
		else
			lval[1] = CINT;
		lval[2] = 0;  /* flag as not pointer or array */
		return (1);
	} else if (ch()=='&' && nch()!='&' && nch()!='=') {
		inbyte();
		k = heir10 (lval);
		if (k == 0) {
			error ("illegal address");
			return (0);
		}
		ptr = lval[0];
		lval[2] = ptr[TYPE];
		if (lval[1])
			return (0);
		/* global and non-array */
		immed ();
		k=128+ptr[OFFSET]+ptr[OFFSET+1]*256;
		onum(k);
		ot("\t/Offset from stackbase at 128 (200(8))");
		nl ();
		lval[1] = ptr[TYPE];
		return (0);
	} else {
		k = heir11 (lval);
		if (match ("++")) {
			if (k == 0) {
				needlval ();
				return (0);
			}
//			if (lval[1])
//				gpush ();
			rvalue (lval);
			gisz (lval);
//			ginc (lval);
//			store (lval);
//			gdec (lval);
			return (0);
		} else if (match ("--")) {
			if (k == 0) {
				needlval ();
				return (0);
			}
			if (lval[1])
				gpush ();
			rvalue (lval);
			gdec (lval);
			store (lval);
			ginc (lval);
			return (0);
		} else
			return (k);
	}
}

heir11 (lval)
int	*lval;
{
	int	k;
	char	*ptr;

	k = primary (lval);
	ptr = lval[0];
	blanks ();
	if ((ch () == '[') | (ch () == '('))
		FOREVER {
			if (match ("[")) {
				if (ptr == 0) {
					error ("can't subscript");
					junk ();
					needbrack ("]");
					return (0);
				} else if (ptr[IDENT] == POINTER)
					rvalue (lval);
				else if (ptr[IDENT] != ARRAY) {
					error ("can't subscript");
					k = 0;
				}
				gpush ();
				expression (YES);
				needbrack ("]");
				if (ptr[TYPE] == CINT)
					gaslint ();
				gadd (NULL,NULL);
				lval[0] = 0;
				lval[1] = ptr[TYPE];
				k = 1;
			} else if (match ("(")) {
				if (ptr == 0)
					callfunction (0);
				else if (ptr[IDENT] != FUNCTION) {
					rvalue (lval);
					callfunction (0);
				} else
					callfunction (ptr);
				k = lval[0] = 0;
			} else
				return (k);
		}
	if (ptr == 0)
		return (k);
	if (ptr[IDENT] == FUNCTION) {
		immed ();
		prefix ();
		outstr (ptr);
		nl ();
		return (0);
	}
	return (k);
}
