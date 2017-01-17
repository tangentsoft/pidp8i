# PiDP-8/I Changes

## Version 2017.01.16

*   Prior releases did not include proper licensing for many of the
    included files.  This project was, therefore, not a proper Open
    Source Software project.  This problem has been fixed.

    In this release, many files that were previously technically only
    under standard copyright due to having no grant of license now have
    an explicit license, typically the same as SIMH itself.  (Thank you
    to all of the authors who allowed me to apply this license to their
    contributions!)

    For several other files, I was able to trace down some prior license
    and include its text here for the first time.

    There remain a few "gray" items: the TSS/8 and ETOS disk images.
    See the [`COPYING.md` file][copying] for more on the current status
    of these OS images.  If the legal status of these files clarifies in
    the future, this software distribution will react accordingly, even
    if that means removing these files from the distribution if we learn
    that these files are not freely-redistributable, as we currently
    believe them to be today.

*   The Step Counter LEDs on the front panel weren't being lit properly
    when EAE instructions were being used.  Thanks for this patch go to
    Henk Gooijen and Paul R. Bernard.

*   The prior `boot/1.script` and `boot/5.script` files are no longer
    simply opaque lists of octal addresses and machine code.  They are
    generated from PAL assembly files provided in the `examples`
    directory, so that you can now modify the assembly code and type
    `make` to rebuild these boot scripts.

*   The mechanism behind the prior item is fully general-purpose, not
    something that only works with `1.script` and `5.script`.  Any
    `examples/*.pal` file found at `make` time is transformed into a
    SIMH boot script named after the PAL file and placed in the `boot`
    directory.  This gives you an easier way to run PDP-8 assembly code
    inside the simulator.  After saying `make` to transform `*.pal` into
    `*.script` files, you can run the program with `bin/pidp8i-sim
    boot/my-program.script` to poke your program's octal values into
    core and run it.  This round-trip edit-and-run process is far faster
    than any of the options given in the [examples' `README.md`
    file][ex].

*   Disassembled both versions of the RIM loader to commented, labeled
    PAL assembly language files.  If you ever wanted to know what those
    16 mysterious instructions printed on the front panel of your
    PiDP-8/I did, you can now read my pidgin interpretation of these
    programs in `examples/*-rim.loader.pal` and become just as confused
    as I am now. :)

*   The two RIM loader implementations now start with a nonstandard
    `HLT` instruction so that when you fire up the simulator with IF=1 to
    start the high-speed RIM loader, it automatically halts for you, so
    you don't have to remember to STOP the processor manually.

    There is currently [a bug][hltbug] in the way the simulator handles
    `HLT` instructions which prevents you from simply pressing START or
    CONT to enter the RIM loader after you've attached your paper tape,
    so you still have to manually toggle in the 7756 starting address
    and press START to load the tape into core.  (I hope to fix this
    before the next release, but no promises.)

*   Added the `configure --throttle` feature for making the simulator
    run at a different speed than it normally does.  See
    [`README-throttle.md`][rmth] for details.

*   The build system now reacts differently when building the PiDP-8/I
    software on a single-core Raspberry Pi:

    *   If you're building the trunk or release branch, you'll get a
        configure error because it knows you can't run the current
        implementation of the incandescent lamp simulator on a
        single-core Pi.  (Not enough spare CPU power, even with heavy
        amounts of throttling.)

    *   If you're building the no-lamp-simulator branch, it inserts a
        throttle value into the generated `boot/*.script` files that do
        not already contain a throttle value so that the simulator
        doesn't hog 100% of the lone core, leaving some spare cycles for
        background tasks.  The above `--throttle` feature overrides
        this.

    These features effectively replace the manual instructions in the
    old `README-single-core.md` file, which is no longer included with
    this software distribution, starting with this release.

*   Lowered the real-time priority of the GPIO thread from 98 to 4.
    This should not result in a user-visible change in behavior, but it
    is called out here in case it does.  (In our testing, such high
    values simply aren't necessary to get the necessary performance,
    even on the trunk branch with the incandescent lamp simulator.)

*   Since v20161128, you `make install` on a system with an existing
    PiDP-8/I software installation, the binary OS media images were not
    being overwritten, on purpose, since you may have modified them
    locally, so the installer chose not to overwrite your versions.

    With this release, the same principle applies to the SIMH boot
    scripts (e.g. `$prefix/share/boot/0.script`) since those are also
    things the user might want to modify.

    This release and prior ones do have important changes to some of
    these files, so if you do not wish to overwrite your local changes
    with a `make mediainstall` command, you might want to diff the two
    versions and decide which changes to copy over or merge into your
    local files.

[copying]: https://tangentsoft.com/pidp8i/doc/trunk/COPYING.md


## Version 2017.01.05

*   Automated the process for merging in new SIMH updates.  From within
    the PiDP-8/I software build directory, simply say `make simh-update`
    and it will do its best to merge in the latest upstream changes.

    This process is more for the PiDP-8/I software maintainers than for
    the end users of that software, but if you wish to update your SIMH
    software without waiting for a new release of *this* software, you
    now have a nice automated system for doing that.

*   Updated SIMH using that new process.  The changes relevant to the
    PiDP-8/I since the prior update in release v20161226 are:

    *   Many more improvements to the simulator's internal timer system.
        This should make deliberate underclocking more accurate.

    *   It is now possible to get hex debug logs for the simulator console
        port by cranking up the simulator's debug level.

*   The simulator now reports the upstream Git commit ID it is based on
    in its version string, so that if you report bugs upstream to the
    SIMH project, you can give them a version number that will be
    meaningful to them.  (They don't care about our vYYYYMMDD release
    numbers or our Fossil checkin IDs.)


## Version 2016.12.26 (The Boxing Day release)

