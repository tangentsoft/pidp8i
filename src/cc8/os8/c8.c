#include <libc.h>
#include <init.pa>

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
