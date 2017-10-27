/*	File gen.c: 2.1 (83/03/20,16:02:06) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"
#include "data.h"

/* ToUpper routine */

	ucase(ch)
	int ch;
	{
		if ('a'>ch || ch>'z') return(ch);
		return(ch-32);
	}
/*
 *	return next available internal label number
 *
 */
getlabel ()
{
	return (nxtlab++);
}

/*
 *	print specified number as label
 */
printlabel (label)
int	label;
{
	olprfix ();
	outdec (label);
}

/*
 *	glabel - generate label
 */
glabel (lab)
char	*lab;
{
	prefix ();
	outstr (lab);
	col ();
	nl ();
}

/*
 *	gnlabel - generate numeric label
 */
gnlabel (nlab)
int	nlab;
{
	printlabel (nlab);
	col ();
	nl ();
}

outbyte (c)
char	c;
{
	if (c == 0)
		return (0);
	fputc (c, output);
	return (c);
}

outstr (ptr)
char	ptr[];
{
	int	k;

	k = 0;
	while (outbyte (ucase(ptr[k++])));
}


tab ()
{
	outbyte (9);
}

ol (ptr)
char	ptr[];
{
	ot (ptr);
	nl ();
}

ot (ptr)
char	ptr[];
{
	outstr (ptr);
}

outdec (number)
int	number;
{
	int	k, zs;
	char	c;

	if (number == -32768) {
		outstr ("-32768");
		return 0;
	}
	zs = 0;
	k = 10000;
	if (number < 0) {
		number = (-number);
		outbyte ('-');
	}
	while (k >= 1) {
		c = number / k + '0';
		if ((c != '0' | (k == 1) | zs)) {
			zs = 1;
			outbyte (c);
		}
		number = number % k;
		k = k / 10;
	}
}
		
store (lval)
int	*lval;
{
	if (lval[1] == 0)
		putmem (lval[0]);
	else
		putstk (lval[1]);
}

rvalue (lval)
int	*lval;
{
	if ((lval[0] != 0) & (lval[1] == 0))
		getmem (lval[0]);
	else
		indirect (lval[1]);
}

rivalue (lval)
int	*lval;
{
	if ((lval[0] != 0) & (lval[1] == 0))
		getincmem (lval[0]);
	else
		incdirect (lval[1]);
}

test (label, ft)
int	label,
	ft;
{
	needbrack ("(");
	expression (YES);
	needbrack (")");
	testjump (label, ft);
}
