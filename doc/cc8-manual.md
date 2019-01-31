# CC8 Manual

CC8 is a C subset implementation for the DEC PDP-8 processor.


## A Bit of Grounding History

The C language and its derivatives are now the industry standard for the
development of operating systems and utilities. The language has evolved
significantly since its initial specification in 1972.

The first implementation of C was on the PDP-11 as part of the early
work on the Unix operating system, and it was initially used to write
system utilities that otherwise would have been written in assembly. A C
language compiler first appeared publicly in Version 2 Unix, released
later in 1972. Much of PDP-11 Unix remained written in assembly until
its developers decided to rewrite the operating system in C, for Version
4 Unix, released in 1973. That decision allowed Unix to be relatively
easily ported to a wholly different platform — the Interdata 8/32 — in
1978 by writing a new code generator for the C compiler, then
cross-compiling everything. That success in porting Unix lead to C’s own
success first as a systems programming language, and then later as a
general-purpose programming language.

The PDP-8 was introduced by DEC in 1965 with the intention of being a
small and cheap processor that could be used in a variety of
environments. From this simple machine, the modern desktop computer
evolved, some of which were used in the writing of this document. The
PDP-8 is also arguably the ancestor of the Raspberry Pi you may be using
our CC8 compiler on, as part of the PiDP-8/I project.

The PiDP-8/I project is part of an effort to prevent the PDP-8 from
sliding into undeserved obscurity. Whether you consider it the ancestor
of the desktop computer or the embedded processor, it is a machine worth
understanding.


## CC8’s Developmental Sparks

With this in mind, the CC8 project’s creator (Ian Schofield) thought it
was time to have a modern language compiler running on the PDP-8.  The
last such compiler to be attempted for the PDP-8, as far as this
document’s authors are aware, was Pascal in 1979 by Heinz Stegbauer.

