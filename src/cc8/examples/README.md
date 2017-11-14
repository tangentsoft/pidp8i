# Using the Examples

This directory contains several example programs. We will use the
`calc.c` example throughout this section.

The program may be compiled using the cc8 cross-compiler to SABR sources
like so:

    $ cc8 calc.c

You can then use the `txt2ptp` program to turn the resulting `calc.s`
file into a paper tape to be loaded into OS/8:

    $ txt2ptp < calc.s > calc.pt
    $ pidp8i             ⇠ ...start PDP-8 sim somehow, then hit Ctrl-E
    sim> att -r dt0 calc.pt
    sim> cont
    .R PIP
    *CALC.SB<PTR:        ⇠ hit Enter, Ctrl-Z, then Escape

See the [assembly examples' `README.md` file][aerm] or the [U/W FOCAL
manual supplement][uwfs] for more ideas on how to get text files like
this SABR file into OS/8.

However you manage it, you can then assemble, load, and run the programs
on the OS/8 side with:

    .COMP CALC.SB
    .R LOADER
    *CALC,LIBC/G/I/O     ⇠ press Esc to execute command and exit LOADER

The `/I/O` flags say this program does both input and output. Each of
these flags you add costs 3 pages of core, so specify only what you
need!

The `/G` flag causes the loader to run the linked program immediately,
but once you're done modifying the program, you probably want to save it
as a core image so it can be run directly instead of being linked and
loaded again each time. You can give `/M` instead, which finalizes the
link and then exits, printing a map file before it does so. You can then
save the result where the OS/8 `R` command can find it with:

    .SAVE SYS:CALC

That produces `SYS:CALC.SV`, which an `R CALC` command will load and
run.

If you wish to compile from C source code on the OS/8 side rather than
cross-compile, I recommend using the `CC0` front-end rather than the
method shown in the [top level `README.md` file][tlrm] involving the
`CC1` stage:

    .R CC0

    >calc.cc

    .COMP CC.SB

    .R LOADER
    *CC,LIBC/M/I/O

Notice that the front-end processor produces `CC.SB`, not `CALC.SB` as
you might be expecting. This is where the `CC` comes from in the `COMP`
and `LOADER` commands.

Note that `CC0` tolerates lowercase input.


[aerm]: /doc/trunk/examples/README.md
[tlrm]: /doc/trunk/src/cc8/README.md
[uwfs]: /doc/trunk/doc/uwfocal-manual-supp.md


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
