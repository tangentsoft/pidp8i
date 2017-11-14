# Using the Examples

This directory contains several example programs. We will use the
`calc.c` example throughout this section.

The program may be compiled using the cc8 cross-compiler to SABR sources
like so:

    $ cc8 calc.c

You can then use the `txt2ptp` program to turn the resulting `calc.s`
file into a paper tape to be loaded into OS/8, or you can copy and paste
the SABR text into a text editor or PIP session on the OS/8 side. See
the [assembly examples' `README.md` file][aerm] or the [U/W FOCAL manual
supplement][uwfs] for ideas on how to get program text into OS/8.

You then assemble and load the programs on the OS/8 side with:

    .COMP CALC.SB
    .R LOADER
    CALC,LIBC/G

You need to append the `/I/O` flags to the final command if your program
uses file I/O.

You can either hit <kbd>Enter<kbd> at the end of the final command above
to load, link, and run your program immediately, or you can hit
<kbd>Escape</kbd> to drop back into OS/8 with the program simply loaded
into core memory, then save a core image of it with:

    .SAVE CALC.SV

The latter then lets you run the program again with a shorter command:

    .R CALC

I recommend that you compile C programs on the OS/8 side using the
preprocessor front-end rather than the method given in the [top-level
`README.md` file][tlrm]:

    .R CC0
    >CALC.CC

That should result in `CC.SB`, which you feed back into the `COMP`
sequence above.


[aerm]: /doc/trunk/examples/README.md
[tlrm]: /doc/trunk/src/cc8/README.md
[uwfm]: /doc/trunk/doc/uwfocal-manual-supp.md


# The Examples

In order of complexity, they are:

## calc.c

This is a simple 4-function calculator.


## ps.c

This prints [Pascal's triangle][pt].

[pt]: https://en.wikipedia.org/wiki/Pascal%27s_triangle


## fib.c

This program calculates [Fibonacci humbers][fn], which implicitly
demonstrates the C compiler's ability to handle [recursion][rec].

[fn]:  https://en.wikipedia.org/wiki/Fibonacci_number
[rec]: https://en.wikipedia.org/wiki/Recursion_(computer_science)


## basic.c

A very simple BASIC interpreter. This program tests a broad swath of the
compiler's functionality.
