# PiDP-8/I Changes

## Version 2016.12.06

*   More improvements to `examples/pep001.pal`.

*   Extracted improved `PRINTS` routine from that example as
    `examples/routines/prints.pal`.


## Version 2016.12.05

*   This release marks the first binary SD card image released under my
    maintainership of the software.  As such, the major user-visible
    features in this release of the Fossil tree simply support that:

    *   The `pidp8i-init` script now understands that the OS's SSH host
        keys may be missing, and re-generates them.  Without this
        security measure, anyone who downloads that binary OS image
        could impersonate the SSH server on *your* PiDP-8/I.

    *   Added a `RELEASE-PROCESS.md` document.  This is primarily for my
        own benefit, to ensure that I don't miss a step, particularly
        given the complexity of producing the binary OS image.  However,
        you may care to look into it to see what goes on over here on
        the other side of the Internet. :)

*   Added an OS/8 BASIC solution to Project Euler Problem #1, so you can
    see how much simpler it is compared to the PAL8 assembly language
    version added in the prior release.

*   Updated the PAL8 assembly version with several clever optimizations
    by Rick Murphy, the primary effect of which is that it now fits into
    a single page of PDP-8 core memory.


## Version 2016.12.03

*   Debounced the switches.  See [the mailing list post][cdb] announcing
    this fix for details.

*   Merged the [`pidp8i-test` program][testpg] from the mailing list.
    The LED testing portion of this program [currently][gpiols] only works
    correctly without the incandescent lamp simulation patch applied.

*   Added a solution to [Project Euler Problem #1][pep001] in PAL8
    assembly language and wrote the [saga of my battle][p1saga] with
    this problem into the wiki.  This also adds a couple of useful PAL8
    routines in `examples/routines`.

*   Integrated David Gesswein's latest `palbart` program (v2.13) into
    the source distribution so that we don't have to:
    
    1.  ship pre-built RIM format paper tapes for the examples; and

    2.  put up with the old versions that OS package repos tend to have
        (Ubuntu is still shipping v2.4, from 6 years ago!)

*   Fixed a bug in the `make install` script that caused it to skip
    installing `screen` and `usbmount` from the OS's package repo when
    they are found to be missing.

*   Fixed a related bug that prevented it from disabling the serial
    console if you configure the software without `--serial-mod` and
    then install it, causing the serial console and the GPIO code in the
    PiDP-8/I simulator to fight over GPIO pins 14 and 15.

*   Removed the last of the duplicate binary media entries.  This makes
    the zip files for this version well under half the size of those for
    the 2015.12.15 upstream release despite having more features.

[cdb]:    https://groups.google.com/d/msg/pidp-8/Fg9I8OFTXHU/VjamSoFxDAAJ
[testpg]: https://groups.google.com/d/msg/pidp-8/UmIaBv2L9Ts/wB1CVeGDAwAJ
[gpiols]: https://tangentsoft.com/pidp8i/tktview?name=9843cab968
[pep001]: https://projecteuler.net/problem=1
[p1saga]: https://tangentsoft.com/pidp8i/wiki?name=PEP001.PA


## Version 2016.11.28

*   Added an intelligent, powerful build system, replacing the
    bare-bones `Makefile` based build system in the upstream version.
    See [`README.md`][readme] for more info on this.

*   The installation is now completely relocatable via `./configure
    --prefix=/some/other/place`. The upstream version would not work if
    installed somewhere other than `/opt/pidp8` due to many hard-coded
    absolute paths.  (This is enabled by the new build system, but
    fixing it was not simply a matter of swapping out the build system.)

*   Changed all of the various "PDP," "PDP-8", and "PiDP-8" strings to
    variants on "PiDP-8/I", partly for consistency and partly because it
    seems unlikely that this software will ever be used with anything
    other than the PiDP-8/I project.

    Part of this renaming means that the default installation location
    is now `/opt/pidp8i`, which has the nice side benefit that
    installing this version of the software will not overwrite an
    existing installation of the upstream version in `/opt/pidp8`.

    Another user-visible aspect of this change is that the upstream
    version's `pdp.sh` script to [re]enter the simulator is now called
    `pidp8i`.

*   Merged Ian Schofield's [Display update for the PiDP8][dupatch]
    patch.  Currently it is not optional, but there is [a plan][dudis] to
    allow this feature to be disabled via a `configure` script option.

*   The scripts that control the startup sequence of the PiDP-8/I
    simulator now include helpful header comments and emit useful
    status messages to the console.  Stare no more at opaque lists
    of SimH commands, wondering what each script does!

*   Merged `scanswitch` into the top-level `src` directory, since the
    only thing keeping it in a separate directory was the redundant
    `gpio.h` file. There were minor differences between the two `gpio.h`
    files, but their differences do not matter.

*   Installing multiple times no longer overwrites the binary OS/program
    media, since the disk images in particular may contain local
    changes.  If you want your media images overwritten, you can insist
    on it via `make mediainstall`.

*   The installation tree follows the Linux Filesystem Hierarchy
    Standard, so that files are in locations an experienced Linux user
    would expect to find them.  The biggest changes are that the content
    of the upstream `bootscripts` tree is now installed into
    `$prefix/share/boot`, and the OS/program media images which used to
    be in `imagefiles` are now in `$prefix/share/media`.

*   Added a bunch of ancillary material: [wiki articles][wiki],
    [USB stick label artwork][art], a PAL8 assembly [example program][ex]
    for you to toggle in, etc. Also filed a bunch of [tickets][tix]
    detailing feature proposals, known bugs and weaknesses, etc. If you
    were looking for ways to contribute to the development effort, these
    new resources provide a bunch of ideas.

*   Made some efforts toward portability.

    While this project will always center around Raspbian and the
    PiDP-8/I add-on board, the intent is that you should be able to
    unpack the project on any other Unix type system and at least get
    the simulator up and running with the normal SimH manual control
    over execution instead of the nice front panel controls provided by
    the PiDP-8/I board.

    In particular, the software now builds under Mac OS X, though it
    does not yet run properly.  (The modified SimH PDP-8 simulator
    currently misbehaves if the PiDP-8/I panel is not present.  Fixing
    this is on the radar.)

*   Fixed a bunch of bugs!

[readme]:  https://tangentsoft.com/pidp8i/doc/trunk/README.md
[dupatch]: https://groups.google.com/forum/#!topic/pidp-8/fmjt7AD1gIA
[dudis]:   https://tangentsoft.com/pidp8i/tktview?name=e06f8ae936
[wiki]:    https://tangentsoft.com/pidp8i/wcontent
[ex]:      https://tangentsoft.com/pidp8i/doc/trunk/examples/README.md
[art]:     https://tangentsoft.com/pidp8i/dir?c=trunk&name=labels
[tix]:     https://tangentsoft.com/pidp8i/tickets


## Version 2015.12.15

*   The official upstream release of the software, still current as of
    late 2016, at least.
