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

/*
 * This file is the c pre processor of the native C compiler for the PDP-8 series of computers.
 * Linked with LIBC.RL to create CC0.SV
 * Hardware requirements:
 * 1. PDP/8 processor with minimal EAE (MQ register is heavily used).
 * 2. 20K (5x4K banks) of core.
 * 3. OS/8 operating system with FORTRAN II library (LIB8.RL)
 * 4.                            SABR assembler (SABR.SV)
 * 5.                            Linking loader (LOADER.SV)
 *
 * 1. The compiler consists of 3 files: CC0.SV, CC1.SV, CC2.SV on system device. (SYS:)
 * The runtime support files are:
 * 1. The c library created from libc.c and assembled to LIBC.RL on the user device.
 * 2. A runtime support file: HEADER.SB on the user device (DSK:)

 * These 3 .SV files run in sequence:
 * CC0: C pre-processor: Asks for ONE source file and creates CC.CC for CC1.SV.
 *      And, generates an intermediate file (CASM.TX) used by CC2.SV.
 * CC1: C tokeniser: Reads CC.CC and converts the code to a token list in FIELD 4
 * CC2: SABR code generator: Reads token list and generates CC.SB from
 *      a collection of code fragments. 
 * Finally, the SABR assembler is used on CC.SB and the runtime is generated
 * by LOADER.SV using CC.RL and LIBC.RL

 */

/* Ask for input file, copy to CC.CC and run CC1 */
/* Update Feb 2018 */
/* 1. Read file line by line */
/* 2. Exclude FF (12) */
/* 3. Implement simple #define directive **Warning** quoted text is not ignored */
/* 4. Implement #asm/#endasm directive */
/* Update April 2020 */
/* 5. Implement switch satement via re-write */
/* *** 1: default: must be included 2: Fall through is not implemented */
/*     3: Nesting is allowed */
/* 6: Implement logical operators !=,>=,<= via token symbols #,£,? */

int ln[80],*p,*q,*tm,*dfp,tkbf[10],smbf[10];
int dflst[1024],tmln[80];
int asm[1024];
int *swp,swpbf[256],*swc,xm;

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
    swc=swp=swpbf;
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

        p=ln;
        while (*p) {
            tm=p;
            switch (*p++) {
                case '>':
                    if (*p=='=') {
                        *tm=' ';
                        *p='?';
                    }
                    break;
                case '<':
                    if (*p=='=') {
                        *tm=' ';
                        *p='#';
                    }
                    break;
                case '!':
                    if (*p=='=') {
                        *tm=' ';
                        *p='£';
                    }
                    break;
            }
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

        p=strstr(ln,"switch");
            if (p) {
                p=p+6;
                while (*p!='{') {
                    if (!*p) 
                        fgets(ln);
                    *swp++=*p++;
                }
                *swp=0;
                xm=0;
                strcpy(ln,"while(1) {\r\n");
            }
		
        p=strstr(ln,"case ");
        if (p) {
            p=p+5;
            q=strstr(ln,":");
            *q=0;
            memcpy(tmln,p,q-p+1);
            if (xm)
                fputs("}\r\n");
            sprintf(ln,"if (%s== %s) {\r\n",swc,tmln);
            xm++;
        }

        p=strstr(ln,"default");
		if (p) {
            while (*p++!=':');
             if (xm)
                fputs("}");
            fputs(p);
            *ln=0;
		}

		
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
