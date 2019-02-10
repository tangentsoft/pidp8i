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


## The Project’s Major Products

To this end, one of us (Ian Schofield) wrote two C compilers for the PDP-8:

1.  A [cross-compiler](#cross) that builds and runs on any host computer
    with a C compiler that still understands K&R C.

2.  A [native OS/8 compiler](#native), compiled to assembly by the
    cross-compiler.

Ian also collected and wrote the LIBC implementation common to both
compilers.


## Requirements

The CC8 system generally assumes the availability of:

*   16&nbsp;kWords of core.

    (CC8 provides no built-in way to use more memory than this, so you
    will probably have to resort to [inline assembly](#asm) or FORTRAN
    II library linkage to get access to more than 16&nbsp;kWords of core.)

*   A PDP-8/e or higher class processor.  The CC8 compiler code and its
    [LIBC implementation](#libc) make liberal use of the MQ register
    and the BSW OPR instruction introduced with the PDP-8/e.

    This code will not run on, for example, a PDP-8/I with the EAE
    option installed, because although the EAE adds the MQ register, it
    does not give the older processor the BSW instruction.

    CC8 works on the PiDP-8/I because it is only the front panel that
    emulates a PDP-8/I. The underlying SIMH PDP-8 simulator is catholic
    in its support for PDP-8 family features: it doesn’t simulate any
    single PDP-8 family member exclusively. It is probably closest in
    behavior to a highly tricked-out PDP-8/a, meaning in part that it
    does support the MQ register and the BSW instruction.

    (Many of the CPU features of the SIMH PDP-8 simulator are hard-coded
    into the instruction decoding loop, so that there is no way to
    disable them at run time with configuration directives. If you have
    a PiDP-8/I and were expecting a strict PDP-8/I simulation underneath
    that pretty front panel, we’re sorry to pop your bubble, but the
    fact of the matter is that a PiDP-8/I is a Family-of-8 mongrel.)

*   At build time, the OS/8 FORTRAN II/SABR subsystem must be available.

*   At run time, any [stdio](#stdio) operation involving file I/O
    assumes it is running atop OS/8. For instance, file name arguments
    to [`fopen()`](#fopen) are passed to OS/8 for interpretation.

There is likely a subset of CC8-built programs which will run
independently of OS/8, but the bounds on that class of programs is not
currently clear to us.


<a id="cross" name="posix"></a>
## The Cross-Compiler

The features of the cross-compiler are basically that of Small-C itself,
being a good approximation of K&R C (1978) minus:

*   most of the standard library (see [below](#lib) for what we *do*
    have)

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
and Ritchie, first edition, 1978. The reader is directed to the extensive
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

[ddj]:  https://en.wikipedia.org/wiki/Dr._Dobb%27s_Journal
[sabr]: /wiki?name=A+Field+Guide+to+PDP-8+Assemblers#sabr
[sc85]: https://github.com/ncb85/SmallC-85


<a id="cpp"></a>
### The Cross-Compiler’s Preprocessor Features

Unlike [the native OS/8 compiler](#native), the cross-compiler does have
rudimentary C preprocessor features:

*   Literal `#define` only.  You cannot define parameterized macros.
    There are no `-D` or `-U` flags to define and undefine macros from
    the command line.

*   `#include`, but only for files in the current directory.  There is
    no include path, either hard-coded within the compiler or modifiable
    via the traditional `-I` compiler flag.

*   [Inline assembly](#asm) via `#asm`.

*   **TBD:** `#if` and such?

*   **TDB:** Token pasting?

*   **TDB:** Stringization?


### <a id="nhead"></a>Necessary Headers

There are two header files shipped with CC8, intended to be used only by
users of the cross-compiler:

*   `libc.h` — Declares the entry points used by [LIBC](#libc) using
    CC8 [library linkage directives](#linkage). If your program makes
    use of any library functions, you must `#include` this at the top of
    your program.

*   `init.h` — Inserts a block of [inline assembly](#asm) startup code
    into your program, which initializes the program environment, sets
    up LIBC, and defines a few low-level routines. Certain programs may
    get away without this code, but the rules for which programs and why
    are not currently clear to us.  **TODO:** Find out the rules that
    govern whether this is necessary.

As a rule, all cross-compiler users should include both of these at the
top of every program.

Because the cross-compiler lacks an include path feature, you generally
want to symlink these files to the directory where your source files
are. This is already done for the CC8 examples and such.

If you compare the examples in the source tree (`src/cc8/examples`) to
those with uppercased versions of those same names on the OS/8 `DSK:`
volume, you’ll notice that these `#include` statements were stripped
out. This is [necessary](#os8pp); the linked documentation tells you why
and how the OS/8 version of CC8 gets away without a `#include` feature.

The tool that strips these `#includes` out for us is called
`bin/cc8-to-os8`, which you might find useful if you’re frequently
working with programs that need to work under both compilers.


<a id="native" name="os8"></a>
## The Native OS/8 Compiler

This compiler is supplied in both source and binary forms as part of the
PiDP-8/I software distribution.

The native CC8 compiler is built from source code to SABR assembly by
the CC8 cross-compiler unless you pass `--disable-os8-cc8` to the
PiDP-8/I distribution’s `configure` script to suppress it. Those SABR
files are then copied to a virtual DECtape image, which is attached to
the PDP-8 simulator, assembled, and linked to produce the CC8 native
OS/8 compiler by the [`cc8-tu56.os8`][cctu] script run by
[`os8-run`](./os8-run.md). Take a look at that script and the `os8-run`
docs if you want to learn more about this process.

[cctu]: /file?fn=media/os8/scripts/cc8-tu56.os8

Because the CC8 native is compiled *by* CC8, the [standard memory
layout](#memory) applies to the compiler itself. Among other things,
this means each phase requires approximately 16&nbsp;kWords of core.

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

1.  `c8.c` &rarr; `c8.sb` &rarr; `CC.SV`: The compiler driver: accepts
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
### Features of the Native OS/8 Compiler

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

[fib]: /doc/src/cc8/examples/fib.c


<a id="nlim" name="limitations"></a>
### Known Limitations of the OS/8 CC8 Compiler

The OS/8 version of CC8 is missing many language features relative to
[the cross-compiler](#cross), and much more compared to modern C.

1.  <a id="typeless"></a>The language is typeless in that everything is
    a 12 bit integer and any variable/array can interpreted as `int`, `char`
    or pointer.  All variables and arrays must be declared as `int`.
    The return type may be left off of a function's definition; it is
    implicitly `int` in all cases, since `void` is not supported.

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

4.  <a id="os8pp"></a>Unlike the CC8 cross-compiler, the OS/8 compiler
    has no functional implementation for any C preprocessor directive:
    `#define`, `#ifdef`, `#include`, etc.  This even includes [inline
    assembly](#asm) via `#asm`!

    It is important to realize that there is a *start* at a native
    preprocessor in `src/cc8/os8/c8.c`, but it’s pretty much [broken and
    useless at the moment][os8pre], which means if you try to make use
    of it, the compiler will likely do stupid or wrong things. 
    
    This means you cannot use `#include` directives to string multiple
    C modules into a single program.

    It also means that if you take a program that the cross-compiler
    handles correctly and just copy it straight into OS/8 and try to
    compile it, it probably still has the `#include <libc.h>` line and
    possibly one for `init.h` as well. *Such code will fail to compile.*
    You must strip such lines out when copying C files into OS/8.

    (The native compiler emits startup code automatically, and it
    hard-codes the LIBC call table in the `CC2` compiler stage,
    implemented in `p8.c`, so it doesn’t need `#include` to make these
    things work.)

5.  Variables are implicitly `static`, even when local.

6.  Arrays may only be single indexed. See `PS.C` for an example.

7.  The compiler does not yet understand how to assign a variable's
    initial value as part of its declaration. This:

        int i = 5;

    must instead be:

        int i;
        i = 5;

8.  There is no `&&` nor &brvbar;&brvbar;, nor are there plans to add
    them in the future.  Neither is there support for complex relational
    operators like `>=` nor even `!=`.  Abandon all hope for complex
    assignment operators like `+=`.

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

10. The stack, which includes all globals and literals, is only 4&nbsp;kWords.
    Stack overflow is not detected.  Literals are inlcuded in this due
    to a limitation in the way `COMMN` is implemented in SABR.

11. There is no argument list checking, not even for standard library
    functions.

12. `do/while` loops are parsed, but the code is not properly generated.
    Regular `while` loops work, as does `break`, so one workaround for a
    lack of `do/while` is:

        `while (1) { /* do something useful */; if (cond) break; }

    We have no intention to fix this.

13. `switch` doesn't work.

The provided [LIBC library functions](#lib) is also quite limited and
nonstandard compared to Standard C.  See the documentation for each
individual library function for details.

[os8pre]: /tktview/4a1bf30628


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
PDP-8 architecture, we only have about 3&nbsp;kWords to construct the parse
result, for example.

In addition, the native OS/8 compiler is severely limited in code space,
so it does not understand the full C language. It is less functional
than K&R C 1978; we do not have a good benchmark for what it compares to
in terms of other early C dialects, but we can sum it up in a single
word: primitive.

Nonetheless, our highly limited C dialect is Turing complete. It might
be better to think of it as a high-level assembly language that
resembles C rather than as "C" proper.


## <a id="libc"></a>The CC8 C Library: Documentation

In this section, we will explain some high-level matters that cut across
multiple functions in the C library. This material is therefore not
appropriate to repeat below, in the [C library function
reference](#libref).


### <a id="ctype"></a>ctype

The ISO C Standard does not define what the `is*()` functions do when
the passed character is not representable as `unsigned char`. Since this
C compiler does not distinguish types, our `is*()` functions return
false for any value outside of the ASCII range, 0-127.


### <a id="stdio"></a>stdio

The stdio implementation currently assumes US-ASCII 7-bit text I/O.

Input characters have their upper 5 bits masked off so that only the
lower 7 bits are valid in the returned 12 bit PDP-8 word. Code using
[`fgetc`](#fgetc) cannot be used on arbitrary binary data because its
“end of file” return case is indistinguishable from reading a 0 byte.

The output functions will attempt to store 8-bit data, but since you
can’t read it back in safely with this current implementation, per
above, you should only write ASCII text to output files with this
implementation. Even if you are reading your files with some other code
which is capable of handling 8-bit data, there are further difficulties
such as a lack of functions taking an explicit length, like `fwrite()`,
which makes dealing with ASCII NUL difficult. You could write a NUL to
an output file with `fputc()`, but not with `fputs()`, since NUL
terminates the output string.

The stdio implementation only allows one file to be open at a time for
reading and one for writing. Since there is no input buffering to speak
of in the stdio implementation, there is no need to close files opened
for input, there being no resources to free. Together, those two facts
explains why [the `fclose()` implementation](#fclose) takes no argument:
it always closes the lone output file, if any.

This means that to open multiple output files, you have to `fclose` each
file before calling [`fopen("FILENA.ME", "w")`](#fopen) to open the next.
To open multiple input files, simply call `fopen()` to open each
subsequent file, implicitly closing the prior input file.

The CC8 LIBC file I/O implementation is built atop the OS/8 FORTRAN II
subsystem’s file I/O functions. This has some important implications in
the documentation below:

1.  We have not tried to “hoist” descriptions of what the FORTRAN II
    subsystem does up into this documentation. It may be necessary for
    you to read the FORTRAN II documentation in the OS/8 manual to
    understand how the CC8 LIBC’s stdio file I/O functions behave in
    certain respects.

2.  Programs built with CC8 which use its file I/O functions are
    dependent upon OS/8.


#### Ctrl-C Handling

Unlike on modern operating systems, there is nothing like `SIGINT` in
OS/8, which means Ctrl-C only kills program that explicitly check for
it.  The keyboard input loop in the CC8 LIBC standard library does do
this.

The thing to be aware of is, this won’t happen while a program is stuck
in an infinite loop or similar. The only way to get out of such a
program is to either restart OS/8 — assuming the broken program hasn’t
corrupted the OS’s resident parts — or restart the PDP-8.

(You can restart OS/8 by causing a jump to core memory location 07600.
Within the `pidp8i` environment, you can hit Ctrl-E, then say “`go
7600`”.  From the front panel, press the Stop key, toggle 7600 into the
switch register, press the Load Add key, then press the Start key.)


### <a id="wordstr"></a>Strings are of Words, Not of Bytes or Characters

In several places, the Standard says a conforming C library is supposed
to operate on “bytes” or “characters,” at least according to [our chosen
interpretation][cppr]. Except for the text I/O restrictions called out
[above](#stdio), LIBC operates on strings of PDP-8 words, not on this
more modern notion of fixed 8-bit bytes the nebulous concept of
“characters.”

Because someone may be used to the idea that string and memory functions
like [`memcpy()`](#memcpy) and [`strcat()`](#strcat) will operate on
bytes, we’ve marked all of these cases with a reference back to this
section.

By the same token, most functions that operate on NUL-terminated string
buffers in a conforming C library implementation actually check for 0
words in this implementation. The distinction is that these routines
aren’t carefully masking off the top 4 or 5 bits to check *only* against
a 7- or 8-bit NUL character, they’re checking for a word equal to zero.

This is another manifestation of [CC8’s typeless nature](#typeless).


### <a id="ptrwrap"></a>Pointers Wrap Around

Pointers in this C implementation are generally [confined to core memory
field 1](#memory), the user data field.  That is to say, the code
generated by CC8 does not use 15-bit extended addresses; it just flips
page depending on what type of data or code it’s trying to access.

This means that if you iterate a pointer past the end of a 4096 word
core memory field, it will become 0 and continue blithely along. This
affects a great many of the LIBC functions’ behavior. LIBC generally
does not try to check for such wraparound problems, much less signal
errors when it happens. The programmer is expected to avoid doing this.

Keep in mind that not all of field 1 belongs to the programmer.
Scribbling on this field is likely to cause great havoc, including data
loss, system crashes, and more.

All of this means that code that operates on pointers will generally
only do its work within the user data field. You will likely need to
resort to [inline assembly](#asm) and `CIF`/`CDF` instructions to escape
that field. Getting our [LIBC](#libc) to operate on other fields may be
tricky or even more difficult than it’s worth.

On the bright side, pointers are always 12-bit values, accessed with
indirect addressing, rather than page-relative 7-bit addresses, so that
programs built with CC8 need not concern themselves with [page
boundaries][memadd].


### <a id="missing"></a>Missing Functions

The bulk of the Standard C Library is not provided, including some
functions you’d think would go along nicely with those we do provide,
such as `feof()` or `fseek()`.  Keep in mind that the library is
currently restricted to [a single 4&nbsp;kWord field](#memory), and we
don’t want to lift that restriction. Since the current implementation
nearly fills that space, it is unlikely that we’ll add much more
functionality beyond the currently provided 33 functions. If we ever fix
any of the limitations we’ve identified below, consider it “gravy”
rather than any kind of obligation fulfilled.  Do not bring your modern
C environment expectations to CC8!

Some of these missing functions are less useful in the CC8 world than in
more modern C environments. The low-memory nature of this world
encourages writing loops over [word strings](#wordstr) in terms of
pointer arithmetic and implicit zero testing (e.g. `while (*p++) { /*
use p */ }`) rather than make expensive calls to `strlen()`, so that
function isn’t provided.


## <a id="libref"></a>The CC8 C Library: Reference

CC8 offers a very limited standard library, which is shared between the
native and cross-compilers. While some of its function names are the
same as functions defined by Standard C, these functions generally do
not conform completely to any given standard due to the severe resource
constraints imposed by the PDP-8 architecture. This section of the
manual documents the known limitations of these functions relative to
[the current C standard as interpreted by `cppreference.com`][cppr], but
it is likely that we have overlooked corner cases that our library does
not yet implement.  When in doubt, [read the source][libcsrc].

[The LIBC implementation][libcsrc] is currently stored in the same
source tree directory as the native compiler, even though it’s shared
between the two compilers. This is because the two compilers differ only
from the code generation layer up: if you cross-compile a C program with
`bin/cc8`, you must still *assemble and link* it under OS/8, which means
using the `LIBC.RL` binary produced for use by the native compiler.

Contrast [the `libc.h` file][libch] which is symlinked or copied
everywhere it needs to be. This is because neither version of CC8 has
the notion of an include path. This file must therefore be available in
the same directory as each C file that uses it.

In the following text, we use OS/8 device names as a handwavy kind of
shorthand, even when the code would otherwise run on any PDP-8 in
absence of OS/8. Where we use “`TTY:`”, for example, we’d be more
precise to say instead “the console teleprinter, being the one that
responds to [IOT device code][iot] 3 for input and to device code 4 for
output.” We’d rather not write all of that for every stdio function
below, so we use this shorthand.

[cppr]:    https://en.cppreference.com/w/c
[libch]:   /doc/trunk/src/cc8/include/libc.h
[libcsrc]: /doc/trunk/src/cc8/os8/libc.c

Functions which are either not yet completely documented or which we
have not yet checked for conformance to any particular standard are
marked **DOCUMENTATION INCOMPLETE**.


### <a id="atoi"></a>`atoi(s, outlen)`

Takes a null-terminated ASCII character string pointer `s` and returns a
12-bit PDP-8 two’s complement signed integer. The length of the numeric
string is returned in `*outlen`.

**Standard Violations:**

*   Skips leading ASCII 32 (space) characters only, not those matched by
    [`isspace()`](#isspace), as the Standard requires.

*   The `outlen` parameter is nonstandard.


### <a id="cupper"></a>`cupper(p)`

Implements this loop more efficiently:

    char* tmp = p;
    while (*tmp) {
        *tmp = toupper(*tmp);
        ++tmp;
    }

That is, it does an in-place conversion of the passed [0-terminated word
string](#wordstr) to all-uppercase.

This function exists in LIBC because it is useful for ensuring that file
names are uppercase, as OS/8 requires. With the current CC8 compiler
implementation, the equivalent code above requires 24 more instructions
than calling `cupper()` instead, best-case! That means a single call
converted from a loop around [`toupper()`](#toupper) to a `cupper()`
call more than pays for the 21 instructions and one extra jump table
slot this function requires in LIBC.

Do not depend on the return value. There is a predictable mapping, but
it has no inherent meaning, so we are not documenting that mapping here.
If CC8 had a “`void`” return type feature, we’d be using that here.

**Nonstandard.** No known analog in any other C library.


### <a id="dispxy"></a>`dispxy(x,y)`

Plot a point at coordinate (x,y) on a [VC8E point-plot display][vc8e].

This is the display type assumed by the PiDP-8/I Spacewar!
implementation. There were many other display types designed for and
sold with PDP-8 family computers, which generally used different IOT
codes. If you’re trying to control something other than a VC8E, you
might want to replace this routine’s internals rather than code a
separate implementation, leading to wasted space in your LIBC.

**Nonstandard.**

[dixy]: http://homepage.divms.uiowa.edu/~jones/pdp8/man/vc8e.html
[vc8e]: https://homepage.divms.uiowa.edu/~jones/pdp8/man/vc8e.html


### <a id="exit"></a>`exit(ret)`

Exits the program.

This function is implemented in terms of the [FORTRAN II library
subroutine `EXIT`][f2exit], which in the OS/8 implementation simply
returns control to the OS/8 keyboard monitor.

If `EXIT` returns for any reason, LIBC halts the processor.

**Standard Violations:**

*   The passed return code is ignored, there being no such thing as a
    program’s “status code” in OS/8.

*   There is no `atexit()` mechanism in the CC8 LIBC.

[f2exit]: https://archive.org/details/bitsavers_decpdp8os8_39414792/page/n702


### <a id="fclose"></a>`fclose()`

Closes the currently-opened output file.

This function simply calls the OS/8 FORTRAN II library subroutine
[`OCLOSE`][f2fio].

**Standard Violations:**

*   Does not take a `FILE*` argument.  (See [`fopen()`](#fopen) for
    justification.)

*   Always closes the last-opened *output* file, only, there being
    [no point](#stdio) in explicitly closing input files in this
    implementation.

[f2fio]: https://archive.org/details/bitsavers_decpdp8os8_39414792/page/n700


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

Opens OS/8 file `DSK:name.DA`.

The `name` parameter must point to at most six 0-terminated uppercase
characters, [one character per word](#wordstr).  (See
[`cupper()`](#cupper).)

The file is opened for reading if `mode` points to an ”`r`” character,
and it is opened for writing if `mode` points to a “`w`” character. This
need only point to a single character, since only that one memory
location is ever referenced. No terminator is required.

The OS/8 device name and file name extension are hard-coded, the former
by the current `fopen()` implementation and the latter by the OS/8
FORTRAN II [`IOPEN` and `OOPEN`][f2fio] subroutines that `fopen()` is
implemented in terms of. This means there is currently no way to use
this `stdio` implementation to read from or write to files on OS/8
devices other than `DSK:` or to files with extensions other than `.DA`.

**Standard Violations:**

*   Does not return a `FILE*`. Functions which, in Standard C, take a
    `FILE*` argument do not do so in the CC8 LIBC, because there can be
    only one opened input file and one opened output file at a time, so
    the file that is meant is implicit in the call.

    This also means `fopen()` has no way to signal a failure to open the
    requested file name!  ...Which is just as well, since there is also
    no `ferror()` or `errno` in our LIBC.

*   Does not accept the standard mode `a`, for append.  Since there is
    also no `fseek()` in CC8’s LIBC, a preexisting file named for
    writing is always overwritten.

*   Does not accept the standard `+` modifier to combine read/write
    modes: files are only readable or only writeable under this
    implementation.  Neither is it possible to give “`rw`”, the
    nonstandard but widely supported way to specify “open for
    read/write”.

*   Does not support the `b` modifier for binary I/O: files are assumed
    to contain ASCII text only.

*   Does not diagnose null pointers as required by the Standard: it will
    probably do something silly like reference [core memory location 0 in
    field 1](#memory), then return without having done anything useful,
    causing the subsequent I/O calls on that file to fail.


### <a id="fprintf"></a>`fprintf(fmt, args...)`

Writes its arguments (`args`...) to the currently-opened output file
according to format string `fmt`.

Returns the number of characters written to the output file.

This function is just a simple wrapper around [`printf()`](#printf)
which sets a flag that causes `printf()` to write its results to the
current output file instead of `TTY:`, so you must read its
documentation to fully understand `fprintf()`. Since `printf()` is in
turn based on [`sprintf()`](#sprintf), you must read that function’s
documentation as well.

**Standard Violations:**

*   `fprintf` does not take a `FILE*` pointer as its first argument. It
    simply writes to the one and only output file that can be opened at
    a time by [`fopen()`](#fopen).

*   File I/O errors are not diagnosed.


### <a id="getc" name="fgetc"></a>`getc()`, `fgetc()`

Reads a single ASCII character from `TTY:` or from the last file opened
for input by [`fopen()`](#fopen), respectively.

**Standard Violations:**

*   Returns ASCII NUL (0) to signal EOF, not an implementation-defined
    out-of-range EOF constant.  (Most commonly -1 in other C library
    implementations.)  Since there is no `feof()` function in CC8 LIBC
    to disambiguate the cases, this function cannot safely be called for
    files that could contain a 0 byte, since it will result in a false
    truncation.


### <a id="gets"></a>`gets(s)`

Reads a string of ASCII characters from `TTY:`, up to and including the
terminating CR character, storing it at core memory location `s`, and
following the terminating CR with a NUL character.

Backspace characters from the terminal remove the last character from
the string.

Returns the passed string pointer on success.

**Standard Violations:**

*   Cannot return 0 for “no input” as Standard C requires: always
    succeeds.


### <a id="isalnum"></a>`isalnum(c)`

Returns nonzero if either [`isdigit()`](#isdigit) or
[`isalpha()`](#isalpha) returns nonzero for `c`.

**Standard Violations:**

*   Does not know anything about locales; assumes US-ASCII input.


### <a id="isalpha"></a>`isalpha(c)`

Returns nonzero if the passed character `c` is either between 65 and 90
or between 97 and 122 inclusive, being the ASCII alphabetic characters.

**Standard Violations:**

*   Does not know anything about locales; assumes US-ASCII input.


### <a id="isdigit" name="isnum"></a>`isdigit(c)`, `isnum(c)`

Returns nonzero if the passed character `c` is between 48 an 57,
inclusive, being the ASCII decimal digit characters.

**Standard Violations:**

*   `isnum` is a nonstandard alias for `isdigit` conforming to no other
    known C library implementation.  Both are implemented with the same
    LIBC code.

*   Does not know anything about locales; assumes US-ASCII input.


### <a id="isspace"></a>`isspace(c)`

Returns nonzero if the passed character `c` is considered a “whitespace”
character.

This function is not used by `atoi`: its whitespace matching is
hard-coded internally.

**Standard Violations:**

*   Whitespace is currently defined as ASCII 1 through 32, inclusive.
    Yes, this is a *vast* overreach.


### <a id="itoa"></a>`itoa(num, str)`

Convert a 12-bit PDP-8 two’s complement integer `num` to an [ASCII word
string](#wordstr) in memory pointed to by `str`.

If `num` is an arbitrary integer, `str` should point to 6 words of
memory to cover the worst-case condition, e.g. "-2048\\0".

There is no thousands separator in the output string.

**Nonstandard.** The `itoa()` function in the [Visual C++][itoam] and
[Embarcadero C++][itoae] environments takes a third parameter, the
radix.

[itoae]: http://docs.embarcadero.com/products/rad_studio/radstudio2007/RS2007_helpupdates/HUpdate4/EN/html/devwin32/itoa_xml.html
[itoam]: https://docs.microsoft.com/cpp/c-runtime-library/reference/itoa-itow


### <a id="kbhit"></a>`kbhit()`

Returns nonzero if `TTY:` has sent an input character that has not yet
been read, which may then be read by a subsequent call to
[`getc()`]((#getc). Returns 0 otherwise.

This function’s intended purpose is to let the program do work while
waiting for keyboard, since calling `getc()` before input is available
would block the program.

**Nonstandard.** Emulates a function common in DOS C libraries or those
descended from them, such as [Embarcadero C++][kbhite] and [Visual
C++][kbhitm].

[kbhite]: http://docs.embarcadero.com/products/rad_studio/radstudio2007/RS2007_helpupdates/HUpdate4/EN/html/devwin32/kbhit_xml.html
[kbhitm]: https://docs.microsoft.com/cpp/c-runtime-library/reference/kbhit


### <a id="memcpy"></a>`memcpy(dst, src, n)`

Copies `n` words from core memory location `src` to `dst` in the [user
data field](#memory).

Beware that the copy will [wrap around](#ptrwrap) to the beginning of
the field if either `src+n` or `dst+n` &ge; 4096.

The `dst` buffer can safely overlap the `src` buffer only if it is at a
lower address in memory. (Note that there is no `memmove()` in this
implementation.)

Uses autoincrement registers 12₈, 13₈, and 14₈.

**Standard Violations:**

*   Returns 0 instead of the `dst` pointer as required by the Standard.
    A NULL return is specified as a failure condition by the Standard.

    This function has no internally-detected failure cases, so there is
    no ambiguity in the meaning of the return value.


### <a id="memset"></a>`memset(dst, c, len)`

Sets a run of `len` core memory locations starting at `dst` to `c`.

Beware that this function will [wrap around](#ptrwrap) if `dst+len-1`
&ge; 4096.

**Standard Violations:**

*   Returns 0 instead of the `dst` pointer as required by the Standard.

*   This function has no internally-detected failure cases, so the
    Standard’s requirement that this function return 0 in error cases
    means there is no ambiguity in the meaning of the return value.

    If we ever fix the prior violation, there will still be no ambiguity
    with the error case since [a valid pointer in CC8 cannot be
    zero](#memory).


### <a id="printf" name="fprintf"></a>`printf(fmt, args...)`

Writes its arguments (`args`) formatted according to format string `fmt`
to `TTY:`.

This function is implemented in terms of [`sprintf()`](#sprintf), so see
its documentation for further details.

**WARNING:** Because `printf()` is implemented in terms of `sprintf()`
and it points at a static buffer high up in field 1, which has other
data following it, you can only print up to *64* bytes at a time with
`printf()`. Printing more will first overwrite data precious to either
SABR or the FORTRAN II subsystem, and then it will wrap around and begin
stomping on the start of field 1.


### <a id="puts" name="fputs"></a>`puts(s)`, `fputs(s)`

Writes a null-terminated character string `s` either to `TTY:` or to the
currently-opened output file.

The characters pointed to are expected to be 7-bit ASCII bytes stored
within each PDP-8 word, with the top 5 bits unset.

**Standard Violations:**

*   The `puts()` implementation does not write a newline after the
    passed string.

    (Neither does our `fputs()`, but that’s actually Standard behavior.)

*   Both `puts()` and `fputs()` are supposed to return nonzero on
    success, but this implementation returns 0.
    
    Technically, these functions aren’t explicitly “returning” anything,
    they’re just leaving 0 in AC, that being the ASCII NUL character
    that terminated the loop inside each function’s implementation.

*   `fputs()` detects no I/O error conditions, and thus cannot return
    EOF to signal an error. It always returns 0, whether an error
    occurred or not.

*   `fputs()` does not take a `FILE*` as its first parameter due to the
    [implicit single output file](#stdio).


### <a id="revcpy"></a>`revcpy(dst, src, n)`

For non-overlapping buffers, has the same effect as
[`memcpy()`](#memcpy), using less efficient code.

Because it copies words in the opposite order from `memcpy()`, you may
be willing to pay its efficiency hit when copying between overlapping
buffers when the destination follows the source.

**Nonstandard.** Conforms to no known C library implementation.


### <a id="sprintf"></a>`sprintf(outstr, fmt, args...)`

Formats its arguments (`args`) for output to `outstr` based on format
string `fmt`.

The allowed standard conversion specifiers are `%`, `c`, `d`, `o`, `s`,
`u`, and `x`.  See your favorite C manual for their meaning.

The CC8 LIBC does support one nonstandard conversion specifier, `b`,
meaning binary output. Think of it like `x`, but in base 2.

The `d` specifier is implemented in terms of [`itoa()`](#itoa). The `b`,
`o`, `u`, and `x` specifiers are implemented in terms of the
unsigned-only internal helper function at the core of `itoa()`.

Left and right-justified padding is supported. Space and zero-padding
is supported.

Width prefixes are obeyed.

Precision specifiers are parsed but have no effect on the output.
**TODO**: Claim based on code inspection; verify with tests.

Returns the number of characters written to the output stream, not
including the trailing NUL character.

**WARNING:** This function does not check its buffer pointer for
end-of-field, so if you cause it to print more than can be stored at the
end of a field, it will wrap around and begin writing at the beginning
of the same field. This also has effects on the behavior of
[`printf()`](#printf) and [`fprintf()`](#fprintf).

**Standard Violations:**

*   The `x` specifier is supposed to result in lowercase hexadecimal
    output, but it gives uppercase A-F for the upper 6 hex digits. It
    therefore behaves like `%X` in conforming implementations.

*   As long as CC8 has no floating-point support, the `a`, `e`, `f`, and
    `g` format specifiers (and their capitalized variants) cannot be
    supported.

*   The standard `n` and `p` format specifiers could be supported, but
    currently are not.

*   The `i` alias for the more common `d` specifier is not supported.

*   Unsupported input specifiers cause the function to return the number
    of characters written so far, not a negative value as the Standard
    requires.  In the case of `sprintf()`, this means the trailing NUL
    character will not be written!

*   There is no `snprintf()`, `vprintf()`, etc.


### <a id="sscanf"></a>`sscanf`

Reads formatted input from a file.

**Standard Violations:**

*   `[f]scanf()` is not provided. Call [`[f]gets()`](#gets) to get a
    string and then call `sscanf()` on it.

*   This list cannot possibly be complete.

**DOCUMENTATION INCOMPLETE**


### <a id="strcat"></a>`strcat(dst, src)`

Concatenates one [0-terminated word string](#wordstr) to the end of
another in the same data field, [normally field 1](#memory).

This function will not copy data between fields.

If the terminating 0 word is not found in `dst` by the end of the
current field, it will wrap around to the start of the field and resume
searching there; the concatenation will occur wherever it does find a 0
word. If there happen to be no 0 words in the field, it will iterate
forever!

Beware that this function will [wrap around](#ptrwrap) if
`dst + strlen(dst) + strlen(src)` &ge; 4096 and stomp on whatever’s
at the start of the field.

These are not technically violations of Standard C as it leaves such
matters undefined.

Returns a copy of `dst`.

**Standard Violations:**

*   None known.


### <a id="strcpy"></a>`strcpy(dst, src)`

Copies one [0-terminated word string](#wordstr) to another memory
location in the same data field, [normally field 1](#memory).

This function will not copy data between fields.

Beware that this function will [wrap around](#ptrwrap) if either
`src+strlen(src)` or `dst+strlen(dst)` &ge; 4096.

The `dst` buffer can safely overlap the `src` buffer only if it is at a
lower address in memory.

Unlike [`memcpy()`](#memcpy) the current implementation does not use any
autoincrement registers.

**Standard Violations:**

*   Returns 0, not a copy of `dst` as the Standard requires.


### <a id="strstr"></a>`strstr(haystack, needle)`

Attempts to find the first instance of `needle` within `haystack`, which
are [0-terminated word strings](#wordstr). This function’s behavior is
undefined if either buffer is not 0-terminated.

The implementation uses the [naïve string search algorithm][nssa], so
the typical execution time is O(n+m), but the worst case time is
&Theta(nm). Don’t go expecting us to buy execution speed with
preprocessing steps as with [BMH][bmh] or [KMP][kmp]!

Both the `haystack` and `needle` buffer pointers are offsets within the
current data field, [normally field 1](#memory).

Beware that this function will [wrap around](#ptrwrap) if either
`haystack+strlen(haystack)` or `needle+strlen(needle)` &ge; 4096,
continuing the search or match (respectively) from that point.

**Returns:**

*   *a pointer to the first needle*, if one is found within the haystack

*   *zero* if either no needle is in the haystack, *or* the haystack is
    zero-length (i.e. `*haystack == '\0'`), *or* the needle is bigger
    than the haystack

**Standard Violations:**

*   None known.

[bmh]:  https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore%E2%80%93Horspool_algorithm
[kmp]:  https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
[nssa]: https://en.wikipedia.org/wiki/String-searching_algorithm#Na%C3%AFve_string_search


### <a id="toupper"></a>`toupper(c)`

Returns the uppercase form of ASCII character `c` if it is lowercase,
Otherwise, returns `c` unchanged.

**Standard Violations:**

*   There is no `tolower()` in the CC8 LIBC.

*   Does not know anything about locales; assumes US-ASCII input.


### <a id="xinit"></a>`xinit`

??

**Nonstandard.** Conforming to...?

**DOCUMENTATION INCOMPLETE**

[iot]: /wiki?name=IOT+Device+Assignments


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

[pce]: /wiki?name=PEP001.C


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


## <a id="memory"></a>Memory Model

The OS/8 FORTRAN II linking loader (`LOADER.SV`) determines the core
memory layout for the built programs.  Most commonly, it uses this
scheme:

**Field 0:** FORTRAN library utility functions and OS/8 I/O system

**Field 1:** The user data field: runtime stack, globals, and literals

**Field 2:** The program's executable code

**Field 3:** The LIBC library code

The first page of field 1 is currently unused. That means it is not
possible to have a valid pointer declared either as a C global or on the
stack with value 0000₈. This has practical positive consequences such as
the fact that you can depend on a call to [`gets()`](#gets) to always
return a truthy value provided you pass it a normal C pointer.  If you
hand-craft a pointer that happens to point to the first core memory
location in a field, which is therefore confusable with `NULL`, then on
your head be the consequences!

LIBC uses zero page memory locations 147₈ through the end of the page.
Functions which use the autoincrement locations 10₈ through 17₈ are
so-documented above.

For more on this topic, see the companion article [PDP-8 Memory
Addressing][memadd].

[memadd]: /wiki?name=PDP-8+Memory+Addressing


<a id="asm"></a>
## Inline Assembly Code

The [cross-compiler](#cross) — and not the [native compiler!](#os8pp) —
allows [SABR][sabr] assembly code between `#asm` and `#endasm` markers
in the C source code:

    #asm
        TAD (42      / add 42 to AC
    #endasm

Such code is copied literally from the input C source file into the
compiler’s SABR output file, so it must be written with that context in
mind.


### <a id="calling"></a>The CC8 Calling Convention

You can write whole functions in inline assembly, though for simplicity,
we recommend that you write the function wrapper in C syntax, with the
body in assembly:

    add48(a)
    int a
    {
        a;          /* load 'a' into AC; explained below */
    #asm
        TAD (48
    #endasm
    }

Doing it this way saves you from having to understand the way the CC8
software stack works, which we’ve chosen not to document here yet, apart
from [its approximate location in core memory](#memory). All you need to
know is that parameters are passed on the stack and *somehow* extracted
when they’re referenced in C code.

CC8 returns values from functions in AC, so our example does not require
an explicit “`return`” statement: we’ve arranged for our intended return
value to be in AC at the end of the function body, so the implicit
return does what we want here.

The above snippet therefore declares a function `add48` taking a single
parameter “`a`” and returning `a+48`.

Keep in mind when reading such code that CC8 is [essentially
typeless](#typeless): it’s tempting to think of the above code as taking
an integer and returning an integer, but you can equally correctly think
of it as taking a character and returning a character. Indeed, that
function will take a value in the range 0 thru 9 and return the
equivalent ASCII digit! CC8’s typeless nature mates well with K&R C’s
indifference toward return type declaration.


### Equivalence to Statements

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


### Optimization

There are several clever optimizations in the examples above that you
might want to use in your own programs:

*   In the `add42` example the line “`a;`” means “load `a` into AC”. In
    a Standard C compiler, this would be considered use of a variable in
    `void` context and thus be optimized out, but K&R C has no such
    notion, so it has this nonstandard meaning in CC8.  This technique
    is used quite a lot in our [LIBC](#libc), so you can be sure the
    behavior won’t be going away.

*   Knowing that functions return their value in AC, you can call
    another C function from the middle of a block of assembly code but
    never store its return value explicitly: just use its return value
    directly from AC.

*   In the `puts` example, the statement `*p++` implicitly stores the
    value at the core memory location referred to by `p` in AC, so we
    can use it within the assembly body of that loop without ever
    explicitly referring to `p`.

It is worth reading the SABR output from the compiler to discover tricks
such as these.

Beware that CC8 isn’t a particularly smart compiler. It will not even
try to pull of many of the automatic tricks you’d expect from a modern C
compiler. The bulk of the CC8 optimizer’s code is located in its user’s
brain, at the moment.


### <a id="linkage" name="varargs"></a>Library Linkage and Varargs

CC8 has some non-standard features to enable the interface between the
main program and the C library. This constitutes a compile time linkage
system to allow for standard and vararg functions to be called in the
library.

**TODO:** Explain this.


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
