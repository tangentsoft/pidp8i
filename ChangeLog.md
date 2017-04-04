# PiDP-8/I Changes

## Version 2017.04.04

*   Removed the PDP-8 CPU idle detection feature.  Oscar Vermeulen
    reports that it also interfered with NLS LED driving mode in his
    last version, and we have no reason to believe our NLS code is
    sufficiently different to avoid the problem.

    This does not affect ILS users, since enabling ILS mode disables
    this change.

    NLS system upgrades wouldn't normally be affected because the
    changed files are not normally overwritten on installation.  If
    you're affected, you know it, and how to fix it.

*   Replaced the earlier attempts at finding an ideal IPS rate for the
    simulator when run on single-core hosts with usage of SIMH's
    percentage style throttle rates.  We now explicitly set the throttle
    rate to "50%" which not only achieves an even higher throttle rate
    than in earlier releases, it's reliable in the face of varying
    background CPU usage.  See the single-core section of
    `README-throttle.md` for details.


## Version 2017.04.01 The "I May Be a Fool, but I am *Your* Fool" Release

*   Added the `configure --alt-serial-mod` option to change the GPIO
    code to work with [James L-W's alternative serial mod][sm2].

*   Increased the stock CPU throttle from 0.67 MIPS to 0.85 MIPS on most
    Pi 1 class devices, except for the Pi Zero which is a bit faster and
    so is able to run at 1.25 MIPS.

    (Testing shows that increasing it further causes SIMH to complain
    that it doesn't have enough CPU power to run that fast, despite the
    fact that top(1) shows it taking only about half the available CPU
    power.  It's just as well: we don't want to hog all the CPU power on
    a single-core Pi, anyway.)

*   When built in NLS mode, most of the PDP-8 simulator configuration
    files we generate now have CPU idle detection enabled, allowing host
    CPU usage to drop when the simulated CPU is basically idle, such as
    when waiting for keyboard input.

*   Replaced a simplistic 2-second delay in the startup sequence of the
    simulator, `pidp8i-test`, and "[Incandescent Thought][it]" with a
    smarter interlocked startup sequencing mechanism that largely
    eliminates the delay.

*   Fixed a problem introduced in v20170204 which causes the `LOAD_ADD`
    and `DEPOSIT` switch handlers to generate incorrect core addresses
    when the SIMH PDP-8 CPU core sets bits beyond the lower 12 in the PC
    register.  We were assuming this register is always 12-bit clean,
    but it isn't.

*   Merged in upstream SIMH improvements.  Changes relevant to the
    PiDP-8/I include:

    *   The PDP-8 CPU reset mechanism now does more of what our
        preexisting `START` switch handler did, so we now delegate to
        that upstream mechanism, reducing behavior differences between
        the cases.

    *   Improved keyboard polling behavior in terminal handler.

    *   Fixed many bugs identified by Coverity Scan in many different
        subsystems of the simulator.  Normally I wouldn't note such a
        low-level change in this user-centric document, but it is
        possible that some of these improvements fix user-visible bugs.

*   SIMH's default PDP-8 configuration enables the DF32 disk device with
    the short name "DF", but since the SIMH `DEPOSIT` command works on
    both devices and registers, a command like `d df 0` is ambiguous,
    causing the default configuration of SIMH to give a "Too few
    arguments" error for this command, even though it's obvious that you
    mean the CPU DF register here.  (Surely you didn't mean to overwrite
    the first word of your disk image instead?)  Since upstream refuses
    to fix it, I have disabled the DF32 device in all of the default
    `boot/*.script` files.

    Since these scripts aren't overwritten on installation, this will
    only affect new installs unless you say `make mediainstall`, in
    which case your binary OS media is also overwritten.  Do this at
    your own risk!

*   Many improvements to the `SING_STEP` + `DF` USB auto-mounting and
    SIMH attaching feature:

    *   Prior versions apparently could only mount paper tape images
        correctly.  This release includes a fix that allows RX type
        floppy images and TU type DECtape images to autoattach.

    *   Changed the meaning of `SING_STEP` + `DF=7` from attaching an RL
        type removable hard disk cartridge image to the first RL01 drive
        to attaching an older and more period-correct RK type image to
        the *second* RK05 drive.  The second half of the change lets you
        use this feature along with the stock OS/8 image to attach a
        second hard disk at runtime.

    *   The file name matching code used by this feature was only using
        the first character of the file name extension, and it wasn't
        pinning the match to the end of the file name string.  Thus, if
        you'd set `DF=0`, it would look for the first file with `.p`
        anywhere in the file name, not `.pt` at the end, as expected.

    *   Improved error messages given when this feature doesn't work.

*   The `pidp8i-test` program now accepts a `-v` flag to make it give
    the version and configuration string and exit.

*   `pidp8i-test` now exits more cleanly, shutting down ncurses, turning
    off the front panel LEDs, and writing a diagnostic message on signal
    exits.

*   The version number part of the configuration string written by
    `pidp8i-test -v` and as the first line of output from the simulator
    now uses the same 10-digit Fossil checkin ID format as the Fossil
    timeline, making it easier to match that version to a particular
    Fossil checkin.

*   The Raspberry Pi model detection code behind the Pi model tag in the
    configuration string was only doing the right thing for the more
    common Pi models.  It now reports the correct Pi model for the
    various flavors of Compute Module and the Pi Zero.

*   Improved error handling in the process that inserts the version info
    into the configuration string emitted when the simulator and test
    programs start up.

*   We now build and link in the upstream `sim_video` module, which
    allows access to a video display via SDL.  We do not currently
    use this in the project core, but I recall hearing about a
    third-party project that uses this for a local graphical X-Y
    vector display implementation for playing Spacewar!  When built
    on a system without SDL or even a local bitmap display, this
    code becomes nearly a no-op, affecting build time very little.

*   SIMH changes to a different delay mechanism at CPU throttle rates
    below 1000 IPS, which prevents our incandescent lamp simulator from
    running correctly.  Therefore, when you give a `./configure
    --throttle` flag value that would use this throttle mode, we disable
    the ILS even when building on multi-core Raspberry Pis.

    (This fix doesn't disable the ILS at run time if you manually set a
    throttle value under 1000 IPS via a SIMH command.  We'll try to help
    you avoid accidentally shooting yourself in the foot, but if you're
    going to *aim*, you're on your own.)

*   Several internal refactorings to improve code style, reduce the
    [upstream SIMH patch footprint][foot], and fix corner case bugs.

[foot]: http://pastebin.com/5Jnx15QX
[it]:   https://tangentsoft.com/pidp8i/wiki?name=Incandescent+Thought
[sm2]:  https://groups.google.com/d/msg/pidp-8/-leCRMKqI1Q/Dy5RiELIFAAJ


## Version 2017.02.04

*   Largely rewrote the incandescent lamp simulator (ILS) feature.
    The core of Ian Schofield's original contribution is still hiding
    in there if you go spelunking, but everything surrounding it
    is different.

    The changes and design decisions surrounding this are [complicated
    and subtle][ilsstory], but the end result is that the ILS is now
    free of judders, blips, shudders, and chugs.  (Those are nuanced
    technical terms for "badness.")  The ILS is now buttery smooth from
    1 kIPS to the many-MIPS rate you get running un-throttled on a Pi 3.

*   Although most of the ILS work does not directly apply to the "no
    lamp simulator" (NLS) case, the sample rate dithering reduces
    display update artifacts seen in this case as well.

*   Slowed the ILS brightness rates down a bit: more lampy, less
    snappy.  Whether this is accurate or not is something we'll have
    to determine through research separately in progress.

*   The ILS display is a bit brighter: the delay values used in prior
    versions put a cap on max brightness that was well under the full
    LED brightness achievable.

*   For the first time, it is possible to build Deeper Thought (any
    version) against the ILS, with minor adjustments.  Prior versions of
    the ILS had too different an external interface to allow this.  Full
    details are in a [new wiki article][ithought].

*   In normal free-running mode, the simulator lights the Fetch and
    Execute LEDs at 50%, whereas before there was an imbalance that
    purely had to do with the much lower complexity of fetching an
    instruction inside the simulator vs executing it.
    
    (In real hardware, the complexities were different: fetch involved a
    core memory retrieval, very much non-instantaneous, whereas the
    execution of the fetched instruction kind of happened all at once in
    complicated electron flows, rather than the sequential C code of the
    SIMH PDP-8 simulator.  Thus, it was reasonable for DEC to talk about
    PDP-8/I fetch-and-execute cycles as if the two steps were of equal
    time complexity.)

    I haven't compared the resulting LED appearance to a real PDP-8/I.

*   Several other tweaks to LED state handling to better match real
    hardware.

*   Redesigned the `pidp8i-test` program to allow manual stepping
    forwards and backwards in addition to the previous auto-advancing
    behavior.
    
    As soon as you press one of the arrow keys, the test program moves
    to the next or previous action in the sequence and stops
    auto-advancing.  This mode is useful when testing the hardware with
    a multimeter or similar, and you need a certain row or column to
    stay lit up indefinitely.

    You can also press <kbd>R</kbd> to resume auto-advancing behavior,
    or either <kbd>Ctrl-C</kbd> or <kbd>X</kbd> to exit the program
    gracefully.

    This requires that you have `libncurses-dev` installed on your Pi.

*   The SIMH PDP-8 simulator's internal SR register now matches the
    hardware switches when you say Ctrl-E then `ex sr`.  Prior versions
    only loaded the hardware switch register values into the internal
    register when it executed an `OSR` instruction.

*   Copied the KiCad design files into the source tree since they are
    now formally released by Oscar Vermeulen under a Creative
    Commons license.  Also included the PDF version of the schematic
    produced by Tony Hill.  (This is all in the `hardware/` directory.)

*   Lowered the default simulator throttle value for single-core Pi
    boards from 1332 kIPS to 666 kIPS after doing some testing with
    the current code on a Raspberry Pi 1 Model B+.  This value was
    chosen since it is approximately twice the speed of a PDP-8/I.
    This leaves a fair bit of CPU left over for background tasks,
    including interactive shell use.

    This value may be a bit low for Pi Zero users, but it is easily
    [adjustable][rmth].

*   Merged in the relevant SIMH updates.  This is all internal stuff
    that doesn't affect current PiDP-8/I behavior.

*   Many build system and documentation improvements.

[ilsstory]: https://tangentsoft.com/pidp8i/wiki?name=Incandescent+Lamp+Simulator
[ithought]: https://tangentsoft.com/pidp8i/wiki?name=Incandescent+Thought


## Version 2017.01.23

*   When any program that talks to the PiDP-8/I front panel starts up,
    it now prints out a banner modeled on the [Erlang configuration
    line][ecl].  For example, when I run the software in the development
    tree on my PiDP-8/I, I get the following:

        PiDP-8/I trunk:i49cd065c [pi3b] [ils] [serpcb] [gpio]

    It tells me that:
    
    *   I'm running code built from Fossil checkin ID 49cd065c on the
        trunk branch, as opposed to a release version, which would be
        marked `release:v20170123` or similar.  (The `i` here is a tag
        standing for "ID", as in Fossil checkin ID.  Contrast `v` used
        to tag release version numbers.)

    *   I'm running it on a Raspberry Pi 3 Model B with Ian Schofield's
        incandescent lamp simulator (ILS) feature enabled.

    *   The software is built to expect that the PiDP-8/I PCB and the Pi
        board attached to it have had the serial mods made to them.

    *   The GPIO module found the GPIO hardware and was able to attach
        to it.

*   I get a very different result when running it on my desktop machine:

        PiDP-8/I trunk:id8536d91 [cake] [nls] [nopcb] [rt]

    This tells me:

    *   I'm running a different version of the development branch (i.e.
        the "trunk") of the code than what's running on the Pi.

    *   It's not running on a Pi at all.  (Cake ≠ pi.)

    *   I've disabled the ILS feature, so it's running with the "no lamp
        simulator" (NLS) GPIO module.
    
    *   Which is all to the good, because there's no point burning CPU
        power running the ILS code on a host machine that doesn't have a
        PiDP-8/I PCB attached.

    *   The GPIO thread is running with real-time privileges.

*   The ILS feature can now be disabled at `configure` time via the new
    `--no-lamp-simulator` flag.  This option is automatically set when
    building on a single-core Raspberry Pi.  (The flag is there only to
    allow someone building the software on a multi-core host machine to
    disable the ILS.)

*   Tweaked the ILS decay constants to be asymmetric, better mimicking
    the way real incandescent lamps work: they heat up to full
    brightness faster than they fade to perceptively "off."

*   The LED values used by the GPIO thread were being recalculated way
    too often.

    In the ILS case, it was updating the values approximately at the
    same rate as the ILS's PWM core frequency, roughly 7,500 times per
    second, which is far higher than the human persistence of vision
    limit.  While the PWM rate does need to be that fast to do its job,
    the underlying LED state values do not need to change nearly that
    often to fool the human into seeing instantaneous updates.

    The NLS case was actually worse, recalculating the LED values on
    every instruction executed by the PDP-8 CPU simulator, which even on
    a Pi 1 is likely to be a few MHz.

    In both the ILS and NLS cases, we now update the LED values about
    100 times a second, maintaining that rate dynamically based on the
    current execution speed of the simulator.

*   In prior versions, the ILS was only updating at its intended rate
    when the PDP-8 simulator was running flat-out on a current
    multi-core Raspberry Pi.  If you throttled the SIMH simulator to a
    slower execution rate, the display quality would start to degrade
    noticeably below about 1 MIPS.

*   With the prior fix, we now ship 5.script (i.e. the handler for
    starting the simulator with IF=5, or restarting it with IF=5 +
    `SING_STEP`) set to a throttle value of 30 kIPS, which allows the
    human to see each AC/MQ modification.  The built-in delay loops are
    still there, else we'd have to drop this to well under 1 kIPS.

*   The `SING_INST` switch now immediately puts the processor into
    single instruction mode, not requiring a separate press of the
    `STOP` key, as in prior versions.  This is the correct behavior
    according to the 1967-1968 edition of DEC's Small Computer Handbook
    for the PDP-8/I.

*   Greatly simplified the way single-instruction mode, processor
    stoppage, and the `CONT` key work.  The prior implementation was
    error-prone and difficult to understand.  This fixes a particularly
    bad interaction between the way `HLT` instructions and `CONT` key
    presses were handled, causing the processor to not resume correctly
    from `HLT` state.

*   Consolidated and cleaned up the bulk of the PiDP-8/I switch handling
    code so that it is not so intimately tied into the guts of the PDP-8
    CPU emulator.  This will greatly increase the chance that future
    updates to the upstream SIMH code will apply cleanly to our version.

*   Fixed a bug in `examples/bit-rotate.pal` which caused it to skip the
    actual bit rotation step.  We were trying to microcode two
    instructions into one that the PDP-8 won't accept together, and we
    didn't catch it until now because the HLT bug masked it, and the
    `palbart` assembler we ship isn't smart enough to notice the bug.

*   Fully generalized the mechanism for generating `obj/*.lst`,
    `bin/*.pt`, and `boot/*.script` from `examples/*.pal`.  You can now
    drop any PAL assembly language program into the `examples` directory
    and type `make` to build these various output forms automatically
    using the shipped version of `palbart`.  This effectively turns this
    PiDP-8/I software distribution into a PDP-8 assembly language
    development environment: rapidly build, test, and debug your PAL
    programs on your PC before you deploy them to real hardware.  Or,
    write PAL programs to debug the hardware or simulator, as we did
    with `examples/bit-rotate.pal`.

*   Fixed a sorting bug in the tool that generates `boot/*.script` from
    `obj/*.lst`, resulting in `dep` instructions that weren't sorted by
    core address.  This didn't cause any real problem, but it made
    tracing the execution of a PAL assembly program difficult if you
    were trying to refer to the `boot/*.script` file to check that the
    PiDP-8/I's front panel is showing the correct register values.

*   Updated SIMH to the latest upstream version and shipping a subset of
    the SIMH docs as unversioned files from tangentsoft.com.

*   The `configure` script now aborts the build if it sees that you're
    trying to build the software as root, since that means it generates
    the init script and the pidp8i script expecting to run the installed
    software as root, not as your normal user.  The most common way this
    happens is that you have a configured source tree, then change one
    of the `*.in` files and say `sudo make install`, thinking to build
    and install the change in one step.  This fixes that.

*   Several improvements to the build system.

[ecl]: http://stackoverflow.com/q/1182025/142454


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

*   Since v20161128, when you `make install` on a system with an
    existing PiDP-8/I software installation, the binary OS media images
    were not being overwritten, on purpose, since you may have modified
    them locally, so the installer chose not to overwrite your versions.

    With this release, the same principle applies to the SIMH boot
    scripts (e.g. `$prefix/share/boot/0.script`) since those are also
    things the user might want to modify.

    This release and prior ones do have important changes to some of
    these files, so if you do not wish to overwrite your local changes
    with a `make mediainstall` command, you might want to diff the two
    versions and decide which changes to copy over or merge into your
    local files.

[hltbug]:  https://tangentsoft.com/pidp8i/info/f961906a5c24f5de
[copying]: https://tangentsoft.com/pidp8i/doc/trunk/COPYING.md
[rmth]:    https://tangentsoft.com/pidp8i/doc/trunk/README-throttle.md


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

[vteditdoc]: https://tangentsoft.com/pidp8i/wiki?name=Using+VTEDIT


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
[dupatch]: https://groups.google.com/forum/#!topic/pidp-8/fmjt7AD1gIA
[dudis]:   https://tangentsoft.com/pidp8i/tktview?name=e06f8ae936
[wiki]:    https://tangentsoft.com/pidp8i/wcontent
[ex]:      https://tangentsoft.com/pidp8i/doc/trunk/examples/README.md
[art]:     https://tangentsoft.com/pidp8i/dir?c=trunk&name=labels
[tix]:     https://tangentsoft.com/pidp8i/tickets


## Version 2015.12.15

*   The official upstream release of the software, still current as of
    late 2016, at least.
