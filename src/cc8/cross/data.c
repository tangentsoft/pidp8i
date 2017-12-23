/*	File data.c: 2.2 (84/11/27,16:26:13) */
/*% cc -O -c %
 *
 */

#include <stdio.h>
#include "defs.h"

/* storage words */

char	symtab[SYMTBSZ];
char	*glbptr, *rglbptr, *locptr;
int	ws[WSTABSZ];
int	*wsptr;
int	swstcase[SWSTSZ];
int	swstlab[SWSTSZ];
int	swstp;
char	litq[LITABSZ];
int	litptr;
char	macq[MACQSIZE];
int	macptr;
char	line[LINESIZE];
char	mline[LINESIZE];
int	lptr, mptr, gsize;

/* miscellaneous storage */

int	nxtlab,
	litlab,
	stkp,
	argstk,
	ncmp,
	errcnt,
	glbflag,
	ctext,
	cmode,
	lastst,
	inbreak;

FILE	*input, *input2, *output;
FILE	*inclstk[INCLSIZ];
int	inclsp;
char	fname[NAMESIZE];
FILE *bfile;

char	quote[2];
unsigned char	*cptr;
int	*iptr;
int	fexitlab;
int	iflevel, skiplevel;
int	errfile;
int	sflag;
int	cflag;
int	errs;
int	aflag;