*   Tony Hill updated SIMH to the latest upstream version.

    This change represents about 15 months worth of work in the
    [upstream project][simh] — plus a fair bit of work by Tony to merge
    it all — so I will only summarize the improvements affecting the
    PDP-8 simulator here:

    *   Many improvements to the internal handling of timers.
    
        The most user-visible improvement is that you can now clock your
        emulated PDP-8 down to well below the performance of a real
        PDP-8 via `SET THROTTLE`, which can be useful for making
        blinkenlights demos run at human speeds without adding huge
        delay loops to the PDP-8 code implementing that demo.

    *   Increased the number of supported terminals from four to either
        twelve or sixteen, depending on how you look at it.  Eight of
        the additional supported terminal devices are conflict-free,
        while the final four variously conflict with one or more of the
        other features of the simulated PDP-8.  If you want to use all
        16, you will be unable to use the FPP, CT, MT and TSC features
        of the system.

        This limitation reflects the way the PDP-8 worked.  It is not an
        arbitrary limitation of SIMH.

    *   Added support for the LS8E printer interface option used by the
        WPS8 word processing system.

    *   The simulator's command console now shows the FPP register
        descriptions when using it as a PDP-8 debugger.

    *   Added the `SHOW TTIX/TTOX DEVNO` SIMH command to display the
        device numbers used for TTIX and TTOX.

    *   The `SHOW TTIX SUMMARY` SIMH command is now case-insensitive.

    *   Upstream improvements to host OS/compiler compatibility.  This
        increases the chances that this software will build out of the
        box on random non-Raspbian systems such as your development
        laptop running some uncommon operating system.

*   When you `make install`, we now disable Deeper Thought 2 and the
    legacy `pidp8` service if we find them, since they conflict with our
    `pidp8i` service.

*   Added the install user to the `gpio` group if you `make install` if
    that group is present at install time.  This is useful when building
    and installing the software on an existing Raspbian SD card while
    logged in as a user other than `pi` or `pidp8i`.

[simh]: https://github.com/simh/simh/


## Version 2016.12.18

*   The entire software stack now runs without explicit root privileges.
    It now runs under the user and group of the one who built the
    software.

    For the few instances where it does need elevated privileges, a
    limited-scope set of sudo rules are installed that permit the
    simulator to run the necessary helper programs.

*   The power down and reboot front panel switch combinations are no
    longer sensitive to the order you flip the switches.

*   Changed the powerdown front panel switch combination to the more
    mnemonically sensible `Sing_Step` + `Sing_Inst` + `Stop`.

    Its prior switch combo — `Sing_Step` + `Sing_Inst` + `Start` — is
    now the reboot sequence, with the mnemomic "restart."

*   Removed the USB stick mount/unmount front panel switch combos.  The
    automount feature precludes a need for a manual mount command, and
    unmount isn't necessary for paper tape images on FAT sticks.

*   The simulator now runs correctly on systems where the GPIO setup
    process fails.  (Basically, anything that isn't a Raspberry Pi.)
    Prior to this, this failure was just blithely ignored, causing
    subsequent code to behave as though all switches were being pressed
    at the same time, causing utter havoc.

    The practical benefit of this is that you can now work with the
    software on your non-Pi desktop machine, losing out only on the
    front panel LEDs and switches.  Everything else works just as on the
    Pi.  You no longer need a separate vanilla SimH setup.

*   Added a locking mechanism that prevents `pidpi8-test` and
    `pidp8i-sim` from fighting over the front panel LEDs.  While
    one of the two is running, the other refuses to run.

*   Added `examples/ac-mq-blinker.pal`, the PAL8 assembly code for the
    `boot/5.script` demo.

*   Fixed two unrelated problems with OS/8's FORTRAN IV implementation
    which prevented it from a) building new software; and b) running
    already-built binaries.  Thanks go to Rick Murphy for providing the
    working OS/8 images from which the files needed to fix these two
    problems were extracted.

*   Added the VT100-patched `VTEDIT` TECO macro from Rick Murphy's OS/8
    images, and made it automatically run when you run TECO from the
    OS/8 disk pack.  Also added documentation for it in `VTEDIT.DC` on
    the disk pack as well as [in the wiki][vteditdoc].

*   The default user name on the binary OS images is now `pidp8i`
    instead of `pi`, its password has changed to `edsonDeCastro1968`,
    and it demands a password change on first login.  I realize it's a
    hassle, but I decided I didn't want to contribute to the plague of
    open-to-the-world IoT boxes.

*   Many build system and documentation improvements.

[vteditdoc][https://tangentsoft.com/pidp8i/wiki?name=Using+VTEDIT]


## Version 2016.12.06

*   The `pidp8i-test` program's LED test routines did not work correctly
    when built against the incandescent lamp simulator version of the
    GPIO module.  Reworked the build so that this test program builds
    against the no-lamp-simulator version instead so that you don't have
    to choose between having the lamp simulator or having a working
    `pidp8i-test` program.

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
[rmth]:    https://tangentsoft.com/pidp8i/doc/trunk/README-throttle.md
[dupatch]: https://groups.google.com/forum/#!topic/pidp-8/fmjt7AD1gIA
[dudis]:   https://tangentsoft.com/pidp8i/tktview?name=e06f8ae936
[wiki]:    https://tangentsoft.com/pidp8i/wcontent
[ex]:      https://tangentsoft.com/pidp8i/doc/trunk/examples/README.md
[art]:     https://tangentsoft.com/pidp8i/dir?c=trunk&name=labels
[tix]:     https://tangentsoft.com/pidp8i/tickets
[hltbug]:  https://tangentsoft.com/pidp8i/info/f961906a5c24f5de


## Version 2015.12.15

*   The official upstream release of the software, still current as of
    late 2016, at least.
