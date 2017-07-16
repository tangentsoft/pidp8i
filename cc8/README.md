# A Minimal Implementation of C for the DEC PDP-8 Processor

## Introduction

The C language and its derivatives are now the industry standard for the
development of operating systems and utilities. The language has evolved
significantly since its initial specification in 1972. At this time, the
PDP-7 was used for the initial implementation and the compiler ported to
a number of other systems including the PDP-11. Also, the first
glimmerings of Unix appeared following a re-write of the assembly
language version in C and the rest is of course history. The PDP-8 was
introduced by DEC in 1965 at the same time as the PDP-7 with the
intention of being a small and cheap processor that could be used in a
variety of environments. From this simple machine, the modern desktop
device has evolved which I am using to type this document. Nonetheless,
far from fading into obscurity, there is a very active group of
enthusiasts who have looked to implementing the PDP-8 on modern hardware
and the thanks to Oscar Vermuelen and others, we can all have a PDP8/I
to play with. With this in mind, I thought it was time to have a modern
language compiler running on the PDP-8 which as far as I can tell, the
last native compiler developed for the PDP-8 was Pascal in 1979 by Heinz
Stegbauer. In more recent times, one cross-compiler has been developed
by Vince Slyngstad and updated by Paolo Maffei based on Ron Cain’s Small
C using a VM approach. [This code][sms] is most certainly worth
examining, and I am delighted to acknowledge this work as I have used
some of the C library code in this project.

[sms]: http://so-much-stuff.com/pdp8/C/C.php

Finally, I would refer the reader to Fabrice Bellard’s OTCC. It is this
bit of remarkable software that suggested that there may be a chance to
implement a native PDP-8 compiler.

Developing a native compiler for the PDP-8 is not an easy task as this
processor has a very limited address space and no hardware stack. And,
although the option exists to write the whole thing in assembly language
as has been the case for Pascal and Algol, this project has explored the
option of writing the compiler itself in C. To this end, 2 compilers
have been written. Firstly, a cross-compiler based again on Ron Cain’s
Small-C which is used to compile the native OS/8 compiler and library.
As yet, the native compiler has rather limited functionality and will
not compile itself. The cross-compiler will compile itself but produces
an enormous (28K) assembler file which cannot be run on the PDP-8.


# The Cross-Compiler

The code for this is in the `cross` subdirectory, and is built along
with the top-level PiDP-8/I software. As above, this is based upon Ron
Cain’s Small-C compiler, and the reader is directed to the extensive
documentation available on the web. The key file is the code generator
section in `code8.c`. This is used to generate SABR (Symbolic Assembler
for Binary Relocatable programmes) assembly output which is normally
used as the second pass of the OS/8 FORTRAN II system.

You use this cross-compiler to compile the 3 components of the OS/8 C
compiler:

1.  `n8.c` &rarr; `n8.s`: The parser/tokeniser section of the compiler

2.  `p8.c` &rarr; `p8.s`: The token to SABR code converter section.

3.  `libc.c` &rarr; `libc.s`: The C library used by both of the above
    via the `libc.h` include file.

When you build and run this cross-compiler on a POSIX type system such
as the Raspbian PiDP-8/I environment, the resulting `*.s` files will
have LF-only line endings. You may need to run these files through a
`unix2dos` type utility in order to produce the CRLF line endings that
OS/8's SABR assembler expects, depending on how you get those `*.s`
files into OS/8 as `*.SB`.

The `*.SB` files may be assembled under OS/8:

    .COMP N8.SB
    .COMP LIBC.SB
    .COMP P8.SB

This will create the `*.RL` files for the linking loader (`LOADER.SV`).

The cross-compiler has some non-standard features to enable the
interface between the main programme and the C library. This constitutes
a compile time linkage system to allow for standard and vararg functions
to be called in the library. The 3 SABR files generated from the source
files as above may then be separately downloaded, compiled, loaded and
linked under OS/8. Each of these SABR files generates just less than 4K
of relocatable code as `N8.RL`, `P8.RL` and `LIBC.RL`. These are linked
in pairs under OS/8 to create the 2 native OS/8 compiler phases:

