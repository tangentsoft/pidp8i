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


## GOVERNMENT HEALTH WARNING

**You are hereby warned**: The native OS/8 compiler does not contain any
error checking whatsoever. If the source files contain an error, you
may get:

*   A runtime crash in the compiler
*   SABR assembly output that won't assemble
*   Output that assembles but won't run correctly

Rarely will any of these failure modes give any kind of sensible hint as
to the cause. OS/8 CC8 cannot afford the hundreds of kilobytes of error
checking and text reporting that you get in a modern compiler like GCC
or Clang. That would have required a roomful of core memory to achieve
on a real PDP-8. Since we're working within the constraints of the old
PDP-8 architecture, we only have about 3 kWords to construct the parse
result, for example.

In addition, the native OS/8 compiler is also severely limited in code
space, so it does not understand the full C language. It is almost
certainly less functional in all respects than K&R C 1978; we do not
have a good benchmark for what it compares to in terms of other early C
dialects, but we can sum it up in a single word: "primitive."

Nonetheless, our highly limited C dialect is Turing complete. It might
be better to think of it as a high-level assembly language that
resembles C rather than as "C" proper.


### Features

Here is what is known to work:

1.  **Local and global variables**

1.  **Pointers,** within limitations given in the following section.

1.  **Functions:** Parameter lists must be declared in K&R form:

        int foo (a, b)
        int a, b;
        {
            ...
        }

1.  **Recursion:** See [`FIB.CC`][fib] for an example of this.

[fib]: https://tangentsoft.com/pidp8i/doc/src/cc8/examples/fib.c

1.  **Simple arithmetic operators:** `+`, `-`, `*`, `/`, etc.

1.  **Bitwise operators:** `&`, &brvbar;, `~` and `!`

1.  **Simple comparison operators:** False expressions evaluate as 0 and
    true as -1 in twos complement form, meaning all 1's in binary form.
    See the list of limitations below for the operators excluded by our
    "simple" qualifier.

1.  **A few 2-character operators:** `++`, `--` (postfix only) and `==`.

1.  **Limited library:** See `libc.h` for allowed libc functions, of
    which there are currently 31, including:

    1.  **A subset of stdio:**

        *   `fopen` is implemented as
        
                void fopen(char *filename, char *mode)
                
            The filename must be upper case. Mode is either "w" or "r".

        *   Only 1 input file and 1 output may be open at any one time

        *   `fclose()` only closes the output file.

        *   Call `fopen` to open a new input file. The current file does
            not need to be closed.

        *   `fprintf`, `fputc`, and `fputs` are as expected.

        *   `fgets` is implemented. It will read and retain CR/LF. It
            returns a null string on EOF.

        *   `fscanf` is not implemented. Read a line with `fgets()` and
            then call `sscanf` on it.

        *   `feof` is not implemented; `fgetc` and `fgets` will return a
            null on EOF.

    1.  **printf:**  See `libc.c` for the allowed format specifiers:
        `%d`, `%s` etc.  Length and width.precision formatting is supported.

    There are many limitations in this library relative to Standard C or
    even K&R C, which are documented below.

1.  **Limited structuring constructs:** `if`, `while`, `for`, etc. are
    supported, but they may not work as expected when deeply nested or
    in long `if/else if/...` chains.


### Known Limitations

Much of what you understand as "C" does not work in our dialect:

1.  The language is typeless in that everything is a 12 bit integer and
    any variable/array can interpreted as `int`, `char` or pointer.  All
    variables and arrays must be declared as `int`.  The return type may
    be left off of a function's definition; it is implicitly `int` in
    all cases, since `void` is not supported.

2.  There must be an `int main()` which must be the last function in the
    single input C file.

3.  We do not yet support separate compilation of multiple C modules
    that get linked together.  You can produce relocatable libraries in
    OS/8 `*.RL` format and link them with the OS/8 LOADER, but because
    of the previous limitation, only one of these can be written in C.

