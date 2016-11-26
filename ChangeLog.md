Release 2016.11.26 (UNRELEASED)

*   Added an intelligent, powerful build system, replacing the
    bare-bones `Makefile` build system in the upstream version.

    This means you don't unpack the zip file or tarball into
    `/opt/pidp8` and build it in place, as with the upstream version.
    Instead, you unpack it somewhere you normally build software
    (e.g. $HOME/src) then say

        $ ./configure && make && sudo make install

    ...just as with other typical Linux/Unix software.
    
    This feature subsumes the complicated build and installation
    steps from the upstream version: you don't have to `cd src`
    before building, you don't have to remember to build `scanswitch`
    separately, and you don't have to run the (now-removed) separate
    installation scripts.

    The `configure` script accepts most traditional flags for such
    a script, the most important of which is `--prefix`, which means
    that you can now install to any directory you like.  The software
    is now fully-relocatable and can be started from arbitrary
    working directories, whereas the upstream version could only be
    installed into `/opt/pidp8` and must be started from within its
    `bin` subdirectory.

    The `configure` script defines only one custom option for the
    PiDP-8/I project: `--serial-mod`, which replaces the old method
    of modifying `src/Makefile` to add `-DSERIALSETUP`.

*   Changed all of the various "PDP," "PDP-8", and "PiDP-8" strings to
    variants on "PiDP-8/I", partly for consistency and partly because
    it seems this software will never be used to simulate anything but
    a PDP-8/I.

    Part of this renaming means that the default installation location
    is now `/opt/pidp8i`, which means that installing this version
    of the software will not overwrite an existing `/opt/pidp8`
    installation.

    Another aspect of it is that the old `pdp.sh` script to [re]enter
    the simulator is now called `pidp8i`.

*   Merged Ian Schofield's "Display update for the PiDP8" patch.
    Currently it is not optional, but it is planned to be disableable
    with a `configure` script option.

*   The scripts that control the startup sequence of the PiDP-8/I
    simulator now include helpful header comments and emit useful
    status messages to the console.  Stare no more at opaque lists
    of SimH commands.

*   Added a bunch of ancillary material: wiki articles, design
    documents for proposed features, vector USB stick label artwork,
    etc.  Also filed a bunch of tickets.  If you were looking for
    ways to contribute to the development effort, see [the project's
    ticket tracker](https://tangentsoft.com/pidp8i/ticket) and its
    [wiki](https://tangentsoft.com/pidp8i/wcontent).  There's plenty
    of work to go around!

*   Made some efforts toward portability.  While this project will
    always center around Raspbian and the PiDP-8/I add-on board,
    the intent is that you should be able to unpack the project on
    any other Unix type system and at least get the simulator up and
    running with the normal SimH manual control over execution instead
    of the nice front panel controls provided by the PDP-8/I board.

    In particular, the software now builds and runs under Mac OS X.

*   Fixed a bunch of bugs!