Phase 1: Link `N8.RL` and `LIBC.RL` to be saved as `CC1.SV`

Phase 2: Link `P8.RL` and `LIBC.RL` to be saved as `CC2.SV`

The commands are as follows, with `$` being an Escape keypress:

    .R LOADER
    *N8,LIBC/I$
    .SAVE SYS CC1
    .R LOADER
    P8,LIBC/I/O$
    .SAVE SYS CC2

N8 (`CC1.SV`) terminates by chaining to `CC2.SV` to complete the process
of generating a final SABR file.

Several of the C programs in this distribution reference a PAL assembly
initialization routine `cc8/include/init.pa`, which is symlinked into
each directory that uses it. It defines some low-level subroutines,
initializes the environment for the programs, and calls into the LIBC
initialization code. If you are going to compile these programs with the
OS/8 native compiler, you will need to copy `init.pa` into the OS/8
environment as well.

The same goes for `cc8/include/libc.h`, which defines the mappings
between the familiar C library routine names and their underlying
implementation names.

The linking loader determines the core layout of each of the pairs of
`.RL` files as above. Typically this is as follows:

**Field 0:** FOTRAN library utility functions and OS/8 I/O system

**Field 1:** Reserved for the programme’s runtime stack/globals/literals.

**Field 2:** Usually the primary programme ... either N8 or P8.

**Field 3:** Usually the LIBC library.

In all, each phase of the native OS/8 compiler will use 16K of core.


## The Native Compiler

This compiler is built and linked as above. The final two files
generated are `CC1.SV` and `CC2.SV`. These should be on the OS/8 system
device. (`SYS:`) The compiler expects the source file in C to be in `CC.CC`
on the default user device. (`DSK:`) In addition, you will need to file
`HEADER.SB` on the default user device. This is used by `CC2.SV`. I suggest
you use the provided RK05 image as this has the `SYS:` and `DSK:` partitions
configured as required and include a linked copy of the compiler and
some example programs.

To try it out:

Boot OS/8 from the included RK05 image with:

     $ bin/pidp8i-sim boot/cc8.script

Note that this is a different version of OS/8 than the one currently
distributed in the IF=0, IF=3, and IF=7 positions in the PiDP-8/I boot
scheme. It is version V3Q (up from V3D) and it is configured to smash
lowercase input to uppercase only in the OS/8 command processor.
Lowercase input in the text editor will be saved as lowercase.

With this special CC8 OS/8 environment running, you can enter a C
programme in lower case via the editor.

Before getting into the text editor, try building a copy of one of the
example programs:

    .COPY CC.CC<PS.CC

Run with:

    .R CC1

Then:

    .COMP CC.SB

Then:

    .R LOADER
    *CC,LIBC/G

And hope for the best!!!!


## GOVERNMENT HEALTH WARNING

**You are hereby warned**: the native OS/8 compiler does not contain any
error checking whatsoever. If the source files contain an error, you
will either receive a compile time warning while compiling the CC.SB
file or a runtime crash. Any error will not give you any hint as to the
cause. It is worth noting that GCC uses some 100’s of K for error
checking. The app has only 3K to create a token list from CC.CC that
could run directly in a suitable VM. If your programme does not work,
check it carefully or test it with gcc in the sure knowledge that you
would need a roomful of magnetic core memory to do this!

The native OS/8 compiler implements a very limited version of C which is
nonetheless Turing complete. You could see it more as a simple scripting
system. It is actually typeless in that everything is a 12 bit integer
and any variable/array can interpreted as `int`, `char` or pointer.
However, all variables, functions and arrays must be declared `int`.

1.  There must be an `int main()` which appears as the last function.

2.  Arrays and variables may be local or global (implied static).

3.  Arrays may only be single indexed. See `PS.CC` for an example.

4.  Do not attempt to scope variables within a function. e.g. `for (int
    i=...`

5.  The only allowed digraphs are `++`, `--` (postfix only) and `==`.

6.  Comparison operators compare signed. Result is boolean true = -1.

7.  `&`, `|` and `!` are bitwise.

