/*	File preproc.c: 2.3 (84/11/27,11:47:40) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"
#include "data.h"

/*
 *	open an include file
 */
doinclude ()
{
	char	*p;
	FILE	*inp2;

	blanks ();
	if (inp2 = fixiname ())
		if (inclsp < INCLSIZ) {
			inclstk[inclsp++] = input2;
			input2 = inp2;
		} else {
			fclose (inp2);
			error ("too many nested includes");
		}
	else {
		error ("Could not open include file");
	}
	kill ();
}

/*
 *	fixiname - remove "brackets" around include file name
 */
fixiname ()
{
	char	c1, c2, *p, *ibp;
	char buf[20];
	FILE *fp;
	char buf2[100];

	ibp = &buf[0];

	if ((c1 = gch ()) != '"' && c1 != '<')
		return (NULL);
	for (p = line + lptr; *p ;)
		*ibp++ = *p++;
	c2 = *(--p);
	if (c1 == '"' ? (c2 != '"') : (c2 != '>')) {
		error ("incorrect delimiter");
		return (NULL);
	}
	*(--ibp) = 0;
	fp = NULL;
	if (c1 == '<' || !(fp = fopen(buf, "r"))) {
		strcpy(buf2, DEFLIB);
		strcat(buf2, buf);
		fp = fopen(buf2, "r");
	}
	return(fp);
}

/*
 *	"asm" pseudo-statement
 *
 *	enters mode where assembly language statements are passed
 *	intact through parser
 *
 */
doasm ()
{
	cmode = 0;
	FOREVER {
		inln ();
		if (match ("#endasm")) {
			ol("/\t#ENDASM");
			break;
		}
		if (feof (input))
			break;
		outstr (line);
		nl ();
	}
	kill ();
	cmode = 1;
}

dodefine ()
{
	addmac();
}

doundef ()
{
	int	mp;
	char	sname[NAMESIZE];

	if (!symname(sname)) {
		illname();
		kill();
		return 0;
	}

	if (mp = findmac(sname))
		delmac(mp);
	kill();
}

preprocess ()
{
	if (ifline()) return 0;
	while (cpp());
}

doifdef (ifdef)
int ifdef;
{
	char sname[NAMESIZE];
	int k;

	blanks();
	++iflevel;
	if (skiplevel) return 0;
	k = symname(sname) && findmac(sname);
	if (k != ifdef) skiplevel = iflevel;
}

ifline()
{
	FOREVER {
		inln();
		if (feof(input)) return(1);
		if (match("#ifdef")) {
			doifdef(YES);
			continue;
		} else if (match("#ifndef")) {
			doifdef(NO);
			continue;
		} else if (match("#else")) {
			if (iflevel) {
				if (skiplevel == iflevel) skiplevel = 0;
				else if (skiplevel == 0) skiplevel = iflevel;
			} else noiferr();
			continue;
		} else if (match("#endif")) {
			if (iflevel) {
				if (skiplevel == iflevel) skiplevel = 0;
				--iflevel;
			} else noiferr();
			continue;
		}
		if (!skiplevel) return(0);
	}
}

noiferr()
{
	error("no matching #if...");
}


cpp ()
{
	int	k;
	char	c, sname[NAMESIZE];
	int	tog;
	int	cpped;		/* non-zero if something expanded */

	cpped = 0;
	/* don't expand lines with preprocessor commands in them */
	if (!cmode || line[0] == '#') return(0);

	mptr = lptr = 0;
	while (ch ()) {
		if ((ch () == '/') & (nch () == '/')) {
			inln();
		}
		if ((ch () == ' ') | (ch () == 9)) {
			keepch (' ');
			while ((ch () == ' ') | (ch () == 9))
				gch ();
		} else if (ch () == '"') {
			keepch (ch ());
			gch ();
			while (ch () != '"') {
				if (ch () == 0) {
					error ("missing quote");
					break;
				}
				if (ch() == '\\') keepch(gch());
				keepch (gch ());
			}
			gch ();
			keepch ('"');
		} else if (ch () == 39) {
			keepch (39);
			gch ();
			while (ch () != 39) {
				if (ch () == 0) {
					error ("missing apostrophe");
					break;
				}
				if (ch() == '\\') keepch(gch());
				keepch (gch ());
			}
			gch ();
			keepch (39);
		} else if ((ch () == '/') & (nch () == '*')) {
			inchar ();
			inchar ();
			while ((((c = ch ()) == '*') & (nch () == '/')) == 0)
				if (c == '$') {
					inchar ();
					tog = TRUE;
					if (ch () == '-') {
						tog = FALSE;
						inchar ();
					}
					if (alpha (c = ch ())) {
						inchar ();
						toggle (c, tog);
					}
				} else {
					if (ch () == 0)
						inln ();
					else
						inchar ();
					if (feof (input))
						break;
				}
			inchar ();
			inchar ();
		} else if (an (ch ())) {
			k = 0;
			while (an (ch ())) {
				if (k < NAMEMAX)
					sname[k++] = ch ();
				gch ();
			}
			sname[k] = 0;
			if (k = findmac (sname)) {
				cpped = 1;
				while (c = macq[k++])
					keepch (c);
			} else {
				k = 0;
				while (c = sname[k++])
					keepch (c);
			}
		} else
			keepch (gch ());
	}
	keepch (0);
	if (mptr >= MPMAX)
		error ("line too long");
	lptr = mptr = 0;
	while (line[lptr++] = mline[mptr++]);
	lptr = 0;
	return(cpped);
}

keepch (c)
char	c;
{
	mline[mptr] = c;
	if (mptr < MPMAX)
		mptr++;
	return (c);
}

defmac(s)
char *s;
{
	kill();
	strcpy(line, s);
	addmac();
}

addmac ()
{
	char	sname[NAMESIZE];
	int	k;
	int	mp;

	if (!symname (sname)) {
		illname ();
		kill ();
		return 0;
	}
	if (mp = findmac(sname)) {
		error("Duplicate define");
		delmac(mp);
	}
	k = 0;
	while (putmac (sname[k++]));
	while (ch () == ' ' | ch () == 9)
		gch ();
	while (putmac (gch ()));
	if (macptr >= MACMAX)
		error ("macro table full");
}

delmac(mp) int mp; {
	--mp; --mp;	/* step over previous null */
	while (mp >= 0 && macq[mp]) macq[mp--] = '%';
}
	

putmac (c)
char	c;
{
	macq[macptr] = c;
	if (macptr < MACMAX)
		macptr++;
	return (c);
}

findmac (sname)
char	*sname;
{
	int	k;

	k = 0;
	while (k < macptr) {
		if (astreq (sname, macq + k, NAMEMAX)) {
			while (macq[k++]);
			return (k);
		}
		while (macq[k++]);
		while (macq[k++]);
	}
	return (0);
}

toggle (name, onoff)
char	name;
int	onoff;
{
	switch (name) {
	case 'C':
		ctext = onoff;
		break;
	}
}
