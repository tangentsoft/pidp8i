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

/* C pre-processor stub for PDP/8 c compiler 2017 */
/* Ask for input file, copy to CC.CC and run CC1 */
/* Update Feb 2018: */
/* 1. Read file line by line */
/* 2. Exclude FF (12) */
/* 3. Implement simple #define directive **Warning** quoted text is not ignored */
/* 4. Implement #asm/#endasm directive */

int ln[80],*p,*q,*tm,*dfp,tkbf[10],smbf[10];
int dflst[1024],tmln[80];
int asm[1024];


skpsp()
{
	while (isspace(*p))
		p++;
}

strlen(p)
char *p;
{
	int n;

	n=0;
	while (*p++)
		n++;
	return n;
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


parse()
{
	getsym();
	strcpy(dfp,tkbf);
	getsym();
	strcpy(dfp+512,tkbf);
	dfp+=10;
}

dorep()
{
	p=dflst;
	while (*p) {
		q=strstr(ln,p);
		if (q) {
			memset(tmln,0,80);
			if (q-p)
				memcpy(tmln,ln,q-ln);
			strcat(tmln,p+512);
			strcat(tmln,q+strlen(p));
			memcpy(ln,tmln,80);
		}
		p+=10;
	}
}


main()
{
	int bfr;
	int fnm[10];

	putc('>');
	gets(fnm);
	cupper(fnm);
	fopen(fnm,"r");
	fopen("CC.CC","w");
	*asm=0;
	memset(dflst,0,1024);
	dfp=dflst;
	while (1) {
		fgets(p=ln);
		if (!*ln)
			break;

		dorep();

		while (*p) {
			if (*p==12)
				*p=' ';
			p++;
		}

		p=strstr(ln,"#asm");
		q=0;
		while (p) {
			fgets(ln);
			q=strstr(ln,"#endasm");
			if (q) {
				strcpy(ln,"`\r\n");
				break;
			}
			toupper(ln);
			strcat(asm,ln);
		}
		if (p)
			strcat(asm,"$");
		
		p=strstr(ln,"#define ");
		if (p) {
			p=p+8;
			parse();
		} else
			fputs(ln);
	}
	fclose();
	fopen("HEADER.SB","r");
	fopen("CASM.TX","w");
	while (bfr=fgetc())
		fputc(bfr);
	fputc('!');
	p=asm;
		while (*p)
			fputc(*p++);
	fclose();

#asm	
	CALL 1,CHAIN
	ARG FNM
	HLT
FNM,	TEXT "CC1@@@"
#endasm


}
