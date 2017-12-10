/*
 * This file is part of the CC8 OS/8 C compiler.
 *
 * The CC8 OS/8 C compiler is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The CC8 OS/8 C compiler is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the CC8 OS/8 C compiler as ../GPL3.txt.  If not, see
 * <http://www.gnu.org/licenses/>.
 */


#include <libc.h>
#include <init.h>


#define SMAX 10
#define CMAX 256
#define BMAX 64
#define LMAX 32
#define DMAX 32
#define CBMX 1024
#define LXMX 999

int asm[CBMX];
int ltbf[512];
int xlt[CMAX];
int gm[CMAX];		/* Global symbol table */
int tkbf[LMAX];
int *p,*q,*s,*ltpt;
int gsym,lsym,gadr,ladr,stkp,lctr,*fptr,gsz,ctr,tm,ectr,cop;
int glim,*n,ccm;
int tmp;
int tkn[BMAX];
int bfr[BMAX];
int tmbf[LMAX];
int smbf[LMAX];
int Lb[BMAX];
int lm[CMAX];		/* Auto symbol table */
int fstk[BMAX];		/* Push down stack for For etc. */
int inproc,addr,cbrk;
int izf,ixf,idf,ssz,icd;


skpsp()
{
	while (isspace(*p))
		p++;
}

getsym()
{
	q=tkbf;
	skpsp();
	while (isalnum(*p))
		*q++=*p++;
	*q=0;
	skpsp();
	return *tkbf;
}


/* recursive descent parser for arithmetic/logical expressions */

S(  ) {

	cop=0;
	J( );
	switch(*p++){
	case '=':
		S();
		stri(1);
		stkp--;
		break;
	case ']':
	case ')':
		return 1;
	case ',':
		break;
	default: 
		p--;
	}
	return 0;
} /* end S */

J(  ) {

	K( );
	switch(*p++){
	case '&': J( ); stri(20); break;
	case '|': J( ); stri(-20); break;
	default: p--; return;
	}
	stkp--;
} /* end J */

K(  ) {

	V( );
	switch(*p++){
	case '<': K( ); stri(11); break;
	case '>': K( ); stri(-11); break;
	case '$': K( ); stri(24); break;
	default: p--; return;
	}
	stkp--;
} /* end K */

V(  ) {

	W( );
	switch(*p++){
	case '+': V(); stri(2); break;
	case '-': V(); stri(3); break;
	default: p--; return;
	}
	stkp--;
} /* end V */

W(  ) {

	Y( );
	skpsp();
	cop=*p;
	switch(*p++) {
	case '*': W( ); stri(13); break;
	case '/': W( ); stri(14); break;
	case '%': W( ); stri(14);stri(-14); break;
	case '=': if (*p=='=') {
				*p='$';return;
			  }
	default: p--; return;
	}
	stkp--;
} /* end W */


