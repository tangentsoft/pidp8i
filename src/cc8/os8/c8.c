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


main()
{
	int bfr;
	int fnm[10];

	putc('>');
	gets(fnm);
	cupper(fnm);
	fopen(fnm,"r");
	fopen("CC.CC","w");
	while (bfr=fgetc())
		if (bfr!=12)		/* Ignore form feed */
			fputc(bfr);
	fclose();
#asm	
	CALL 1,CHAIN
	ARG FNM
	HLT
FNM,	TEXT "CC1@@@"
#endasm


}