4.  Unlike the CC8 cross-compiler, the OS/8 compiler ignores `#include`
    directives.  (One day, `CC0` may get that ability, but not today.)
    This means you cannot use `#include` directives to string multiple C
    modules into a single program.

    If that then makes you wonder how the OS/8 compiler looks up its
    stock library functions — note that I've resisted using the word
    "standard" here, for they are anything but that in the Standard C
    sense — it is that the entry point mappings declared in `libc.h` are
    hard-coded into the `CC2` compiler stage, implemented in `p8.c`.

    If you have a program that is compiled using both the cross-compiler
    and the OS/8 compiler, you may wish to use `#include` statements,
    since the cross-compiler does process them.

5.  Variables are implicitly `static`, even when local.

6.  Arrays may only be single indexed. See `PS.CC` for an example.

7.  We do not even support all of K&R C yet, much less post-C89 features
    such as statement-scoped variable declarations:

        for (int i = ...

    All variables must either be predeclared at the top of the function
    they're used in, or they must be global.

8.  The compiler does not yet understand how to assign a variable's
    initial value as part of its declaration. This:

        int i = 5;

    must instead be:

        int i;
        i = 5;

9.  There is no `&&` nor &brvbar;&brvbar;.  Neither is there support for
    complex relational operators like `>=` nor even `!=`.  Abandon all
    hope for complex assignment operators like `+=`.

    Most of this can be worked around through clever coding. For
    example, this:

        if (i != 0 || j == 5)

    could be rewritten to avoid both missing operators as:

        if (!(i == 0) | (j == 5))

    because a true result in each subexpression yields -1 per the
    previous point, which when bitwise OR'd together means you get -1 if
    either subexpression is true, which means the whole expression
    evaluates to true if either subexpression is true.

    If the code you were going to write was instead:

        if (i != 0 || j != 5)

    then the rewrite is even simpler owing to the rules of [Boolean
    algebra](https://en.wikipedia.org/wiki/Boolean_algebra):

        if (!(i == 0 & j == 5))

    These rules mean that if we negate the entire expression, we get the
    same truth table if we flip the operators around and swap the
    logical test from OR to AND, which in this case converts the
    expression to a form that is now legal in our limited C dialect. All
    of this comes from the Laws section of the linked Wikipedia article;
    if you learn nothing else about Boolean algebra, you would be well
    served to memorize those rules.

10. `atoi` is non-standard: `int atoi(char *, int *)`, returning
     the length of the numeric string.

11. `scanf` is not implemented; use `gets` then `sscanf`

12. Dereferencing parenthesized expressions does not work: `*(<expr>)`

13. `struct` is not supported.

14. Double precision `int`, `float` etc. are not supported. If you need
    to do heavy duty maths, use FORTRAN, U/W FOCAL, or even OS/8 BASIC.

15. The stack, which includes all globals and literals, is only 4 kwords.
    Stack overflow is not detected.  Literals are inlcuded in this due
    to a limitation in the way `COMMON` is implemented in SABR.

16. There is no argument list checking, not even for standard library
    functions.

17. `do/while` is parsed, but code for it is not properly generated.


### Known Bugs

1.  Binary file I/O is not always reliable.  You are strongly encouraged
    to limit I/O to text files.

2.  Don’t forget to handle form feed.  See `c8.c`.

3.  For some obscure reason, always open the input file first, then the
    output file. I suspect a fault in `libc.c`, which you are welcome to
    fix, keeping in mind that we're using every trick in the book to fit
    as much functionality in as we currently do.  It may not be possible
    to make this as reliable as modern C programmers expect.


## <a id="pre"></a>Preprocessor

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

This document is under the [GNU GPLv3 License][gpl], copyright © May,
June, and November 2017 by [Ian Schofield][ian], with minor updates by
[Warren Young][wy].

[gpl]: https://www.gnu.org/licenses/gpl.html
[ian]: mailto:Isysxp@gmail.com
[wy]:  https://tangentsoft.com/
