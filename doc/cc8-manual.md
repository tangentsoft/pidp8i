A minimal Implementation of C for the Digital Equipment Corp. PDP/8 processor
Introduction:

	The C language and its derivatives are now the industry standard for 
the development of operating systems and utilities. The language has evolved 
significantly since its initial specification in 1972. At this time, the PDP/7 
was used for the initial implementation and the compiler ported to an number of 
other systems including the PDP/11. Also, the first glimmerings of Unix 
appeared following a re-write of the assembly language version in C and the 
rest is of course history. The PDP/8 was introduced by DEC in 1965 at the same 
time as the PDP/7 with the intention of being a small and cheap processor that 
could be used in a variety of environments. From this simple machine, the 
modern desktop device has evolved which I am using to type this document. 
Nonetheless, far from fading into obscurity, there is a very active group of 
enthusiasts who have looked to implementing the PDP/8 on modern hardware and 
the thanks to Oscar Vermuelen and others, we can all have a PDP8/I to play 
with. With this in mind, I thought it was time to have a modern language 
compiler running on the PDP/8 which as far as I can tell, the last native 
compiler developed for the PDP/8 was Pascal in 1979 by Heinz Stegbauer.  In 
more recent times, one hosted compiler has been developed by Vince Slyngstad 
and updated by Paolo Maffei based on Ron Cain’s Small C using a VM approach. 
This code is most certainly worth examining and I am delighted to acknowledge 
this work as I have used some of the c library code in this project. 
See: http://svn.so-much-stuff.com/svn/trunk/pdp8/small-c/
Finally, I would refer the reader to Fabrice Bellard’s OTCC. It is this bit of 
remarkable software that suggested that there may be a chance to implement a 
native compiler.
	Developing a native compiler for the PDP/8 is not an easy task as this 
processor has a very limited address space and no hardware stack. And, although 
the option exists to write the whole thing in assembly language as has been the 
case for Pascal and Algol, this project has explored the option of writing the 
compiler itself in C. To this end, 2 compilers have been written. Firstly, a 
hosted cross compiler based again on Ron Cain’s Small C which is used to 
compile the native compiler and library. As yet, the native compiler has rather 
limited functionality and will not compile itself. The hosted compiler will 
compile itself but produces an enormous (28K) assembler file which cannot be 
run on the PDP/8.

The hosted compiler:

	The code for this is in the SMC subdirectory of the project file and 
may be built under Unix with the supplied Makefile. As above, this is based 
upon Ron Cain’s Small C compiler and the reader is directed to the extensive 
documentation available on the web. The key file is the code generator section 
in code8.c. This compiler may be used to compile quite complex c programs.
It is automatically installed by the PiDP8I build system.
It will generate a SABR (Symbolic Assembler for Binary Relocatable programmes) 
which is normally used as the second pass of the OS/8 FORTRAN II system. This 
used to compile the 3 components of the native C compiler
1. C8.c ->C8.SB		The pre-processor (CPP).
2. N8.c->N8.SB		The parser/tokeniser section of the compiler
3. P8.c -> P8.SB		The token to SABR code converter section.
4. Libc.c -> LIBC.SB		The C library used by both of the above via the 
libc.h include file.
   The *.SB files then need to be transferred to a OS/8 system running under 
(for example) simh preferably on your PiDP8/I. See below for details.
   To use the hosted compiler, use the command:
   cc8 myfile.c
   This will create the file myfile.sb. This file will then need to be copied 
to the OS/8 system ... usually into DSK:. 
   Please see the examples in the build system under src/src/cc8/examples.
Then, the *.SB files need to compiled under OS/8:
In the case: .COMP MYFILE.SB
This will generate a file MYFILE.RL
To run this:
.R LOADER
*MYFILE,LIBC/I/O/G
The OS/8 command lines to build the native compiler are:
1. .COMP N8.SB
2. .COMP LIBC.SB
3. .COMP P8.SB
4. .COMP C8.SB
This will create the *.RL files for the linking loader (LOADER.SV).
   The hosted compiler has some non-standard features to enable the interface 
between the main programme and the c library. This constitutes a compile time 
linkage system to allow for standard and vararg functions to be called in the 
library. The 4 SABR files generated from the source files as above may then be 
separately downloaded, compiled, loaded and linked under OS/8.  Each of these 
SABR files generates just less than 4K of relocatable code as C8.RL,N8.RL,P8.RL 
and LIBC.RL. These are linked in pairs under OS/8 to create the 3 native 
compiler phases.
Phase 1: Link C8.RL and LIBC.RL to be save as CC.SV
Phase 2: Link N8.RL and LIBC.RL to be saved as CC1.SV
Phase 3: Link P8.RL and LIBC.RL to be saved as CC2.SV
   These files are automatically built by the PiDP8I build system and copied to 
