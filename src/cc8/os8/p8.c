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
#define CMAX 280
#define BMAX 64
#define LMAX 32
#define DMAX 32
#define CBMX 1024
#define LXMX 999

int ltbf[512];
int xlt[CMAX];
int gm[512];		/* Global symbol table */
int tkbf[LMAX];
int *p,*q,*s,*ltpt;
int gsym,lsym,gadr,ladr,stkp,lctr,*fptr,gsz,ctr,tm,ectr;
int glim;
int ltsz,pflg,t;
int tmstr[32];

main()
{

	iinit(128);
	fopen("CASM.TX","r");
	fopen("CC.SB","w");
	while (1) {
		t=fgetc();
		if (t=='!')
			break;
		fputc(t);
	}

	strcpy(xlt,"ITOA    PUTS    DISPXY  GETC    GETS    ATOI    SSCANF  XINIT   MEMCPY  KBHIT   PUTC    STRCPY  STRCAT  STRSTR  EXIT    ISNUM   ");
	strcat(xlt,"ISALPHA SPRINTF MEMSET  FGETC   FOPEN   FPUTC   FCLOSE  PRINTF  ISALNUM ISSPACE FPRINTF FPUTS   STRCMP  CUPPER  FGETS   REVCPY  ");
	strcat(xlt,"TOUPPER ");
	cupper(gm);
	while (strl()) {
		pflg=0;
		*tmstr=0;
		switch (strd()) {
			case 99:
				fprintf("/\r");
				break; 
			case 1:
				fprintf("\tJMSI PTSK\r");
				break;
			case 3:
				strcpy(tmstr,"\tCIA\r");
			case 2:
				fprintf("%s\tTADI STKP\r\tJMSI POP\r",tmstr);
				break;
			case 4:
				fprintf("\tCLA\r\tTAD (%d\r",strd());
				break;
			case 5:
				tm=strd();
				if (tm<0)
					tm=200-tm;
				fprintf("CC%d,\r",tm);
				break;
			case 6:
				if (strl()>1)
					fprintf("\tTAD STKP\r\tTAD (%d\r\tDCA STKP\r",strl());
				else
					if (strl()>0)
						fputs("\tISZ STKP\r");
				strd();
				break;
			case 7:
				p=gm+strd();
				while (*p-' ')
					fputc(*p++);
				fputs(",\r");
				break;
			case -8:
				strcpy(tmstr,"\tDCA JLC\r\tTADI JLC\r");
			case 8:
				if (strl()>0)
					fprintf("\tCLA\r\tTAD (%d\r%s\tJMSI PSH\r",strd(),tmstr);
				else
					fprintf("\tCLA\r\tTAD STKP\r\tTAD (%d\r%s\tJMSI PSH\r",strd(),tmstr);
				break;
			case 9:
				tm=strd();
				p=gm+strd();
				strcpy(tkbf,"        ");
				memcpy(tkbf,p,7);
				if (p=strstr(xlt,tkbf)) {
					t=(p-xlt)>>3;
					if ((t==6) + (t==17) + (t==23)) 
						fprintf("\tCLA\r\tTAD (%d\r\tJMSI PSH\r",tm++);
					fprintf("\tCLA\r\tTAD (%d\r\tMQL\r\tCALL 1,LIBC\r\tARG STKP\r\tCDF1\r",t);
				}
				else
					fprintf("\tCPAGE 2\r\tJMSI PCAL\r\t%s\r",tkbf);
				if (tm)
				    fprintf("\tMQL\r\tTAD (%d\r\tTAD STKP\r\tDCA STKP\r\tSWP\r",-tm);
				break;
			case 10:
				fprintf("\tCLA\r\tTAD GBL\r\tTAD (%d\r",strd());
				break;
			case -11:
				fprintf("\tCIA\r\tTADI STKP\r\tJMSI POP\r\tSMA SZA CLA\r\tCMA\r");
				break;
			case 11:
				fprintf("\tCIA\r\tTADI STKP\r\tJMSI POP\r\tSPA CLA\r\tCMA\r");
				break;
			case 12:
					fprintf("\tSNA\r\tJMP CC%d\r",strd());
				break;
			case 13:
				fprintf("\tJMSI POP\r\tDCA JLC\r\tSWP\r\tCALL 1,MPY\r\tARG JLC\r\tCDF1\r");
				break;
			case -14:
				fprintf("\tCALL 1,IREM\r\tARG 0\r\tCDF1\r");
				break;
			case 14:
				fprintf("\tJMSI POP\r\tDCA JLC\r\tSWP\r\tCALL 1,DIV\r\tARG JLC\r\tCDF1\r");
				break;
			case 15:
				fprintf("\tISZI JLC\r\tNOP\r");
				break;
			case 16:
				fprintf("\tMQL\r\tTAD STKP\r\tTAD (%d\r\tDCA STKP\r\tSWP\r\tJMPI POPR\r/\r",strd());
				break;
			case 17:
				pflg++;
			case -17:
				if (strl()>0) 
					fprintf("\tCLA\r\tTAD (%d\r\tDCA JLC\r\tTADI JLC\r",strd());
				else
					fprintf("\tCLA\r\tTAD STKP\r\tTAD (%d\r\tDCA JLC\r\tTADI JLC\r",strd());
				if (pflg==0)
					break;
			case 19:
				fprintf("\tJMSI PSH\r");
				break;
			case 20:
				fprintf("\tANDI STKP\r\tJMSI POP\r");
				break;
			case -20:
				fprintf("\tJMSI POP\r\tMQO\r");
				break;
			case 21:
				if (strl()>0) 
					fprintf("\tCLA\r\tTAD (%d\r",strd());
				else
					fprintf("\tCLA\r\tTAD STKP\r\tTAD (%d\r",strd());
				break;
			case 22:
				fprintf("\tDCA JLC\r\tTADI JLC\r");
				break;
			case 23:
				if (strl()<200)
					fprintf("\tJMP CC%d\r",strl());
				strd();
				break;
			case -23:
					fprintf("\tJMP CC%d\r",strd());
				break;
			case 24:
				fprintf("\tCIA\r\tTADI STKP\r\tJMSI POP\r\tSNA CLA\r\tCMA\r");
				break;
			case 25:
				fprintf("\tMQL\r\tCMA\r\tTADI JLC\r\tDCAI JLC\r\tSWP\r");
				break;
			case 26:
				fprintf("\tSNA CLA\r");
			case -26:
				fprintf("\tCMA\r");
				break;
			case 27:
				fputs("\tCIA\r");
				break;
			case 29:
				while (1) {
					t=fgetc();
					if (t=='$')
						break;
					fputc(t);
				}
		}
	}
	ltsz=ltpt-ltbf;
	fprintf("\tLAP\r\tCPAGE %d\rLCC0,\t%d\rXCC0,\tCC0\rCC0,\t\r",ltsz+2,-ltsz);
	p=ltbf;
	while (ltsz) {
		fprintf("%d",*p++);
		if (ltsz>1)
			fputs("; ");
		if ((ltsz&7)==0) 
			fputc(13);
		ltsz--;
	}
	fprintf("\r\tEAP\rGBLS,\t%d\r",gadr);
	fprintf("\rMCC0,\t0\r\tCDF1\r\tTAD LCC0\r\tSNA CLA\r\tJMP I MCC0\r\tTAD XCC0\r\tDCA JLC\rDCC0,\tCDF0\r\tTADI JLC\r");
	fprintf("\tJMSI PSH\r\tCLA\r\tISZ JLC\r\tISZ LCC0\r\tJMP DCC0\r\tJMP I MCC0\rCCEND,\t0\r\t\END\r");

	fclose();
}