Y(  ) {
	int o,ctx;
	int txbf[10];

	skpsp();

	if (!*p)
		return;

	if (cop) {
		stri(19);
		stkp++;
	}

	if (*p=='"') {
		stri(10);
		stri(ltpt-ltbf);
		while (*++p-'"') {
			if (*p=='\\')
				switch (*++p) {
				case 'r':
					*p=13;
					break;
				case 'n':
					*p=10;
			}
			*ltpt++=*p;
		}
		*ltpt++=0;
		p++;
		return;
	}
	n=q=p;
	if (*p=='-')
		p++;
	if(isdigit(*p)) {
		while(isdigit(*p))
			p++;
		stri(4);
		atoi(q,&tmp);
		stri(tmp);
		return;
	}
	if (*p==39) {
		stri(4);
		stri(*++p);
		p+=2;
		return;
	}
	ixf=izf=idf=icd=0;
	if (!getsym()) {
		switch (*p++) {
			case '&':
				getsym();
				stri(21);
				stri(fndlcl(tkbf));
				return;
			case '*':
				getsym();
				ixf++;
				break;
			case '!':
				Y();
				stri(26);
				return;
			case '~':
				Y();
				stri(-26);
				return;
			case '(':
				S();
				return;
			case ')':
				icd=1;
				return;
		}
	}
	if(*p=='('){
		strcpy(txbf,tkbf);
		ctx=o=0;p++;
		while (*p && !o) {
			o=S( );
			if (icd)
				break;
			stkp++;
			stri(19);
			ctx++;		/* arg count */
		}
		stri(9);
		stri(ctx);
		stkp-=ctx;
		if ((o=strstr(gm,txbf))){
			stri(o-gm);
		}
		else {
			stri(gsz);
			strpad(txbf);
			strcat(gm,smbf);
			gsz+=9;
		}
		return;
	}
	/* Digraphs */

	q=p+1;
	if (tmp=*q==*p) 
		switch (*p) {
		case '+':
			izf=-tmp;
			p+=2;
			break;
		case '-':
			idf=-tmp;
			p+=2;
			break;
	}

	o=fndlcl(tkbf);
	tmp=-17;
	if (ssz>1) {
		if (*p=='[') {
			stri(21);
			stri(o);
			stri(19);
			stkp++;
			p++;S();
			stri(2);
			if (*p=='=')
				stri(19);
			else {
				stri(22);
				stkp--;
			}
			return;
		}
		tmp=21;
	}
	switch (*p) {
		case '=':
			if (*q=='=')
				break;
			tmp=8;
			if (ixf)
				tmp=-8;
			ixf=0;
			stkp++;
			break;
	}
	stri(tmp);
	stri(o);
	if (*n=='-')
		stri(27);
	if (izf)
		stri(15);
	if (idf)
		stri(25);
	if (ixf)
		stri(22);
	return;
} /* end Y */

procst(trm)
char trm;
{
	ccm=ctr=1;
	p=q=Lb;
	while(1) {
		tm=fgetc();
		ctr-=tm=='(';
		ctr+=tm==')';
		ccm-=tm==',';
		if (!ctr || tm==trm)
			break;
		*q++=tm;
	}
	*q=0;
	if (inproc)
		while (*p)
			S();
}

strpad(sym)
char *sym;
{
	char *a,*b;

	strcpy(a=smbf,"         ");  /* 9 spaces */
	while (*sym)
		*a++=*sym++;
}

addsym(sym,sz)
char *sym;
int sz;
{
	strpad(sym);
	smbf[8]=sz;
	if (inproc+(sz<0)) {
		smbf[7]=stkp+1;
		stkp+=sz;
		strcat(lm,smbf);
		stri(6);
		stri(sz);
		return;
	}
	smbf[7]=gadr;
	gadr+=sz;
	strcat(gm,smbf);
	gsz+=9;
}

fndlcl(sym)
char *sym;
{
	strpad(sym);
	smbf[7]=0;
	if (s=strstr(lm,smbf)) {
		ssz=s[8];
		s=s+7;
		return *s-stkp;
	}
	if (s=strstr(gm,smbf)) {
		ssz=s[8];
		s=s+7;
		return *s;
	}
	return 0;
}

gettk()
{
	char xtm;

	q=tkbf;
	while (isspace(xtm=fgetc()));
	while (isalnum(xtm)) {
		*q++=xtm;
		xtm=fgetc();
	}
	*q=0;
	return xtm;
}

popfr()
{
	while (*fptr==inproc) {
		cbrk=*--fptr;
		stri(23);
		stri(*--fptr);
		stri(5);
		stri(*fptr+2);
		fptr--;
	}
}

dostt()
{
	p=tmbf;
	while (tm!=';') {
		*p++=tm;
		tm=fgetc();
	}
	*p=0;
	strcpy(Lb,tkbf);
	strcat(Lb,tmbf);
	p=Lb;
	S();
	tm=1;
}