the SYS: segment of the system drive.
C8 (CC.SV)  terminates by chaining to N8 (CC1.SV)  which chains to CC2.SV to 
complete the process of generating a final SABR file. (CC.SB).
	The C8,N8 and P8 source files also reference a runtime header as an 
include file “HEADER.SB”. This is the start-up code for each phase and also 
calls the initialisation code in LIBC.
	The linking loader determines the core layout of each of the pairs of 
.RL files as above. Typically this is as follows:
Field 0:		FOTRAN library utility functions and OS/8 I/O system
Field 1:		Reserved for the programme’s runtime 
stack/globals/literals.
Field 2:		Usually the primary programme ... either C8, N8 or P8.
Field 3:		Usually the LIBC library.
Filed 4:		Used to hold the tokenised programme as a binary 
stream. These functions are
		available to the user as detailed below. (STRI....)
	In all, each phase of the native compiler will use 20K of core.

The native compiler:

	This compiler is built and linked as above. The final three files 
generated are CC.SV, CC1.SV and CC2.SV. These should be on the OS/8 system 
device (SYS:). In addition, you will need to file HEADER.SB on the default user 
device. This is used by CC2.SV. I suggest you use the provided RK05 image as 
this has the SYS: and DSK: partitions configured as required and include a 
linked copy of the compiler and some example programs.
To try it out:
Boot OS/8 from the RK05 image as above (preferably on your PiDP8, this app 
really flashes the lights!). Check the bootscript in 7.script to enable lower 
case in Simh. Then, you can enter a c programme in lower case via the editor. 
The run it as below . is the monitor .!
.EXE CCR.BI (CCR.BI is a batch file to run the compile and go process).
$JOB COMPILE AND RUN C PROGRAMME
>ps.c
And see what happens....
See below for an explanation as to how the system works.
This is the original help document and the system has now been updated.
See the addenda for details. Specifically, the update 2020 section.

GOVERNMENT HEALTH WARNING

       You are hereby warned: the native compiler does not contain any error 
checking whatsoever. If the source files contain an error, you will either 
receive a compile time warning while compiling the CC.SB file or a runtime 
crash. Any error will not give you any hint as to the cause. It is worth noting 
that GCC uses some 100’s of K for error checking. The app has only 3K to create 
a token list from CC.CC that could run directly in a suitable VM. If your 
programme does not work, check it carefully or test it with gcc in the sure 
knowledge that you would need a roomful of magnetic core memory to do this!

	The native compiler implements a very limited version of c which is 
nonetheless, Turing complete. You could see it more as a simple scripting 
system. It is actually typeless in that everything is 12 bit int and any 
variable/array can interpreted as int, char or pointer. However, all variables, 
functions and arrays must be declared int.
1. There must be an int main() which appears as the last function.
2. Arrays and variables may be local or global (implied static).
3. Arrays may only be single indexed. See PS.CC for an example.
4. Do not attempt to scope variables within a function. Eg for (int i=...
5. The only allowed digraphs are ++,-- (postfix only) and ==.
6. Comparison operators are compare signed. Result is boolean true=-1.
7. &, | and ! are bitwise.
8. See libc.h for allowed libc functions. There are 31.
a. Atoi is non-standard viz int atoi(char *,int *) return length of numeric 
string.
9. Fopen is implemented as void fopen(char *filename, char *mode). Filename 
must be upper case. Mode is either “w” or “r”.
a. Only 1 input file and 1 output may be open at any one time
b. fclose() only closes the output file.
c. Call fopen to open a new input file, current file does not need to be closed.
d. Fprintf,fputc,fputs are as expected.
e. Fgets is implemented. Will read to and retain CR/LF. Null string on EOF.
f. Fscanf is not implemented. Read a line (fgets()) and then sscanf.
g. Feof is not implemented, fgetc/fgets will return a null.
10. Scanf is not implemented use gets then sscanf
11. Printf is as expected. See libc.c for the allowed format chars %d,%s etc.
a. Length or width.precision is supported.
12. Pointers are as expected. Do not try *(<expr>). This does not work.
13. Structs etc. Are not supported.
14. Double precision int, float etc are not supported. If you need to do heavy 
duty maths, use Fortran.
15. The stack which includes all globals and literals and is only 4k. Stack 
overflow is not detected.
16. Recursion is implemented.. See FIB.CC.
17. Literals have to be included in the 4K limit programme area. These are 
copied into the stack at run time. This is due to the fact that ‘COMMON’ 
storage cannot be initialised.
18. There is no option for #include files. The available Libc functions are 
implicitly declared and listed in libc.h and p8.c. As a result, there is no 
arglist checking. Examine libc.c for details.

OS/8 specifics:
1. I strongly suggest you limit I/O to text files.
2. Don’t forget to handle form feed ... see c8.c
3. For some obscure reason, always open the input file first, then the output 
file. I suspect a fault in libc.c. Examine the code!!!! (Every trick in the 
book).

Conclusion:

	This is a somewhat limited manual which attempts to give an outline of 
a very simple compiler for which I apologise as the source code is obscure and 
badly commented. However, the native compiler/tokeniser (n8.c) is only 600 
lines which is a nothing in the scale of things these days. However, I hope 
this project gives some insight into compiler design and code generation 
strategies to target a most remarkable computer. I would also like to give 
credit to the builders of OS/8 and in particular the FORTRAN II system which 
was never designed to survive the onslaught of this kind of modern software.
	Don’t expect too much! This compiler will not build this week’s 
bleeding edge kernel. But, it may be used to build any number of useful utility 
programs for OS/8.

Ian Schofield May 2017
Isysxp@gmail.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

Update April 2020

	The native compiler has now been updated:
1. The digraphs !=,>=,<= have been implemented in addition to (postfix) ++,-- 
and ==.
2. The switch statement has been implemented using an if/then re-write process.
Switch statements may be nested.
NB: The argument statement to switch is executed one or more times. Do not use:
        e.g. switch(*p++) as p may be incremented more than once.
NB: There must be a default: condition terminated with a break statement.
        Switch statement do not ‘fall through’ the terminating  }.
