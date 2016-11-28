# PiDP-8/I Changes


# Release 2016.11.27 (UNRELEASED)

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
    other than the PDP-8/I project.

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

*   Added a bunch of ancillary material: [wiki articles][wiki], [USB
    stick label artwork][art], a PAL-III assembly [example program][ex]
    for you to toggle in, etc. Also filed a bunch of [tickets][tix]
    detailing feature proposals, known bugs and weaknesses, etc. If you
    were looking for ways to contribute to the development effort, these
    new resoruces provide a bunch of ideas.

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


[readme]:  https://tangentsoft.com/pidp8i/doc/trunk/README.md
[dupatch]: https://groups.google.com/forum/#!topic/pidp-8/fmjt7AD1gIA
[dudis]:   https://tangentsoft.com/pidp8i/tktview?name=e06f8ae936
[wiki]:    https://tangentsoft.com/pidp8i/wcontent
[ex]:      https://tangentsoft.com/pidp8i/doc/trunk/examples/README.md
[art]:     https://tangentsoft.com/pidp8i/dir?c=trunk&name=labels
[tix]:     https://tangentsoft.com/pidp8i/tickets


# Release 2015.12.15

*   The official upstream release of the software, still current as of
    late 2016, at least.