In more recent times, one cross-compiler has been developed by Vince
Slyngstad and updated by Paolo Maffei based on Ron Cain’s Small-C using
a VM approach. [This code][sms] is most certainly worth examining, and I
am delighted to acknowledge this work as I have used some of his [C
library code](#lib) in this project.

[sms]: http://so-much-stuff.com/pdp8/C/C.php

Finally, I would refer the reader to Fabrice Bellard’s OTCC. It is this
bit of remarkable software that suggested that there may be a chance to
implement a native PDP-8 compiler.


## The Project’s Products

To this end, one of us (Ian Schofield) wrote two C compilers for the PDP-8:

1.  A cross-compiler that builds and runs on any host computer with a C
    compiler that still understands K&R C.

2.  A native OS/8 compiler and library, compiled to assembly by the
    cross-compiler.


<a id="cross" name="posix"></a>
### The Cross-Compiler

The features of the cross-compiler are basically that of Small-C itself,
being a good approximation of K&R C (1978) minus:

*   most of the standard library (see [below](#lib) for what we *do*
    have)

*   `struct` and `union`

*   function pointers

*   `float` and `long`

The code for this is in the `src/cc8/cross` subdirectory of the PiDP-8/I
source tree, and it is built along with the top-level PiDP-8/I software.
When installed, this compiler is in your `PATH` as `cc8`.

Ian Schofield wrote the CC8 cross-compiler as a [SABR][sabr] code
generator for Ron Cain’s famous Small-C compiler, originally published
in [Dr Dobb’s Journal][ddj], with later versions published elsewhere.
William Cattey later ported this code base to Small-C 85, a living
project currently [available on GitHub][sc85].

This means the cross compiler more or less adheres to the *language*
dialect of C as published in "The C Programming Language," by Kernighan
and Ritche, first edition, 1978. The reader is directed to the extensive
documentation of Small-C available on the web for further details. You
may also find references for K&R C 1978 helpful.

We stress *language* above because we have not attempted to clone the C
Standard Library as of K&R 1978.  CC8 has a [very limited standard
library](#lib), and it has many weaknesses relative to even early
versions of C. See that section of this manual for details about known
limitations, exclusions, and bugs.

The CC8 cross-compiler can successfully compile itself, but it produces
a SABR assembly file that is too large (28K) to be assembled on the
PDP-8.  Thus [the separate native compiler](#native).

The key file, relative to the base Small-C project, is the PDP-8 code
generator in `code8.c` which emits SABR assembly code.  SABR is normally
used as the second pass of the OS/8 FORTRAN II system.

When you use the cross-compiler on a POSIX type system such as the
Raspbian PiDP-8/I environment, the resulting `*.sb` files will have
LF-only line endings, but OS/8 expects CR+LF line endings. The `txt2ptp`
utility program included with the PiDP-8/I distribution will
automatically do that conversion for you when making a SIMH paper tape
image file, which you can then read into the OS/8 environment.

The cross-compiler has some non-standard features to enable the
interface between the main programme and the C library. This constitutes
a compile time linkage system to allow for standard and vararg functions
to be called in the library.

Several of the C programs in this distribution `#include <init.h>` which
inserts an assembly language initialization routine into the program at
that point using the `#asm` [inline assembly feature](#asm). This file
is symlinked into each directory that has a `*.c` file needing it since
CC8 doesn't have an include path feature, and it must be in the current
directory in any case when using the OS/8 version of CC8.

The `init.h` initialization routine defines some low-level subroutines,
initializes the environment for the programs, and calls into the LIBC
initialization code. This file was copied to the OS/8 boot disk as
`DSK:INIT.H` unless you gave `--disable-os8-cc8` when configuring the
PiDP-8/I software.

The file `include/libc.h` is likewise copied to `DSK:LIBC.H`. It defines
the mappings between the familiar C library routine names and their
underlying implementation names.

The linking loader determines the core layout for the built programs.
Most commonly, it uses this scheme:

<a id="fields"></a>
**Field 0:** FOTRAN library utility functions and OS/8 I/O system

**Field 1:** The programme’s runtime stack/globals/literals

**Field 2:** The programme's executable code

**Field 3:** The LIBC library code

Since this memory layout applies to the phases of the CC8 compiler as
well, this means that each phase uses approximately 16 kWords of core.

[ddj]:  https://en.wikipedia.org/wiki/Dr._Dobb%27s_Journal
[sabr]: https://tangentsoft.com/pidp8i/wiki?name=A+Field+Guide+to+PDP-8+Assemblers#sabr
[sc85]: https://github.com/ncb85/SmallC-85


<a id="asm" name="calling"></a>
#### Inline Assembly Code and the CC8 Calling Convention

The cross-compiler allows SABR assembly code (**TBD:** true?) between
`#asm` and `#endasm` markers in the C source code:

    #asm
        TAD (42      / add 42 to AC
    #endasm

You can write whole functions in inline assembly, though for simplicity,
we recommend that you write the function wrapper in C syntax, with the
body in assembly:

    foo(a)
    int a
    {
        a;
    #asm
        / assembly body here
    #endasm
    }

This declares a function `foo` taking a single integer parameter and
returning an integer. 

The calling convention is for the parameters to be passed on [the
stack](#fields), with the return value in AC.  It is common in C
functions with inline assembly to not have explicit “`return`”
statements, but instead to have set up AC just before the implicit
return.

The above `foo` example also shows another common technique in CC8
functions using inline assembly: there being no “void context” in K&R C,
the first line of that function has the nonstandard meaning “load `a`
into AC.” In Standard C, that line would have no effect on the state of
the program or its external environment, so it would simply be optimized
out.

This same technique is used in other ways in well-optimized CC8 code.
For example, you may call a function that returns a value, but never
explicitly store it anywhere if the call is immediately followed by
inline assembly that looks for its input in the accumulator, knowing
that’s where CC8 put the prior call’s return value.

Inline assembly code is copied literally from the input C source file
into the SABR output, so it must be written with that context in mind.
(**TBD:** True?)

A block of inline assembly functions as single statement in the C
program, from a syntactic point of view. Consider the implementation of
the Standard C function `puts` from the CC8 LIBC:

    puts(p)
    char *p;
        {
            while (*p++) 
    #asm
            TLS
    XC1,    TSF
            JMP XC1
    #endasm
        }

Notice that there is no opening curly brace on the `while` loop: when
the `TSF` opcode causes the `JMP` instruction to be skipped — meaning
the console terminal is ready for another output character — control
goes back to the top of the `while` loop. That is, these three
instructions behave as if they were a single C statement and thus
constitute the whole body of the `while` loop.

Note also in the `puts` example that the statement `*p++` implicitly
stores the value at the core memory location referred to by `p` in AC.
Knowing what the compiler has done with values just prior to entering an
inline assembly block is key to using CC8’s inline assembly feature
successfully. Reading the resulting SABR output from the compiler can
therefore be quite helpful in optimizing your code.

Remember: inline assembly is a feature of the cross-compiler only. The
native OS/8 compiler ignores all preprocessor directives, including
`#asm`!


<a id="native" name="os8"></a>
### The Native OS/8 Compiler

This compiler is supplied in both source and binary forms as part of the
PiDP-8/I software distribution.

We ship pre-built binaries to avoid a chicken-and-egg problem: the
binaries require a working OS/8 environment to be built, but when the
PiDP-8/I build system goes to build the bootable OS/8 media, it expects
to have the OS/8 CC8 binaries at hand so it can copy them to the RK05
disk it is building. It's trivial to deal with that on our development
systems, since we normally have a working `os8v3d-*.rk05` disk set from
the previous build sitting around to bootstrap the process, so we break
the cycle at that point rather than do a two-stage RK05 bootstrap build
on end-user systems.

These pre-built binaries are saved as `media/os8/subsys/cc8.tu56` by the
`tools/cc8-tu56-update` script. Basically, that script uses the
cross-compiler to produce SABR assembly files for each stage of the OS/8
CC8 compiler, which it then copies into the OS/8 environment, then it
assembles, links, and saves the result as `CC?.SV`:

2.  `c8.c` &rarr; `c8.sb` &rarr; `CC.SV`: The compiler driver: accepts
    the input file name from the user, and calls the first proper
    compiler stage, `CC1`.

    If we should add a preprocessor feature, it might become part of
    `CC.SV`, or it might be a separate program, probably called either
    `CC0.SV` or `CPP.SV`, which `CC.SV` calls before calling `CC1`.

2.  `n8.c` &rarr; `n8.sb` &rarr; `CC1.SV`: The parser/tokeniser section
    of the compiler.

3.  `p8.c` &rarr; `p8.sb` &rarr; `CC2.SV`: The token to SABR code
    converter section of the compiler.

4.  `libc.c` &rarr; `libc.sb` &rarr; `LIBC.RL`: The C library linked to
    any program built with CC8, including the stages above, but also to
    your own programs.

If you are not changing the OS/8 CC8 source code, you needn't run the
`cc8-tu56-update` script or build the OS/8 version of CC8 by hand.

The PiDP-8/I build system's OS/8 RK05 media build script copies those
files and the other files required for building C programs under OS/8 to
the appropriate OS/8 volumes: `CC?.SV` on `SYS:`, and everything else on
`DSK:`. 

Input programs should go on `DSK:`. Compiler outputs are also placed on
`DSK:`.


<a id="nfeat" name="features"></a>
#### Features of the Native OS/8 Compiler

The following is the subset of C known to be understood by the native
OS/8 CC8 compiler:

1.  **Local and global variables**

1.  **Pointers,** within limitations given below.

1.  **Functions:** Parameter lists must be declared in K&R form:

        int foo (a, b)
        int a, b;
        {
            ...
        }

1.  **Recursion:** See [`FIB.C`][fib] for an example of this.

1.  **Simple arithmetic operators:** `+`, `-`, `*`, `/`, etc.

1.  **Bitwise operators:** `&`, &brvbar;, `~` and `!`

1.  **Simple comparison operators:** False expressions evaluate as 0 and
    true as -1 in twos complement form, meaning all 1's in binary form.
    See the list of limitations below for the operators excluded by our
    "simple" qualifier.

1.  **A few 2-character operators:** `++`, `--` (postfix only) and `==`.

1.  **Limited library:** See [below](#lib) for a list of library
    functions provided, including their known limitations relative to
    Standard C.

    There are many limitations in this library relative to Standard C or
    even K&R C, which are documented below.

1.  **Limited structuring constructs:** `if`, `while`, `for`, etc. are
    supported, but they may not work as expected when deeply nested or
    in long `if/else if/...` chains.

[fib]: https://tangentsoft.com/pidp8i/doc/src/cc8/examples/fib.c


<a id="nlim" name="limitations"></a>
### Known Limitations of the OS/8 CC8 Compiler

The OS/8 version of CC8 is missing many language features relative to
[the cross-compiler](#cross), and much more compared to modern C.

1.  The language is typeless in that everything is a 12 bit integer and
    any variable/array can interpreted as `int`, `char` or pointer.  All
    variables and arrays must be declared as `int`.  The return type may
    be left off of a function's definition; it is implicitly `int` in
    all cases, since `void` is not supported.

    Further to this point, in the OS/8 version of CC8, it is optional
    to declare the types of the arguments to a function. For example,
    the following is likely to be rejected by a strictly conforming
    K&R C compiler, but it is legal in OS/8 CC8 because the types
    are already known, there being only one data tyype in OS/8 CC8:

        myfn(n) { /* do something with n; optionally return something */ }

    This declares a function taking an `int` called `n` and returning
    an `int`. Contrast the CC8 cross-compiler, which requires the
    function's argument type to be declared, if not the return type:

        myfn(n)
        int n;
        {
            /* do something with n; optionally return something */
        }

    (The return type cannot be `void` since there is no `void` in
    K&R C as published in 1978, thus not in CC8, either.)

2.  There must be an `int main()`, and it must be the last function
    in the single input C file.

    Since OS/8 has no way to pass command line arguments to a program
    — at least, not in a way that is compatible with the Unix style
    command lines expected by C — the `main()` function is never
    declared to take arguments.

3.  We do not yet support separate compilation of multiple C modules
    that get linked together.  You can produce relocatable libraries in
    OS/8 `*.RL` format and link them with the OS/8 LOADER, but because
    of the previous limitation, only one of these can be written in C.

4.  Unlike the CC8 cross-compiler, the OS/8 compiler currently ignores
    all C preprocessor directives: `#define`, `#ifdef`, `#include`,
    etc.  This even includes [inline assembly](#asm) via `#asm`!

    This means you cannot use `#include` directives to string multiple
    C modules into a single program.

    If that then makes you wonder how the OS/8 compiler looks up the
    stock library functions defined in `libc.h` — note that I've
    resisted using the word "standard" here, for they are anything but
    that in the Standard C sense — it is that the entry point mappings
    declared in `libc.h` are hard-coded into the `CC2` compiler stage,
    implemented in `p8.c`.

    Similarly, the program initialization code defined in `init.h` is
    inserted into the program directly by the compiler rather than being
    pulled in via the preprocessor.

    Both of these header files must be included when building with the
    cross-compiler. The examples have these `#include` statements
    stripped out as they are copied to the OS/8 RK05 disk during the
    build process. This is done by `bin/cc8-to-os8`, a tool you may find
    use for yourself if you use both compilers on a single source
    program.

    If you have a program that is compiled using both the cross-compiler
    and the OS/8 compiler, you may wish to use `#include` statements,
    since the cross-compiler does process them.

5.  Variables are implicitly `static`, even when local.

6.  Arrays may only be single indexed. See `PS.C` for an example.

7.  The compiler does not yet understand how to assign a variable's
    initial value as part of its declaration. This:

        int i = 5;

    must instead be:

        int i;
        i = 5;

8.  There is no `&&` nor &brvbar;&brvbar;.  Neither is there support for
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

9. Dereferencing parenthesized expressions does not work: `*(<expr>)`

10. The stack, which includes all globals and literals, is only 4 kwords.
    Stack overflow is not detected.  Literals are inlcuded in this due
    to a limitation in the way `COMMN` is implemented in SABR.

11. There is no argument list checking, not even for standard library
    functions.

12. `do/while` loops are parsed, but the code is not properly generated.
    Regular `while` loops work fine, however.

13. `switch` doesn't work.

The provided [LIBC library functions](#lib) is also quite limited and
nonstandard compared to Standard C.  See the documentation for each
individual library function for details.


<a id="bugs"></a>
### Known Bugs in the OS/8 CC8 Compiler

1.  Binary file I/O is not always reliable.  You are strongly encouraged
    to limit I/O to text files.

2.  Don’t forget to handle form feed.  See `c8.c`.

3.  For some obscure reason, always open the input file first, then the
    output file. I suspect a fault in `libc.c`, which you are welcome to
    fix, keeping in mind that we're using every trick in the book to fit
    as much functionality in as we currently do.  It may not be possible
    to make this as reliable as modern C programmers expect.


<a id="warning"></a>
#### GOVERNMENT HEALTH WARNING

**You are hereby warned**: The native OS/8 compiler does not contain any
error checking whatsoever. If the source files contain an error or you
mistype a build command, you may get:

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

In addition, the native OS/8 compiler is severely limited in code space,
so it does not understand the full C language. It is less functional
than K&R C 1978; we do not have a good benchmark for what it compares to
in terms of other early C dialects, but we can sum it up in a single
word: primitive.

Nonetheless, our highly limited C dialect is Turing complete. It might
be better to think of it as a high-level assembly language that
resembles C rather than as "C" proper.


<a id="lib"></a>
## The Standard Library

CC8 offers a very limited standard library, which is shared between the
two compilers, native and cross.  While some of its function names are
the same as functions defined by Standard C, you should read the source
code in `src/cc8/os8/libc.c` to find out what’s actually provided.  This
section of the manual *attempts* to summarize the limitations relative
to either K&R C’s standard library or to ISO C, but it is quite possible
that we have overlooked some corner case that our library does not yet
implement.

This is a **WORK IN PROGRESSS**. None of the documentation below has
yet been cross-checked between the code and the ISO C Standard.

In the following text, we use OS/8 device names as a handwavy kind of
shorthand, even when the code would otherwise run on any PDP-8 in
absence of OS/8. Where we use “`TTY:`”, for example, we’d be more
precise to say instead “the console teleprinter, being the one that
responds to IOT device code 3 for input and to device code 4 for
output.” We’d rather not write all of that for every stdio function
below, so we use this shorthand.


### <a id="libclim"></a>General Limitations of the CC8 LIBC

The stdio implementation currently assumes US-ASCII text I/O. Input
characters have their upper 5 bits masked off. The functions accepting
data for output will tolerate 8 bit data in some cases, but since you
can’t read it back in safely with this current implementation, you
probably should not do this.

The stdio implementation only allows one file to be open for reading at
a time, and one file for writing. The `fclose()` implementation takes no
argument: it always closes the output file, if any. There is no need for
`fclose()` on the input file (if any) because there is no standard input
buffering in this implementation, so there is nothing to free before
opening another file for input.

A great many Standard C Library functions are not provided, including
some you’d think would go along nicely with those we do provide, such as
`feof()` or `fseek()`.  Keep in mind that the library must currently fit
in [a single 4&nbsp;kWord field](#fields).  It is therefore unlikely
that it will expand much or at all beyond the currently provided 33
functions, and those functions will likely not ever match the behavior
that a C programmer expects of functions with these names in a modern
programming environment.


### <a id="atoi"></a>`atoi(s, outlen)`

Takes a null-terminated ASCII character string pointer `s` and returns a
12-bit PDP-8 two’s complement signed integer. The length of the numeric
string is returned in `*outlen`.

**Standard Violations:**

*   Skips leading ASCII 32 (space) characters only, not those matched by
    [`isspace()`](#isspace), as the Standard requires.

*   The `outlen` parameter is nonstandard.


### <a id="cupper"></a>`cupper`

Like [`toupper()`](#toupper), but stores its result in the same memory
location as you passed, saving a few instructions relative to using
`toupper`, if that’s the result you wanted.

**Nonstandard.** Conforming to...?


### <a id="dispxy"></a>`dispxy`

??

**Nonstandard.** Conforming to...?


### <a id="exit"></a>`exit(ret)`

Exits the program.

**TBD**: What does `CALL 0,EXIT` mean? Is that a FORTRAN II library
thing? Can a user register something there to get `atexit` behavior?
Is that why the `CALL` is followed by `HLT` rather than expecting the
call to never return?

**Standard Violations:**

*   The passed return code is ignored.


### <a id="fclose"></a>`fclose()`

Closes the opened output file.

**Standard Violations:**

*   Does not take a `FILE*` argument.  (See [`fopen()`](#fopen) for
    justification.)

*   Always closes the last-opened *output* file: cannot close a file
    opened for input with `fopen(name, "r")`.  If you want to open a
    second input file, simply call [`fopen()`](#fopen) on it, implicitly
    closing the prior input file.  There being no read buffering in LIBC
    to speak of, there’s nothing to free with an `fclose()` call on an
    input file.


### <a id="fgets"></a>`fgets(s)`

Reads a string of ASCII characters from the last file opened for input
by [`fopen()`](#fopen), storing it at core memory location `s`. It reads
until it encounters an LF character, storing that and a trailing NUL
before returning, because it assumes the OS/8 convention of CR+LF
terminated text files.

Returns 0 on EOF, as Standard C requires.

**Standard Violations:**

*   Returns the number of characters read on success, rather than `s` as
    Standard C requires.

*   Since EOF is the only error exit case from this implementation of
    `fgets()`, this LIBC does not provide `feof()`.


### <a id="fopen"></a>`fopen(name, mode)`

Opens an OS/8 file called `name`, which must be in all-uppercase.

The file is opened for reading if `mode` points to an ”`r`” character,
and it is opened for writing if `mode` points to a “`w`” character.

**TBD:** Does it obey OS/8 device names, or is the file always on `DSK:`
or similar?

`flag` is a null-terminated ASCII string with one or more (?) letters in
the following set, **TBD**.

**Standard Violations:**

*   Does not return a `FILE*`. Functions which, in Standard C, take a
    `FILE*` argument do not do so in the CC8 LIBC, because there can be
    only one opened input file and one opened output file at a time, so
    the file that is meant is implicit in the call.


### <a id="getc" name="fgetc"></a>`getc()`, `fgetc()`

Reads a single ASCII character from `TTY:` or from the last file opened
for input by [`fopen()`](#fopen), respectively.

**Standard Violations:**

*   `feof()` is not provided by CC8 LIBC: detect EOF with a null return
    from `fgetc()`.


### <a id="gets"></a>`gets(s)`

Reads a string of ASCII characters from `TTY:`, up to and including the
terminating CR character, storing it at core memory location `s`, and
following the terminating CR with a NUL character.

Backspace characters from the terminal remove the last character from
the string.

**Standard Violations:**

*   `gets()` always returns 1 in this implementation, but it returns the
    passed string pointer in Standard C libraries.


### <a id="isalnum"></a>`isalnum(c)`

Returns &gt; 1 if either [`isdigit()`](#isdigit) or
[`isalpha()`](#isalpha) returns 1 for `c`.

**Standard Violations:**

*   None known.


### <a id="isalpha"></a>`isalpha(c)`

Returns true if the passed character `c` is either between 65 and 90 or
between 97 and 122 inclusive, being the ASCII alphabetic characters.

**Standard Violations:**

*   Does not know anything about locales; assumes US-ASCII input.


### <a id="isdigit" name="isnum"></a>`isdigit(c)`, `isnum(c)`

Returns true if the passed character `c` is between 48 an 57, inclusive,
being the ASCII decimal digit characters.

**Standard Violations:**

*   `isnum` is a nonstandard alias for `isdigit` conforming to **TBD**.
    Both are implemented with the same LIBC code.


### <a id="isspace"></a>`isspace`

Takes an ASCII character and returns 1 if the character is considered a
“whitespace” character.

This function is not used by `atoi`: its whitespace matching is
hard-coded internally.

**Standard Violations:**

*   Whitespace is currently defined as ASCII 1 through 32, inclusive.
    Yes, yhis is certainly a vast overreach.


### <a id="itoa"></a>`itoa`

Converts 12-bit PDP-8 words to ASCII.

**TBD**: Where’s the buffer? Does it take the input to be two’s
complement or unsigned? No thousands separator, right?

**Nonstandard.** Emulates a function most often found in compilers from
the MS-DOS and Windows tradition.


### <a id="kbhit"></a>`kbhit`

Stops the program until a keystroke is detected on `TTY:`, which can
subsequently be read by calling `getc`.

**Nonstandard.** Emulates a function most often found in compilers from
the MS-DOS and Windows tradition.


### <a id="memcpy"></a>`memcpy`

Copies a given number of words from one core memory location to another.

**TBD**: Do we want to document the direction that is safe when copying
overlapping buffers, nailing that in place?  Can it handle page
transitions?  Field transitions?  If there are limitations there, does
it work between whole fields/pages? That is, if it handles neither page
nor field transitions, can it copy from one page to another as long as
both buffers fit completely within their respective pages?

**Standard Violations:**

*   None known.


### <a id="memset"></a>`memset(dst, c, len)`

Sets a run of `len` core memory locations starting at `dst` to `c`.

**TBD**: Can `dst+len` cross a field or page boundary from `dst`?

**Standard Violations:**

*   None known.


### <a id="printf" name="fprintf"></a>`printf(fmt, args...)`,
`fprintf(fmt, args...)`, `sprintf(outstr, fmt, args..)`

Writes formatted values to `TTY:`, to the output file opened with
`fopen`, or to the null-terminated ASCII string buffer `outstr`,
respectively.

See `src/cc8/os8/libc.c` for the allowed format specifiers: `%d`, `%s`
etc.  Length and width.precision formatting is supported.

**Standard Violations:**

*   `fprintf` does not take a `FILE*` pointer as its first argument. It
    simply writes to the one and only output file that can be opened at
    a time by [`fopen()`](#fopen).

*   **TBD**: There must be a whole lot defined by Standard C that this
    implementation cannot handle: format specs, modifiers, the return
    value, error handling...


### <a id="puts" name="fputs"></a>`puts`, `fputs(fp)`

Writes a null-terminated character string to `TTY:`. The characters
pointed to are expected to be 7-bit ASCII bytes stored within each PDP-8
word, with the top 5 bits unset.

**Standard Violations:**

*   None known.


### <a id="revcpy"></a>`revcpy`

??

**Nonstandard.** Conforming to...?


### <a id="sscanf"></a>`sscanf`

Reads formatted input from a file.

**Standard Violations:**

*   `[f]scanf()` is not provided. Call [`[f]gets()`](#gets) to get a
    string and then call `sscanf()` on it.

*   This list cannot possibly be complete.


### <a id="strcat"></a>`strcat`

Concatenates one null-terminated character string to the end of another.

**TBD**: Much the same as `memcpy`: do both strings have to be in the
same page/field, are page/field crossings legal, etc.?

**Standard Violations:**

*   None known.


### <a id="strcpy"></a>`strcpy`

Copies a null-terminated character string from one memory location to
another.

**TBD**: Same as `memcpy`.

**Standard Violations:**

*   None known.


### <a id="strstr"></a>`strstr(haystack, needle)`

Attempts to find a given null-terminated substring within another.

**TBD**: Much the same as `strcpy`, plus: naive algorithm, not BMH,
right? Does it blow up if the needle is bigger than the haystack? Is it
actually a word string comparison function, or does it mask off the top
4 or 5 bits to be a *character* comparison function?

**Standard Violations:**

*   None known.


### <a id="toupper"></a>`toupper(c)`

Returns the uppercase form of ASCII character `c` if it is lowercase,
Otherwise, returns `c` unchanged.

**Standard Violations:**

*   There is no `tolower()` in the CC8 LIBC.

*   Does not know anything about locales; assumes US-ASCII input.


### <a id="xinit"></a>`xinit`

??

**Nonstandard.** Conforming to...?


<a id="examples"></a>
## Trying the Examples

The standard PiDP-8/I OS/8 RK05 boot disk contains several example
C programs that the OS/8 version of CC8 is able to compile.

To try the OS/8 version of CC8 out, boot OS/8 within the PiDP-8/I
environment as you normally would, then try building one of the
examples:

    .EXE CCR   ⇠ BATCH wrapper around CC?.SV: "Compile C and Run"
    >ps.c      ⇠ takes name of C program, builds, links, and runs it

This example is particularly interesting. It generates
Pascal’s triangle without using factorials, which are a bit out of
range for 12 bits!

The other examples preinstalled are:

*   **<code>calc.c</code>** - A simple 4-function calculator program.

*   **<code>fib.c</code>** - Calculates the first 10 Fibonacci numbers.
    This implicitly demonstrates CC8's ability to handle recursive
    function calls.

If you look in `src/cc8/examples`, you will find these same programs
plus `basic.c`, a simple BASIC language interpreter. This one is
not preinstalled because its complexity is currently beyond the
capability of the OS/8 version of CC8. To build it, you will have
to use [the cross-compiler](#cross), then assemble the resulting
`basic.sb` file under OS/8.

Another set of examples not preinstalled on the OS/8 disk are
`examples/pep001-*.c`, which are described [elsewhere][pce].

[pce]: https://tangentsoft.com/pidp8i/wiki?name=PEP001.C


## Making Executables

Executing `CCR.BI` loads, links, and runs your C program without
producing an executable file on disk.  You need only a small variation
on this BATCH file's contents to get an executable core image that
you can run with the OS/8 `R` command:

    .R CC                   ⇠ kinda like Unix cc(1)
    >myprog.c
    .COMP CC.SB
    .R LOADER
    *CC,LIBC/I/O$           ⇠ $ = Escape
    .SAVE SYS:MYPROG

If you've just run `EXE CCR` on `myprog.c`, you can skip the `CC` and
`COMP` steps above, reusing the `CC.RL` file that was left behind.

Basically, we leave the `/G` "go" switch off of the command to `LOADER`
so that the program is left in its pre-run state in core so that
`SAVE` can capture it to disk.


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
June, and November 2017 by [Ian Schofield][ian], with later improvements
by [Warren Young][wy] in 2017 and 2019.

[gpl]: https://www.gnu.org/licenses/gpl.html
[ian]: mailto:Isysxp@gmail.com
[wy]:  https://tangentsoft.com/