NB: Case statements do not ‘fall through’. I.e each case statement is evaluated
separately. 
3. The || and && operators are converted to | and & respectively. Please note
modified operator precedence in this case.  E.g. if(j > 2 || i < 2) is executed 
as 
if ((j > 2) | (i<2)) ... as would be expected using ||. 
That is to say that the precedence of | and & is the same as || and &&.
This is different to the normal precedence of | and &.
E.g., the statement if (j > 2 | 3) is executed as if ((j > 2) | 3). 
A warning is printed if these operators are converted.
4. The ternary operator ?: is now implemented. Again, this may be nested.
5. At present, the size of global declarations is limited such that the offset
of the final variable must be less than 2048. This is not too much of a problem
as the total for all storage including the stack at runtime must be less than
3840 words.
6. Some utility functions have been added to header.sb. This file contains the
initialisation code for the c library (libc.c) and initialises a set of page 
zero 
pointers for the commonly used functions in header.sb: PUSH, POP, PUTSTK,
POPRET, PCALL. In addition there are a set of functions that may be used to
provide temporary storage in field 4. These functions act like a temporary 
binary file:

void iinit(int address): Reset the file pointer to an arbitrary address range 
0-4095.
void stri(int value): Store ‘value’ at the current address and increment the 
address
pointer.
int strl(): Read the word at the current address and do not increment the 
address.
int strd(): Read the word at the current address and increment the address.
As field 4 is not used by OS/8, the whole field may be used and the address 
pointer
will merely wrap from  4095 to 0.

7. The commonest and most basic error namely mis-matched {} is now trapped.
8. Multiline comments are now permitted.
9. You may ignore the addendum above. The entire compiler/runtime is now built
and installed via scripts in the current build for the PiDP8I at:
https://tangentsoft.com/pidp8i/wiki?name=Home
Many thanks to Warren Young for creating and managing this repository.


Limitations/reminders:

1. #define can only replace an alphanumeric symbol with another
alphanumeric symbol. Macros are not implemented.
2. Variable names must be alphanumeric only. Do not use _ etc.
In addition, limit variable names to 6 characters.
3. Check libc.h for valid library functions. (NB atoi is non-standard).
There are now 40.
4. *<array>[<index>] does not work. May be fixed.
5. <array>[<index>]++/-- does not work. May be fixed.
6. *(<statement>) does not work. May be fixed.
7. Xor (^) is not implemented.
8. The syntax for function declaration is oldstyle c.
E.g.

<type> func(arg)
int arg;
{
  <body>
 /optional/ return <statement>
}
the <type> is mandatory for all functions.
Type can by int, char or void. (All are converted to int as there only 
12 bits to play with. Use char/void for readability purposes.
9. Double precision is not supported. If you need to, see dp.c
in examples.
10. Int main() must be declared last. Do not include a return statement
use exit(int) instead.
11. Prefix ++/-- are not implemented. 
12. Please limit line lengths to <80 characters.
13. ^C will exit the programme if the programme is waiting for input.
14. ^C will not kill a failed programme. There is no runtime manager
like FRTS (FortIV) or BRTS(Basic) which can hook the terminal. 
Solution: restart OS/8.
15. You can expect to obtain a runnable programme up to a size of
about 300 lines. If you overrun available core, SABR will return the 
‘S’ error.  See Forth.c in examples.
Ian Schofield
May 2020

