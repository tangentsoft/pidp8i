This directory contains a very simple BASIC interpreter that can be
compiled using the CC8 cross-compiler to create a SABR file, which can
be assembled under the PiDP-8/I OS/8 environment per the instructions in
[the top-level README][/doc/trunk/cc8/README.md] file.

With the native OS/8 compiler installed in a running OS/8 instance, this
will assemble and run the BASIC interpreter:

    .COMP BASIC.SB
    .R LOADER
    BASIC,LIBC/G

And off you go...
