# PiDP-8/I Changes


# Release 2016.11.26 (UNRELEASED)

*   Added an intelligent, powerful build system, replacing the
    bare-bones `Makefile` based build system in the upstream version.
	See [`README.md`][1] for more info on this.

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

*   Added a bunch of ancillary material: [wiki articles][2], [USB
    stick label artwork][3], etc. Also filed a bunch of [tickets][4]
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


[1]: https://tangentsoft.com/pidp8i/doc/trunk/README.md
[2]: https://tangentsoft.com/pidp8i/wcontent
[3]: https://tangentsoft.com/pidp8i/dir?c=trunk&name=labels
[4]: https://tangentsoft.com/pidp8i/tickets


# Release 2015.12.15

*   The official upstream release of the software, still current as of
    late 2016, at least.
