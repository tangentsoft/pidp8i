/*
 * This file is part of the CC8 cross-compiler.
 *
 * The CC8 cross-compiler is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The CC8 cross-compiler is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the CC8 cross-compiler as ../GPL3.txt.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

/*
 * Libc header
 *
 * Please note: no function declarations are made, so make sure the
 * arg lists are correct in the calling code.
 * 
 */

/*
 * WARNING: The following values are indices into the function call
 * table declared in libc.c.  (Search for "CLIST".)  These two lists
 * must match.
 *
 * The indices are in octal, and it is *critical* that they be be so,
 * because these values are treated as text by the compiler, not as C
 * integers.  The compiler emits these numeric strings in the generated
 * SABR code to call through the CLIST table, so since SABR runs in
 * octal mode under CC8, the following indices also have to be in octal.
 */
#define itoa libc0
#define	puts libc1
#define	dispxy libc2
#define	getc libc3
#define	gets libc4
#define	atoi libc5
#define	sscanf vlibc6
#define	xinit libc7
#define	memcpy libc10
#define	kbhit libc11
#define	putc libc12
#define	strcpy libc13
#define	strcat libc14
#define	strstr libc15
#define	exit libc16
#define	isnum libc17
#define	isalpha libc20
#define	sprintf vlibc21
#define	memset libc22
#define	fgetc libc23
#define	fopen libc24
#define fputc libc25
#define fclose libc26
#define printf vlibc27
#define isalnum libc30
#define isspace libc31
#define fprintf vlibc32
#define	fputs libc33
#define strcmp libc34
#define cupper libc35
#define fgets libc36
#define revcpy libc37
#define toupper libc40
#define strpd libc41

/* Declare function aliases. */ 
#define isdigit isnum