fnbrk()
{
	while (tm!='(')
		tm=fgetc();
}

next()
{
	char *lp;
	int fflg;

	lp=0;
	if (*tkbf) {
		strcat(tkbf," ");
		lp=strstr(tkn,tkbf);
	}
	fflg=lctr;
	if (lp) {
			switch(lp-tkn) {
			case 0:
				while (tm!=';' && tm!='{') {
					tm=gettk();
					strcpy(bfr,tkbf);
					while (isspace(tm))
						tm=fgetc();
					switch (tm) {
			case '[':
				tm=gettk();
				atoi(tkbf,&fflg);
				addsym(bfr,fflg);
				tm=fgetc();
				break;
			case '(':
				stri(7);
				stri(gsz);
				if (strstr("main",tkbf))
					strcpy(tkbf,"XMAIN");
				addsym(tkbf,1);
				procst(')');
				stkp=-(ccm+1);
				while (*p) {
					getsym();
					addsym(tkbf,-1);
					p++;
					stkp+=2;
				}
				stkp=0;
				tm=gettk();
				cbrk=100;
				break;
			case ',':
			case ';':
				addsym(tkbf,1);
				break;
					}				/* end whie */
				}					/* end case 0: */
				break;
			case 4:
				fflg=fflg+100;
			case 12:
				fnbrk();
				stri(5);
				*++fptr=fflg;
				stri(fflg);
				procst(0);
				stri(12);
				stri(tm=*fptr+2);
				*++fptr=cbrk;
				if (fflg<100)
					cbrk=tm;
				*++fptr=inproc;
				lctr+=3;
				tm=0;
				stri(99);
				break;
			case 7:
				tm=0;
				break;
			case 18:
				stri(23);
				stri(cbrk);
				break;
			case 24:
				procst(';');
				stri(23);
				stri(ectr);
				tm=1;
				break;
			case 31:
				fnbrk();
				procst(';');
				stri(5);
				stri(lctr++);
				*++fptr=lctr;
				procst(';');
				stri(12);
				stri(lctr+2);
				stri(23);
				stri(lctr+1);
				stri(5);
				stri(lctr++);
				procst(')');
				*++fptr=cbrk;
				*++fptr=inproc;
				stri(23);
				stri(lctr-2);
				stri(5);
				stri(lctr++);
				cbrk=lctr++;
				tm=0;
				break;
			default:
				dostt();
		} /* End switch */
	} else
		switch (tm) {
				case '{':
					tm=1;
					inproc++;
					break;
				case '}':
					break;
				case -1:
				case 0:
					stri(0);
#asm
					CALL 1,CHAIN
					ARG FNM
					HLT
FNM,				TEXT "CC2@@@"
#endasm
				case '/':
					while (fgetc()!='/');			/* Skip comment */
					tm=1;
					break;
				default:
					dostt();
	}
	return tm;
}



main()
{
	char trm;

	memset(ltbf,0,&ssz-ltbf);
	fopen("CC.C","r");
	strcpy(tkn,"int if else while break return for ");
	lctr = 10;
	ectr = 900;
	ltpt = ltbf;
	fptr = fstk;
	*fptr = -1;
	gadr = 128; /* Start of globals */
	iinit(asm);
	tm=gettk();
	while (1) {
		trm=next();
		tm=gettk();
		switch (trm) {
			case '{':
				inproc++;
				break;
			case '}':
				inproc--;
				if (!inproc) {
					stri(5);
					stri(ectr++);
					stri(16);
					stri(-stkp);
					stkp = *lm = 0;
					break;
				}
			case ';':
			case 1:
				stri(99);
				if (!strcmp("else",tkbf)) {
					stri(-23);
					stri(100+lctr+2);
					popfr();
					*++fptr=100+lctr++;
					*++fptr=cbrk;
					*++fptr=inproc;
				}
				else
					popfr();
			case 0:
				break;
			default:
				procst(';');
		}
	}
}

