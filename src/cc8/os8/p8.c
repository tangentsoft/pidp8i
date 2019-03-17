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
				fputs("/\r\n");
				break; 
			case 1:
				fputs("\tJMSI PTSK\r\n");
				break;
			case 3:
				strcpy(tmstr,"\tCIA\r\n");
			case 2:
				fprintf("%s\tTADI STKP\r\n\tJMSI POP\r\n",tmstr);
				break;
			case 4:
				fprintf("\tCLA\r\n\tTAD (%o\r\n",strd());
				break;
			case 5:
				tm=strd();
				if (tm<0)
					tm=200-tm;
				fprintf("CC%o,\r\n",tm);
				break;
			case 6:
				if (strl()>1)
					fprintf("\tTAD STKP\r\n\tTAD (%o\r\n\tDCA STKP\r\n",strl());
				else
					if (strl()>0)
						fputs("\tISZ STKP\r\n");
				strd();
				break;
			case 7:
				p=gm+strd();
				while (*p-' ')
					fputc(*p++);
				fputs(",\r\n");
				break;
			case -8:
				strcpy(tmstr,"\tDCA JLC\r\n\tTADI JLC\r\n");
			case 8:
				if (strl()>0)
					fprintf("\tCLA\r\n\tTAD (%o\r\n%s\tJMSI PSH\r\n",strd(),tmstr);
				else
					fprintf("\tCLA\r\n\tTAD STKP\r\n\tTAD (%o\r\n%s\tJMSI PSH\r\n",strd(),tmstr);
				break;
			case 9:
				tm=strd();
				p=gm+strd();
				strcpy(tkbf,"        ");
				memcpy(tkbf,p,7);
				if (p=strstr(xlt,tkbf)) {
					t=(p-xlt)>>3;
					if ((t==6) + (t==17) + (t==23)) 
						fprintf("\tCLA\r\n\tTAD (%o\r\n\tJMSI PSH\r\n",tm++);
					fprintf("\tCLA\r\n\tTAD (%o\r\n\tMQL\r\n\tCALL 1,LIBC\r\n\tARG STKP\r\n\tCDF1\r\n",t);
				}
				else
					fprintf("\tCPAGE 2\r\n\tJMSI PCAL\r\n\t%s\r\n",tkbf);
				if (tm)
				    fprintf("\tMQL\r\n\tTAD (%o\r\n\tTAD STKP\r\n\tDCA STKP\r\n\tSWP\r\n",-tm);
				break;
			case 10:
				fprintf("\tCLA\r\n\tTAD GBL\r\n\tTAD (%o\r\n",strd());
				break;
			case -11:
				fputs("\tCIA\r\n\tTADI STKP\r\n\tJMSI POP\r\n\tSMA SZA CLA\r\n\tCMA\r\n");
				break;
			case 11:
				fputs("\tCIA\r\n\tTADI STKP\r\n\tJMSI POP\r\n\tSPA CLA\r\n\tCMA\r\n");
				break;
			case 12:
					fprintf("\tSNA\r\n\tJMP CC%o\r\n",strd());
				break;
			case 13:
				fputs("\tJMSI POP\r\n\tDCA JLC\r\n\tSWP\r\n\tCALL 1,MPY\r\n\tARG JLC\r\n\tCDF1\r\n");
				break;
			case -14:
				fputs("\tCALL 1,IREM\r\n\tARG 0\r\n\tCDF1\r\n");
				break;
			case 14:
				fputs("\tJMSI POP\r\n\tDCA JLC\r\n\tSWP\r\n\tCALL 1,DIV\r\n\tARG JLC\r\n\tCDF1\r\n");
				break;
			case 15:
				fputs("\tISZI JLC\r\n\tNOP\r\n");
				break;
			case 16:
				fprintf("\tMQL\r\n\tTAD STKP\r\n\tTAD (%o\r\n\tDCA STKP\r\n\tSWP\r\n\tJMPI POPR\r\n/\r\n",strd());
				break;
			case 17:
				pflg++;
			case -17:
				if (strl()>0) 
					fprintf("\tCLA\r\n\tTAD (%o\r\n\tDCA JLC\r\n\tTADI JLC\r\n",strd());
				else
					fprintf("\tCLA\r\n\tTAD STKP\r\n\tTAD (%o\r\n\tDCA JLC\r\n\tTADI JLC\r\n",strd());
				if (pflg==0)
					break;
			case 19:
				fputs("\tJMSI PSH\r\n");
				break;
			case 20:
				fputs("\tANDI STKP\r\n\tJMSI POP\r\n");
				break;
			case -20:
				fputs("\tJMSI POP\r\n\tMQA\r\n");
				break;
			case 21:
				if (strl()>0) 
					fprintf("\tCLA\r\n\tTAD (%o\r\n",strd());
				else
					fprintf("\tCLA\r\n\tTAD STKP\r\n\tTAD (%o\r\n",strd());
				break;
			case 22:
				fputs("\tDCA JLC\r\n\tTADI JLC\r\n");
				break;
			case 23:
				if (strl()<200)
					fprintf("\tJMP CC%o\r\n",strl());
				strd();
				break;
			case -23:
					fprintf("\tJMP CC%o\r\n",strd());
				break;
			case 24:
				fputs("\tCIA\r\n\tTADI STKP\r\n\tJMSI POP\r\n\tSNA CLA\r\n\tCMA\r\n");
				break;
			case 25:
				fputs("\tMQL\r\n\tCMA\r\n\tTADI JLC\r\n\tDCAI JLC\r\n\tSWP\r\n");
				break;
			case 26:
				fputs("\tSNA CLA\r\n");
			case -26:
				fputs("\tCMA\r\n");
				break;
			case 27:
				fputs("\tCIA\r\n");
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
	fprintf("\tLAP\r\n\tCPAGE %o\r\nLCC0,\t%o\r\nXCC0,\tCC0\r\nCC0,\t\r\n",ltsz+2,-ltsz);
	p=ltbf;
	while (ltsz) {
		fprintf("%o",*p++);
		if (ltsz>1)
			fputs("; ");
		if ((ltsz&7)==0) 
			fputc(13);
		ltsz--;
	}
	fprintf("\r\n\tEAP\r\nGBLS,\t%o\r\n",gadr);
	fputs("\r\nMCC0,\t0\r\n\tCDF1\r\n\tTAD LCC0\r\n\tSNA CLA\r\n\tJMP I MCC0\r\n\tTAD XCC0\r\n\tDCA JLC\r\nDCC0,\tCDF0\r\n\tTADI JLC\r\n");
	fputs("\tJMSI PSH\r\n\tCLA\r\n\tISZ JLC\r\n\tISZ LCC0\r\n\tJMP DCC0\r\n\tJMP I MCC0\r\nCCEND,\t0\r\n\t\END\r\n");

	fclose();

}