8.  See `libc.h` for allowed libc functions. There are 31.

9.  `atoi` is non-standard: `int atoi(char *,int *)`, returning
        the length of the numeric string.

10. `fopen` is implemented as `void fopen(char *filename, char *mode)`.
    The filename must be upper case. Mode is either "w" or "r".

    *   Only 1 input file and 1 output may be open at any one time

    *   `fclose()` only closes the output file.

    *   Call `fopen` to open a new input file. The current file does not
        need to be closed.

    *   `fprintf`, `fputc`, and `fputs` are as expected.

    *   `fgets` is implemented. It will read and retain CR/LF. It returns
        a null string on EOF.

    *   `fscanf` is not implemented. Read a line with `fgets()` and then
        call `sscanf` on it.

    *   `feof` is not implemented; `fgetc` and `fgets` will return a
        null on EOF.

11. `scanf` is not implemented; use `gets` then `sscanf`

12. `printf` is as expected. See `libc.c` for the allowed format
    specifiers: `%d`, `%s` etc. Length and width.precision formatting is
    supported.

13. Pointers are as expected. Do not try `*(<expr>)`. This does not
    work.

14. `struct` is not supported.

15. Double precision `int`, `float` etc. are not supported. If you need
    to do heavy duty maths, use Fortran.

16. The stack, which includes all globals and literals, is only 4k.
    Stack overflow is not detected.

17. Recursion is implemented. See `FIB.CC`.

18. Literals have to be included in the 4K limit programme area. These
    are copied into the stack at run time. This is due to the fact that
    ‘COMMON’ storage cannot be initialised.

19. There is no option for `#include` files. The available Libc
    functions are implicitly declared and listed in `libc.h` and `p8.c`.
    As a result, there is no arglist checking. Examine `libc.c` for
    details.


## OS/8 Specifics

1.  I strongly suggest you limit I/O to text files.

2.  Don’t forget to handle form feed.  See `c8.c`

3.  For some obscure reason, always open the input file first, then the
    output file. I suspect a fault in `libc.c`. Examine the code!!!!
    (Every trick in the book.)


## Preprocessor

The compiler distribution includes a pre-processor file (`c8.c` &rarr;
`CC0.SV`). This is a stub and merely asks for a filename and calls the
compiler chain. This file may be extended and used to process `#define`
`#include`, etc.

Compile using the cross-compiler to `c8.s`, copy to `C8.SB` under OS/8
on your PDP-8 target system, then:

    .COMP C8.SB
    .R LOADER
    *C8,LIBC/I/O$
    .SAVE SYS CC0
    .R CC0
    >                   enter filename and press Return
    .COMP CC.SB
    .R LOADER
    *CC,LIBC/G          if no file I/O; or...
    *CC,LIBC/I/O/G      ...if using file I/O

Try the examples in `*.CC` on `DSK:`. I personally like `PS.CC`,
Pascal’s triangle. This version does not require factorials, which are a
bit out of range for 12 bits!!

Try it with:

    .R CC0
    >PS.CC
    .COMP CC.SB
    .R LOADER
    *CC,LIBC/G


## Conclusion

This is a somewhat limited manual which attempts to give an outline of a
very simple compiler for which I apologise as the source code is obscure
and badly commented. However, the native OS/8 compiler/tokeniser
(`n8.c`) is only 600 lines which is a nothing in the scale of things
these days.  However, I hope this project gives some insight into
compiler design and code generation strategies to target a most
remarkable computer. I would also like to give credit to the builders of
OS/8 and in particular the FORTRAN II system which was never designed to
survive the onslaught of this kind of modern software.

Don’t expect too much! This compiler will not build this week’s bleeding
edge kernel. But, it may be used to build any number of useful utility
programs for OS/8.


## License

This document is under the [GNU GPLv3 License][gpl], copyright © May and
June 2017 by [Ian Schofield][ian], with minor updates by [Warren
Young][wy] in July 2017.

[gpl]: https://www.gnu.org/licenses/gpl.html
[ian]: mailto:Isysxp@gmail.com
[wy]:  https://tangentsoft.com/
