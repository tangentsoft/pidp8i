# PiDP-8/I Changes


# Release 2016.11.26 (UNRELEASED)

*   Added an intelligent, powerful build system, replacing the
    bare-bones `Makefile` based build system in the upstream version.

    This means you don't unpack the zip file or tarball into
    `/opt/pidp8` and build it in place, as with the upstream version.
    Instead, you unpack it somewhere you normally build software
    (e.g. `$HOME/src`) then say

        $ ./configure && make && sudo make install

    ...just as with other typical Linux/Unix software.
    
    This feature subsumes the complicated build and installation
    steps from the upstream version: you don't have to `cd src`
    before building, you don't have to remember to build `scanswitch`
    separately, and you don't have to run the (now-removed) separate
    installation scripts.

    The `configure` script accepts most traditional flags for such
    a script, the most important of which is `--prefix`, which means
    that you can now install to any directory you like; you are no
    longer required to install to a fixed location in order for
    the scripts to work properly.  You can also run these scripts
    from arbitrary working directories, since the relative paths
    in the scripts are replaced by absolute paths during the
    build and installation process.

    The `configure` script defines only one custom option for the
    PiDP-8/I project: `--serial-mod`, which replaces the old method
    of modifying `src/Makefile` to add `-DSERIALSETUP` if you have
    done the "serial mod" to your Raspberry Pi and PiDP-8/I PCBs.

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

*   Merged Ian Schofield's [Display update for the
    PiDP8](https://groups.google.com/forum/#!topic/pidp-8/fmjt7AD1gIA)
    patch.  Currently it is not optional, but there is [a
    plan](https://tangentsoft.com/pidp8i/tktview?name=e06f8ae936)
    to allow this feature to be disabled via a `configure`
    script option.

*   The scripts that control the startup sequence of the PiDP-8/I
    simulator now include helpful header comments and emit useful
    status messages to the console.  Stare no more at opaque lists
    of SimH commands, wondering what each script does!

*   Added a bunch of ancillary material: [wiki articles][1], [USB
    stick label artwork][2], etc. Also filed a bunch of [tickets][3]
    detailing feature proposals, known bugs and weaknesses, etc. If
    you were looking for ways to contribute to the development
    effort, these new resoruces provide a bunch of ideas.

*   Made some efforts toward portability.

    While this project will always center around Raspbian and the
    PiDP-8/I add-on board, the intent is that you should be able to
    unpack the project on any other Unix type system and at least get
    the simulator up and running with the normal SimH manual control
    over execution instead of the nice front panel controls provided by
    the PDP-8/I board.

    In particular, the software now builds under Mac OS X, though it
    does not yet run properly.  (The modified SimH PDP-8 simulator
    currently misbehaves if the PiDP-8/I panel is not present.  Fixing
    this is on the radar.)

*   Fixed a bunch of bugs!


[1]: https://tangentsoft.com/pidp8i/wcontent
[2]: https://tangentsoft.com/pidp8i/dir?c=trunk&name=labels
[3]: https://tangentsoft.com/pidp8i/tickets


# Release 2015.12.15

*   The official upstream release of the software, still current as of
    late 2016, at least.
