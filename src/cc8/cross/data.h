/*	File data.h: 2.2 (84/11/27,16:26:11) */

/* storage words */

extern	char	symtab[];
extern	char	*glbptr, *rglbptr, *locptr;
extern	int	ws[];
extern	int	*wsptr;
extern	int	swstcase[];
extern	int	swstlab[];
extern	int	swstp;
extern	char	litq[];
extern	int	litptr;
extern	char	macq[];
extern	int	macptr;
extern	char	line[];
extern	char	mline[];
extern	int	lptr, mptr, gsize;

/* miscellaneous storage */

extern	int	nxtlab,
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

extern	FILE	*input, *input2, *output, *bfile;
extern	FILE	*inclstk[];
extern	int	inclsp;
extern	char	fname[];

extern	char	quote[];
extern	char	*cptr;
extern	int	*iptr;
extern	int	fexitlab;
extern	int	iflevel, skiplevel;
extern	int	errfile;
extern	int	sflag;
extern	int	cflag;
extern	int	errs;
extern	int	aflag;
