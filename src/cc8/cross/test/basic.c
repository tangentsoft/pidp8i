#include <libc.h>
#include <init.pa>


#define SMAX 10
#define CMAX 256
#define BMAX 64
#define LMAX 32
#define DMAX 32
#define CBMX 1024
#define LXMX 999


int E [SMAX];   /* subroutine line number stack */
int L [CMAX];   /* FOR loop beginning line number */
int M [CMAX];   /* FOR loop maximum index value */
int P [CMAX];   /* program variable value */
char Lb[CBMX];  /* Line buffer of CBMX chars */
int l,i,j;
int *C;         /* subroutine stack pointer */
char B [BMAX];  /* command input buffer */
char F [2];     /* temporary search string */
char *m [LXMX]; /* pointers to lines of program. This is a real waste of space! */
char *p,*q,*x,*y,*z,*s,*d;



G(  ) {  /* get program line from buffer */
	atoi(B,&l);
	y=m[l];
	if(y){
		if(strstr(B," "))
			strcpy(y,B);
		else
			y=m[l]=0;
		return;
	}
	y=Lb;
	while(*y)
		y=y+DMAX;
	strcpy(y,B);
	m [l]=y;
} /* end G */

/* recursive descent parser for arithmetic/logical expressions */
S(  ) {
	int o;

	o=J( );
	switch(*p++){
	case '=': return o==S( );
		break;
	case '#': return o!=S( );
	default: p--; return o;
	}
} /* end S */

J(  ) {
	int o;

	o=K( );
	switch(*p++){
	case '<': return o<J( );
		break;
	case '>': return o>J( );
	default: p--; return o;
	}
} /* end J */

K(  ) {
	int o;

	o=V( );
	switch(*p++){
	case '$': return o<=K( );
		break;
	case '!': return o>=K( );
	default: p--; return o;
	}
} /* end K */

V(  ) {
	int o;

	o=W( );
	switch(*p++){
	case '+': return o+V( );
		break;
	case '-': return o-V( );
	default: p--; return o;
	}
} /* end V */

W(  ) {
	int o;

	o=Y( );
	switch(*p++){
	case '*': return o*W( );
		break;
	case '/': return o/W( );
	default: p--; return o;
	}
} /* end W */

Y(  ) {
	int o;

	if(*p=='-'){
		p++;
		return -Y();
	}
	q=p;
	if(*p>='0'&&*p<='9'){
		while(*p>='0'&&*p<='9')
			p++;
		atoi(q,&o);
		return o;
	}
	if(*p=='('){
		p++; o=S( ); p++;
		return o;
	}
	return P [*p++];
} /* end Y */

bufclear()
{
	memset(m,0,LXMX);
	memset(Lb,0,CBMX);
}

main(  ) {
	int tmp;  /* temp var to fix bug 07Sep2005 Somos */

	bufclear();
	while(puts("Ok\r\n"),gets(B))
		switch(*B){
		case 'R': /* "RUN" command */
			C=E;
			l=1;
			for(i=0; i<CMAX; i++) /* initialize variables */
				P [i]=0;
			while(l){
				while(!(s=m [l])) l++;
				while(*s!=' ') s++;                      /* skip line number */
				   if ( ! strstr ( s , "\"" ) ) {
				 while ( ( p = strstr ( s , "<>" ) ) ) * p ++ = '#' , * p = ' ' ;
				 while ( ( p = strstr ( s , "<=" ) ) ) * p ++ = '$' , * p = ' ' ;
				 while ( ( p = strstr ( s , ">=" ) ) ) * p ++ = '!' , * p = ' ' ;
				 }
				d=B;
				j=0;
				while(*s){
					if(*s=='"') j++;
					if(*s!=' '||(j&1)) *d++=*s;
					s++;
				}
				*d=j=0;
				d--; /* backup to last char in line */
				if(B [1]!='='){
					switch(*B){
					case 'E': /* "END" */
						l=-1;
						break;
					case 'R':                       /* "REM" */
						if(B [2]!='M') l=*--C;  /* "RETURN" */
						break;
					case 'I':
						if(B [1]=='N'){                         /* "INPUT" */
							tmp=*d;                         /* save for bug fix next line 07Sep2005 Somos */
							gets(p=B); P [tmp]=S( );
						} else {                                /* "IF" */
							*(tmp=strstr(B,"TH"))=0;        /* "THEN" */
							p=B+2;
							if(S( )){
								p=tmp+4; l=S( )-1;
							}
						}
						break;
					case 'P': /* "PRINT" */
						tmp=',';
						p=B+5;
						while(tmp==','){
							if(*p=='"'){
								while(*++p!='"')
									putc(*p);
								p++;
							} else {
								printf("%d",S( ));
							}
							tmp=*p++;
							putc(' ');
						}
						puts("\r\n");
						break;
					case 'G':               /* "GOTO" */
						p=B+4;
						if(B [2]=='S'){ /* "GOSUB" */
							*C++=l; p++;
						}
						l=S( )-1;
						break;
					case 'F':                               /* "FOR" */
						*(tmp=strstr(B,"TO"))=0;        /* "TO" */
						p=B+5;
						P [i=B [3]]=S( );
						p=tmp+2;
						M [i]=S( );
						L [i]=l;
						break;
					case 'N': /* "NEXT" */
						tmp=*d;
						if(P [tmp]<M [tmp]){
							l=L [tmp];
							P [tmp]++;
						}
						break;
					}
				} else {
					p=B+2;
					P [*B]=S( );
				}
				l++;
			} /* end while l */
			break;
		case 'L': /* "LIST" command */
			for(j=0; j<LXMX; j++)
				if(m[j]){
					puts(m[j]);
					puts("\r\n");
				}
			break;
		case 'N': /* "NEW" command */
			bufclear();
			break;
		case 'B': /* "BYE" command */
			return 0;
			break;
		case 0:
		default:
			G( );
		}/* end switch *B */
	return 0;
} /* end main */
