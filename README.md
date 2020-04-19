# Getting Started with the PiDP-8/I Software

## Orientation

You will be reading this file from one of several likely places:

*   [Online][tlrm], within [the Fossil project repository][home].

*   As a text file within the [source packages][src].

*   In the read-only [GitHub mirror][ghm].

The source packages and the GitHub mirror are secondary outputs from the
Fossil repository, which is the primary home for the PiDP-8/I software
development project.

[ghm]:  https://github.com/tangentsoft/pidp8i
[home]: https://tangentsoft.com/pidp8i/
[src]:  https://tangentsoft.com/pidp8i/#src
[tlrm]: https://tangentsoft.com/pidp8i/doc/trunk/README.md


## Prerequisites

*   A Raspberry Pi with the 40-pin GPIO connector.  That rules out the
    first-generation Raspberry Pi model A and B boards which had a
    26-pin GPIO connector.

*   An SD card containing [a compatible OS][os].

*   This software distribution, unpacked somewhere convenient within the
    filesystem on the Raspberry Pi.  We recommend that you unpack it
    somewhere your user has read/write access like `$HOME/src/pidp8i`.

*   We require several tools and libraries that aren't always installed:

    *   A working C compiler and other standard Linux build tools,
        such as `make(1)`.

    *   The Raspberry Pi development libraries

    *   Python's `pexpect` library

    *   The `ncurses` development libraries

    To install all of this on a Raspbian type OS, say:

        $ sudo apt update
        $ sudo apt install build-essential libraspberrypi-dev \
             libncurses-dev python-pip
        $ sudo pip install pexpect

[os]: https://tangentsoft.com/pidp8i/wiki?name=OS+Compatibility


<a id="preparing"></a>
## Preparing Your Pi

If you have a working Raspberry Pi setup, you can probably [skip
ahead](#unpacking).

If you’ve just barely unpacked Raspbian onto an SD card and are now
trying to get the PiDP-8/I software distribution working on it, stop and
go through the [Rasbperry Pi documentation][rpd] first. At the absolute
minimum, run `raspi-config` and make sure the Localization settings are
correct. The defaults are for the United Kingdom, home of the Raspberry
Pi Foundation, so unless you live there, the defaults are probably not
correct for you, in *your* location.

[rpd]: https://www.raspberrypi.org/documentation/


<a id="unpacking"></a>
## Getting the Software onto Your Pi

If you're reading this file within an unpacked distribution of the
PiDP-8/I software, you may [skip to the next section](#configuring),
'ecause you have already achieved this section's aim.

If you are reading this [online][this] and have not yet downloaded and
unpacked the software source code onto your Pi, this section will get
you going.

[this]: https://tangentsoft.com/pidp8i/doc/trunk/README.md


<a id="transferring"></a>
### Transferring the File to the Pi

The first step is to get the tarball (`*.tar.gz` file) onto
the Pi. There are many options:

1.  **Copy the file to the SD card** you're using to boot the Pi.
    When inserted into a Mac or Windows PC, typically only the `/boot`
    partition mounts as a drive your OS can see.  (There's a much
    larger partition on the SD card, but most PCs cannot see it.)
    There should be enough free space left in this small partition to
    copy the file over.  When you boot the Pi up with that SD card,
    you will find the tarball in `/boot`.

2.  **Pull the file down to the Pi** over the web, directly to the Pi:

        $ wget -O pidp8i.tar.gz https://goo.gl/JowPoC

    That will get you a file called `pidp8i.tar.gz` in the current
    working directory containing the latest *release* version.  (Use the
    "bleeding edge" links on the home page if you want the tip of trunk
    instead!)

3.  **SCP the file over** to a running Pi from another machine.
    If your Pi has [OpenSSH installed and running](#sshd), you can use
    [WinSCP][wscp], [Cyberduck][cd], [FileZilla][fz] or another SCP
    or SFTP-compatible file transfer program to copy the file to the
    Pi over the network.

[cd]:   https://cyberduck.io/
[fz]:   https://filezilla-project.org/
[wscp]: https://winscp.net/eng/

4.  **Clone the Fossil repository** using the instructions in the
    [`HACKERS.md` file][hack]. (Best for experts or those who wish to
    become experts.)

[hack]: https://tangentsoft.com/pidp8i/doc/trunk/HACKERS.md

5.  **Switch to the binary OS installation images** available from the
    [top-level project page][cprj].  These are default installations of
    Raspbian Lite with the PiDP-8/I software already downloaded, built,
    and installed.  These images were produced in part using option #4
    above, so you can use Fossil to update your software to the current
    version at any time, as long as the Pi is connected to the Internet.


<a id="unpacking"></a>
### Unpacking the Software on Your Pi

Having transferred the distribution file onto your Pi, unpack it with a
command roughly like:

    $ tar xvf /path/to/pidp8i-VERSION.tar.gz

The final argument to that command will vary depending on what you
downloaded, [how you transferred it to the Pi](#transferring), and where
you put it. If you used the `wget` command above, the path is simply
`pidp8i.tar.gz`, for example. After unpacking the tarball, you will have
a new directory beginning with `pidp8i`.  `cd` into that directory, then
proceed with the [configuration](#configuring) steps below.


<a id="help"></a>
### If You Need More Help

If the above material is not sufficient to get you started, you might
want to look at [the documentation][rpfd] provided by the Raspberry
Pi Foundation.  In particular, we recommend their [Linux][rpfl] and
[Raspbian][rpfr] guides.  The book "[Make: Linux for Makers][lfm]"
by Aaron Newcomb is also supposed to be good.


[rpfd]: https://www.raspberrypi.org/documentation/
[rpfl]: https://www.raspberrypi.org/documentation/linux/
[rpfr]: https://www.raspberrypi.org/documentation/raspbian

[lfm]:  https://www.makershed.com/products/make-linux-for-makers


<a id="configuring"></a>
## Configuring, Building and Installing

This software distribution builds and installs in the same way as most
other Linux/Unix software these days.  The short-and-sweet is:

    $ ./configure && make && sudo make install

The `configure` step is generally needed only the first time you build
the software in a new directory.  You may want to add options, described
[below](#options).

Subsequent software updates and rebuilds should not require that you
re-run the `configure` step, even if you gave custom options. The build
system detects when the configuration is outdated and re-runs the
`configure` script with the same options. If for some reason that
automatic re-configuration fails, you can kick it off manually:

    $ make reconfig

The “`make`” step above will take quite a while to run, especially on
the slower Pi boards. The longest single step is building the OS/8 disk
packs from source media. Be patient; the build process almost certainly
isn’t frozen.

If you’re on a multi-core Pi, You can speed the build process up some by
running “`tools/mmake`” instead of directly calling “`make`”. It runs
Make in a mode that lets it do a lot of work in parallel, making better
use of your multiple CPU cores.  That OS/8 media rebuild step is choked
down to a single thread, though, so this won’t help with that.  (This is
a generic, portable script, so if you’re a software developer, feel free
to reuse `mmake` and `corecount` on your own systems.)

Only the `make install` step needs to be done via “`sudo`”.  For the
most part, we’ve tried very hard to minimize the places where you need
to use root privileges to get something done within the PiDP-8/I
software distribution.


<a id="using"></a>
## Using the Software

After running “`sudo make install`” the first time, you will have to log
out and back in to get the installation’s “bin” directory into your
`PATH`.

For the most part, this software distribution works like the [old stable
2015.12.15 distribution][osd]. Its [documentation][oprj] therefore
describes this software too, for the most part. Beyond that, you might
find our [Learning More](/#learning) links helpful.

The largest user-visible difference between the two software
distributions is that many of the shell commands are different:

1.  To start the simulator running in the background:

        $ pidp8i start

    This will happen automatically on reboot unless you disable the
    service, such as in order to run one of the various [forks of Deeper
    Thought][dt2].

2.  To attach the terminal you're working on to the simulator:

        $ pidp8i

    (Yes, it's the same base command as above.  The `pidp8i` script uses
    its first argument to determine what you want it to do.  Without
    arguments, this is what it does.)

3.  To detach from the simulator's terminal interface while leaving the
    PiDP-8/I simulator running, type <kbd>Ctrl-A d</kbd>.  You can
    re-attach to it later with a `pidp8i` command.

4.  To shut the simulator down while attached to its terminal interface,
    type <kbd>Ctrl-E</kbd> to pause the simulator, then at the `simh>`
    prompt type `quit`.  Type `help` at that prompt to get some idea of
    what else you can do with the simulator command language, or read
    the [SIMH Users' Guide][sdoc].

5.  To shut the simulator down from the Raspbian command line:

        $ pidp8i stop

There are [other significant differences][mdif] between the old stable
distribution and this one. You’ll want to be familiar with that
documentation’s content before reading Oscar Vermeulen’s documentation,
as it still refers to his last release in December 2015.


<a id="systemd" name="unit"></a>
## The systemd Unit File

Starting with release 2019.04.25, the PiDP-8/I software distribution is
based on [systemd][systemd], since Raspbian is now on its third
systemd-based release.

One of the features systemd gives us is the ability to set the unit to
run as user-level service rather than as a system-wide service, which
means you no longer need the `sudo` prefix on commands to start, stop,
restart, and query the service. The only time you now need root
privileges when working with the PiDP-8/I software is when installing
it. After that, the software runs under your normal user account, as do
all of the commands you use to manipulate the background simulator
service.

As a result of these changes, none of these commands work any longer:

    $ sudo /etc/init.d/pidp8i start
    $ sudo service pidp8i stop
    $ sudo systemctl restart pidp8i

The correct forms, respectively, are:

    $ systemctl --user start pidp8i
    $ systemctl --user stop pidp8i
    $ systemctl --user restart pidp8i

These commands are long, so we have extended the `pidp8i` command to
build and run `systemctl` commands for you when you pass it arguments:

    $ pidp8i start
    $ pidp8i stop
    $ pidp8i restart
    $ pidp8i status -l

If you run it without arguments, it attaches to the screen manager
session, just as it always has.

The last command above shows that *all* arguments are passed to
`systemctl`, not just the first, so you can pass any flags that
`systemctl` accepts.

Our systemd service starts at boot by default after you install the
software. To disable the service so you can run something else against
the PiDP-8/I front panel hardware instead, such as Deeper Thought 2:

    $ pidp8i stop
    $ pidp8i disable

If you install this release on a system that has the old SysV init
script on it, that service will be disabled and removed before we
install and enable the replacement systemd user service.

[svinit]:  https://en.wikipedia.org/wiki/Init#SysV-style
[systemd]: https://www.freedesktop.org/wiki/Software/systemd/


<a id="options"></a>
## Configure Script Options

You can change many things about the way the software is built and
installed by giving options to the `configure` script:


<a id="prefix"></a>
### --prefix

Perhaps the most widely useful `configure` script option is `--prefix`,
which lets you override the default installation directory, `/opt/pidp8i`.
There are many good reasons to change where the software gets installed,
but the default is also a good one, so unless you know for a fact that
you want to change this default, leave it alone.

For example, you might prefer that the installer put the built software
under your home directory.  This will do that:

    $ ./configure --prefix=$HOME/pidp8i && sudo make install

You might think that installing to a directory your user has complete
control over would remove the need for installing via `sudo`, but that
is not the case, since the installation script needs root privileges to
mark a few of the executables as having permission to run at high priority
levels, which improves the quality of the display, particularly with the
[incandescent lamp simulator][ils] feature enabled.


<a id="lowercase"></a>
### --lowercase

The American Standards Association (predecessor to ANSI) delivered the
second major version of the ASCII character encoding standard the same
year the first PDP-8 came out, 1965. The big new addition? Lowercase.

That bit of history means that when the PDP-8 was new, lowercase was a
fancy new feature in the computing industry. That, plus the memory
savings you get from storing [stripped ASCII][sa] as two 6-bit
characters per 12-bit PDP-8 word means that most PDP-8 software did not
expect to receive lowercase ASCII text, particularly the older software.

The PDP-8 lived long enough to see lowercase ASCII input become common
in the computing industry.

As a result, PDP-8 software reacts in many strange and wonderful
ways when you give it lowercase input. Some software copes nicely,
other software crashes, and some software just sits there dumbly
waiting for you to type something!

This configuration option lets you control how you want your simulated
PDP-8/I to react to lowercase input:

*   **auto** — The default is for the software to attempt to "do the
    right thing." The simulator is configured to send lowercase input to
    the PDP-8 software running on it. Where we have the skill, will,
    need, and time for it, we have [patched][tty] some of the software
    we distribute that otherwise would not do the right thing with
    lowercase input to make it do so.

    This is *not* the option you want if you are a purist.

*   **pass** — This passes keyboard input through to the simulator
    unchanged, and no patches are applied to the PDP-8 software we
    distribute.

    This is the option for historical purists. If you run into trouble
    getting the software to work as you expect when built in this mode,
    try enabling CAPS LOCK.

*   **upper** — This option tells the PDP-8 simulator to turn lowercase
    input into upper case. This is the behavior we used for all versions
    of the PiDP-8/I software up through v2017.04.04.  Essentially, it
    tells the software that you want it to behave as through you've got
    it connected to an uppercase-only terminal like the Teletype Model 33 ASR.

    The advantage of this mode is that you will have no problems running
    PDP-8 software that does not understand lowercase ASCII text.

    The disadvantage is obvious: you won't be able to input lowercase
    ASCII text.

    The SIMH option we enable in this mode is bidirectional, so that if
    you run a program that does emit lowercase ASCII text — such as Rick
    Murphy's version of Adventure — it will be uppercased, just like an
    ASR-33 would do.

    Another trap here is that the C programming language requires
    lowercase text, so you will get a warning if you leave the default
    option **--enable-os8-cc8** set. Pass **--disable-os8-cc8** when
    enabling **upper** mode.

[sa]:  http://homepage.cs.uiowa.edu/~jones/pdp8/faqs/#charsets
[tty]: https://tangentsoft.com/pidp8i/wiki?name=OS/8+Console+TTY+Setup


<a id="nls"></a>
### --no-lamp-simulator

If you build the software on a multi-core host, the PDP-8/I simulator is
normally built with the [incandescent lamp simulator][ils] feature,
which drives the LEDs in a way that mimics the incandescent lamps used
in the original PDP-8/I. (We call this the ILS for short.) This feature
currently takes too much CPU power to run on anything but a multi-core
Raspberry Pi, currently limited to the Pi 2 and Pi 3 series.

If you configure the software on a single-core Pi — models A+, B+, and
Zero — the simulator uses the original low-CPU-usage LED driving method
instead. (a.k.a. NLS for short, named after this configuration option.)

Those on a multi-core host who want this low-CPU-usage LED driving
method can give the `--no-lamp-simulator` option to `configure`.  This
method not only uses less CPU, which may be helpful if you're trying to
run a lot of background tasks on your Pi 2 or Pi 3, it can also be
helpful when the CPU is [heavily throttled][thro].


<a id="serial-mod"></a>
### --serial-mod

If you have done [Oscar's serial mod][sm1] to your PiDP-8/I PCB and the
Raspberry Pi you have connected to it, add `--serial-mod` to the
`configure` command above.

If you do not give this flag at `configure` time with these hardware
modifications in place, the front panel will not work correctly, and
trying to run the software may even crash the Pi.

If you give this flag and your PCBs are *not* modified, most of the
hardware will work correctly, but several lights and switches will not
work correctly.


<a id="alt-serial-mod"></a>
### --alt-serial-mod

This flag is for an [alternative serial mod by James L-W][sm2]. It
doesn't require mods to the Pi, and the mods to the PiDP-8/I board are
different from Oscar's.  This flag changes the GPIO code to work with
these modifications to the PiDP-8/I circuit design.

See the linked mailing list thread for details.

As with `--serial-mod`, you should only enable this flag if you have
actually done the mods as specified by James L-W.

This option is a pure alternative to [`--serial-mod`](#serial-mod): you
can leave both off, but you cannot pass both.


### --throttle

See [`README-throttle.md`][thro] for the values this option takes.  If
you don't give this option, the simulator runs as fast as possible, more
or less.


<a id="savestate"></a>
### --enable-savestate

By default, the PiDP-8/I starts up with the core state undefined and
runs the boot script you’ve selected either with the IF switches or by
passing it on the command line to `pidp8i-sim` or `pdp8`.  This brings
the simulator up in a known state, with no persistence between restarts
other than what was written to the simulated storage devices before the
last shutdown.

On a real PDP-8 with core memory, however, the values in memory will
persist for weeks without power; core memory on a PDP-8 is not zeroed on
power-up, unlike RAM on a modern computer.  Since the CPU doesn’t start
executing anything on power-up in a stock PDP-8 configuration, this
means the user can toggle in a program/OS restart address with the
switch register (SR), load it into the program counter (PC) with the
Load Addr switch, then START the CPU to restart their program without
having to reload it from tape or disk.

There were also several power fail and restart options designed and made
available for the PDP-8 series throughout its lifetime. One of these,
the KP8-I for the PDP-8/I would detect a power fail condition, then in
the brief time window while the power supply’s reservoir capacitors kept
the computer running, this option card would raise an interrupt, giving
a user-written routine up to 1 millisecond to save important registers
to core so they would persist through the power outage. Then on
power-up, it would start executing at core location 00000, where another
user routine would load those registers back from core to restart the
system where it left off before the power failed.

Giving this option gives roughtly the same effect for all generated boot
scripts: any time the simulator is shut down gracefully, it saves all
key simulator state — registers, core, device assignments, etc. — to a
disk file. Then on restart, that script will reload that saved state if
it finds the saved state file.

This is not identical to a KP8-I, in that it doesn’t require any
user-written PDP-8 code to run, which is why it’s optional: it’s
ahistoric with respect to the way the included OSes normally ran.

In absence of a hardware option like the KP8-I, a more accurate
simulation would only save the core memory state to a host-side disk
file and reload it on simulator re-start. You can get that behavior atop
the current mechanism by adding commands like the following to each
bootscript you want to affect:

    EVAL HLT
    DEP L 0
    DEP AC 0
    DEP DF 0
    DEP IF 0
    DEP MQ 0
    DEP PC 0

That zeroes the key registers and prevents the CPU from running as it
normally would after giving the `RESTORE` command to SIMH.


### --disable-usb-automount

When you install the software on a [systemd][systemd]-based Linux
system, we normally configure the OS to automatically mount USB drives
when they are initially plugged in, which allows the `SING_STEP` + `DF`
media image auto-attach feature to work smoothly. That is, if you plug
in a USB memory stick holding a `*.pt` file containing a paper tape
image, you want the simulator to be able to find it if you have the DF
switches set to 1, telling the PiDP-8/I front panel code to look for
something to attach to the simulator's paper tape reader.

This feature may interfere with other uses of USB, such as when booting
your Pi from an external USB hard disk drive. Give this option to
disable the feature.

(Alternately, you could modify our `etc/udev.rules` and/or
`bin/usb-mount` scripts so that they work cooperatively with your local
USB setup rather than conflicting with it.)


### --disable-cc8-cross

Give this option if you do not want to build Ian Schofield's `cc8` C
cross-compiler on the host.

Because the cross-compiler is needed to build the CC8 native OS/8
compiler, disabling the cross-compiler also causes the native compiler
to be left off the bootable OS/8 RK05 disk image, as if you’d passed the
`--disable-os8-cc8` configuration option.


<a id="disable-os8"></a>
### --disable-os8-\*

Several default components of the [OS/8 RK05 disk image](#os8di) used by
boot options IF=0 and IF=7 can be left out to save space and build time:

*   **--disable-os8-advent** — Leave out the [Adventure][advent] game.

*   **--disable-os8-ba** - Leave out the BASIC games and demos which
    come from DEC's book "101 BASIC Computer Games." These are normally
    installed to `RKB0:` as `*.BA`, thus the option's name.

    (We considered naming it `--disable-os8-basic-games-and-demos`, but
    that's too long, and it can't be `--disable-os8-basic` because that
    implies that it is the OS/8 BASIC subsystem that is being left out,
    which is not even currently an option.)

*   **--disable-os8-cc8** - Leave out Ian Schofield's native OS/8 CC8
    compiler and all of its ancillary files.

*   **--disable-os8-chess** — Leave out John Comeau's
    [CHECKMO-II chess implementation][chess].

*   **--disable-os8-crt** — Suppress the [console rubout behavior][tty]
    enabled while building the OS/8 binary RK05 disk image. You
    probably only want to do this if you have attached a real teletype
    to your PiDP-8/I, and thus do not want video terminal style rubout
    processing.

*   **--disable-os8-dcp** — Leave out the DCP disassembler.

*   **--disable-os8-focal** - Do not install any version of FOCAL on the
    OS/8 system disk. This option sets `--disable-os8-uwfocal` and
    overrides `--enable-os8-focal69`, both discussed below.

*   **--disable-os8-fortran-ii** - Leaves out the FORTRAN II compiler,
    SABR, the linking loader (`LOADER`), the `LIBSET` tool, and the
    `*.RL` library files.  This option is ignored unless you also give
    `--disable-os8-cc8` because the OS/8 version of CC8 is built atop
    this subsystem.

*   **--disable-os8-fortran-iv** - Leave the FORTRAN IV compiler out.

*   **--disable-os8-init** - Do not install `RKB0:INIT.TX` or its “show
    on boot” script, `INIT.CM`.  Rather than disable the default on-boot
    init message, you may want to edit `media/os8/init.tx.in` to taste
    and rebuild.

*   **--disable-os8-k12** - Leave out the Kermit-12 implementation
    normally installed to `RKA0:`

*   **--disable-os8-macrel** - Leave the MACREL v2 assembler and its
    associated FUTIL V8B tool out.

*   **--disable-os8-src** - Do not build the `v3d-src.rk05` disk
    image from the OS/8 source tapes.  This is not controlled by
    `--os8-minimal` because that only affects the bootable disk images.

*   **--disable-os8-uwfocal** - Leave out the U/W FOCAL V4E programming
    environment normally installed to `RKA0:`.
    
    Note that the default installation only installs `UWF16K.SV`, not
    the rest of the files on `media/os8/subsys/uwfocal*.tu56`. There is
    much more to explore here, but we cannot include it in the default
    installation set because that would overrun OS/8's limitation on the
    number of files on a volume.

[advent]: http://www.rickmurphy.net/advent
[chess]:  https://chessprogramming.wikispaces.com/CHEKMO-II
[os8p]:   https://tangentsoft.com/pidp8i/doc/trunk/doc/os8-patching.md


<a id="enable-os8"></a>
### --enable-os8-\*

There are a few file sets not normally installed to the [OS/8 RK05 disk
image](#os8di) used by boot options IF=0 and IF=7. You can install them
with the following options:

*   **--enable-os8-music** — The `*.MU` music scores and Rich Wilson's
    associated compiler (`MUSIC.PA`) and player overlay (`PLAYOV.PA`)
    are not normally installed to the built OS/8 binary RK05 disk image
    because the Raspberry Pi reportedly does not emit eufficient RFI at
    AM radio frequencies when running these programs to cause audible
    music on a typical AM radio, the very point of these demos.  Until a
    way is found around this problem — what, low RFI is a *problem* now?
    — this option will default to "off".

*   **--enable-os8-vtedit** — This option installs a default-run macro
    pack called VTEDIT which causes the OS/8 version of TECO to run in
    full-screen mode and to react to [several special keyboard
    commands][uvte] not normally recognized by TECO.

    This feature is disabled by default because the VTEDIT macro pack
    changes the way TECO operates, and many people want TECO to behave
    like *TECO*. VTEDIT was first created during the PDP-8's commercial
    lifetime, so enabling this option is not an anachronism, but TECO is
    much older and had a much more wide-reaching impact in history, so
    we choose to provide unvarnished TECO by default.

    That having been said, people don't go to a ren fair and expect to
    experience the historical ubiquity of typhoid fever, so do not feel
    guilty if you choose to try this option.

[uvte]: https://tangentsoft.com/pidp8i/wiki?name=Using+VTEDIT

*   **--enable-os8-focal69** — Because the default installation includes
    U/W FOCAL, we have chosen to leave [FOCAL,1969][f69] out by default
    to save space on the O/S 8 system disk. You can give this option to
    install this implementation alongside U/W FOCAL, or you can couple
    this option with `--disable-os8-uwfocal` to reverse our choice of
    which FOCAL implementation to install by default.

    You should know that the reason we made this choice is that the
    version of FOCAL,1969 we are currently shipping is fairly minimal: we
    believe we are shipping the original DEC version of FOCAL,1969 plus a
    few carefully-selected overlays. There are many more overlays and
    patches available on the Internet for FOCAL,1969, but we have not had
    time to sort through these and make choices of which ones to ship or
    how to manage which ones get installed. Thus our choice: we want to
    provide the most functional version of FOCAL by default, and within
    the limitations of the time we have chosen to spend on this, that is
    U/W FOCAL today.

    (See our [U/W FOCAL manual supplement][suppd] for a list of
    differences between these versions of FOCAL, which implicitly
    explains why we chose it.)

    It is possible that we will eventually add enough patches and
    overlays to FOCAL,1969 that it will become more powerful than U/W
    FOCAL, so we might then choose to switch the defaults, but that is
    just speculation at the time of this writing.

[f69]:   https://tangentsoft.com/pidp8i/wiki?name=Running+FOCAL%2C1969
[suppd]: https://tangentsoft.com/pidp8i/doc/trunk/doc/uwfocal-manual-supp.md#diffs


### --os8-minimal

If you set this flag, it sets all `--enable-os8-*` flags to false and
all `--disable-os8-*` flags to true.  If you give this along with any
`--enable-os8-*` option, minimal mode overrides it.  Alas, the only way
to get "minimal plus one or two features" is to explicitly disable all
of the optional OS/8 features you don't want.

This flag's name is aspirational, rather than accurate: our current
"minimal" installation could still be stripped down some more.  We
expect to add more `--disable-os8-*` flags later to reduce the delta
between the ideal "minimal OS/8" and our current offering.  These
options would then be included in `--os8-minimal`.  An example of this
is OS/8's BASIC interpreter, which currently cannot be left out.

This option does not control some things you might think it should:

1.  This option does not affect the `--lowercase` option because that
    affects only OS/8's command interpreter and OS/8's BASIC
    implementation, so we deem it to be orthogonal to the purpose of the
    `--os8-minimal` flag, which only affects the optional post-`BUILD`
    features.  If you want a *truly* pristime OS/8 disk, you should
    therefore also give `--lowercase=none`.

2.  This option does not affect `--disable-os8-src`, because it only
    suppresses optional features in the "bin" media.  If you want a
    minimal OS/8 bin disk and no src disk, give that option as well.

3.  Although it disables *display* of the `INIT.TX` file on boot, the
    file is still generated in case you later want to enable it, since
    the file acts as build documentation as well as a "welcome" message.


### --help

Run `./configure --help` for more information on your options here.


<a id="runtime"></a>
## Runtime Configuration

The `pidp8i` command may be configured by the `pidp8i.rc` file,
located by default in `/opt/pidp8i/etc/`.  This is a Bourne shell script
which is sourced by `pidp8i` if it exists which may set the following
variables for the `pidp8i` script to affect how it works:


<a id="rc-screen-manager"></a>
### `SCREEN_MANAGER=screen`

By default, the PiDP-8/I software distribution installs and uses [GNU
`screen(1)`][gscr] to allow the simulator to run in the background yet be
reattached from a later terminal session, then possibly later to be
backgrounded once again. Without the intermediation of something like
`screen`, the simulator would either forever be in the background and
we’d have to export the console [another way][scons] or you’d have to
fire it up interactively any time you wanted to use it. This scheme lets
us have it both ways.

The `SCREEN_MANAGER` setting is for use by those that need something
other than GNU `screen`. There are several alternatives:

*   **screen**: The default, per above.

*   [**tmux**][tmux]: A popular alternative to `screen`, especially on
    on BSD platforms. Note that the "attention" character for `tmux`
    is <kbd>Ctrl-B</kbd> by default, not <kbd>Ctrl-A</kbd> as with
    `screen`.

*   **none**: This mode is for interactive use, allowing you to
    run the installed simulator with the installed media without any
    screen manager at all.

    In this mode, the `pidp8i` and `pidp8i start` commands do the
    same thing: run the simulator directly attached to your current
    interactive terminal. The `pidp8i stop` command becomes a no-op,
    since stopping the simulator is then done in the standard SIMH way:
    <kbd>Ctrl-E, quit</kbd>.

Note that the alternative screen managers are not installed by default.
If you set `SCREEN_MANAGER=tmux`, you must then ensure that `tmux` is in
fact installed before the `pidp8i` script goes to try and use it. On
Raspbian, this is done by:

    $ sudo apt install tmux

Switching between configured screen managers must be done while the
simulator is stopped.

[gscr]:  https://www.gnu.org/software/screen/
[scons]: /wiki?name=Serial+or+Telnet+PDP-8+Console
[tmux]:  https://tmux.github.io/


## <a id="os8di"></a>The OS/8 RK05 Disk Image

For the first several years of the PiDP-8/I project, the OS/8 RK05 disk
image included with the PiDP-8/I software (called `os8.rk05`) was based
on an image of a real RK05 disk pack that someone allegedly found in a
salvaged PDP-8 system.  Parts of the image were corrupt, and not all of
the pieces of software on the disk worked properly with the other parts.
It was also a reflection of the time it was created and used out in the
world, which was not always what we would wish to use today.

In late 2017 [several of us][aut] created the `mkos8` tool, which was
replaced during 2018 by Bill Cattey with the `os8-run` interpreter
and its stock scripts. The `--*-os8-*` options documented above get
passed into `os8-run` during the PiDP-8/I software build process,
which controls how it generates the `v3d*.rk05` RK05 disk image files.

This set of disk images entirely replaces the old `os8.rk05` disk image,
in that all features of the old disk image are still available. In some
cases, features present on the old `os8.rk05` disk are now left out or
disabled by default, and in other cases we have changed the behavior of
features from the way they used to be on the old disk. Mostly, though,
the new disk images are simply more functional than the old ones.

If you wish to know the full details of how these disk images are
created, see the documentation for [`os8-run`][ori] and that for
[`class simh`][cs].

The remainder of this section describes some aspects of these disk
images which are not clear from the descriptions of the `--*-os8-*`
configuration options above.

[aut]: https://tangentsoft.com/pidp8i/doc/trunk/AUTHORS.md
[ori]: https://tangentsoft.com/pidp8i/doc/trunk/doc/os8-run.md


### Baseline

The baseline for the bootable OS/8 disk images comes from a set of
DECtapes distributed by Digital Equipment Corporation which are now
included with the PiDP-8/I software; see the [`media/os8/*.tu56`
files][os8mf]. From these files and your configuration options, the
`os8-run` script creates the baseline `v3d-dist.rk05` disk image.

The default build creates a complete OS/8 V3D system including `BUILD`
support, FORTRAN IV, MACREL v2, and more.


### Subtractions

It turns out that it's pretty easy to run out of directory space on an
OS/8 RK05 disk due to a limitation in the number of files on an OS/8
filesystem.  For this reason, the archive of device drivers and TD8E
system are left off the system packs.  They can be found in [OS/8
Binary Distribution DECtape #2][bdt2].

If you do fancy work with `BUILD`, you may need to attach that DECtape
image and copy files in from it.


### Default Additions

The OS/8 RK05 disk image build process normally installs many software
and data file sets to the disk image.  See the option descriptions
above: the ["disable" option set](#disable-os8) effectively lists those
packages that are installed by default, and the following set of
["enable" option set](#enable-os8) lists those left out by default.


### Console Enhancements

The default build enhances the console in a few ways:

1.  The SIMH PDP-8 simulator and a few select parts of OS/8 are adjusted
    to cope with lowercase input to [varying degrees](#lowercase).

2.  Rubout/backspace handling is set to assume a video terminal rather
    than a teletype by default.

You can read more about this [in the wiki][oce].

[oce]: https://tangentsoft.com/pidp8i/wiki?name=OS/8+Console+TTY+Setup


### Patches

The `v3d-dist.rk05` disk image referenced above is considered a
read-only master. A copy is made called `v3d.rk05`. This is the
default OS/8 rk05 image assigned to the IF=0 and IF=7 boot options.

In keeping with the standards of good systm management
this image incorporates all mandatory patches, as well as
optional patches appropriate to proper operation of the system.
For details on the available patches, the selection criteria,
and information about other optional patches see the [OS/8 system patches][os8p]
document.

[bdt2]:  https://tangentsoft.com/pidp8i/file/media/os8/al-4712c-ba-os8-v3d-2.1978.tu56
[cl]:    https://tangentsoft.com/pidp8i/doc/trunk/ChangeLog.md
[cs]:    https://tangentsoft.com/pidp8i/doc/trunk/doc/class-simh.md
[os8mf]: https://tangentsoft.com/pidp8i/file/media/os8
[tlrm]:  https://tangentsoft.com/pidp8i/doc/trunk/README.md


## <a id="os8ti"></a>The OS/8 TU56 Tape Image

As with the [OS/8 disk image](#os8di), this distribution’s build system
can create custom TU56 tape images from pristine source media.  This
replaces the old `os8.tu56` binary image previously distributed with
this software.

The build system actually creates four such tape images according to a
2×2 matrix of choices:

*   **--boot-tape-version** — The default value is “`v3f`”, meaning
    that the boot tape is based on OS/8 V3F. Give “`v3d`” to boot from
    a OS/8 V3D tape instead. See the wiki article [OS/8 V3D vs
    V3F][os8ver] for the implications of this choice.

*    **--boot-tape-config** — The default value is “`tc08`”. Give
    “`td12k`” to use a tape image configured with the TD12K DECtape
    controller driver built in. See the wiki article [TC08 vs
    TD12K][os8td] for the reason you’re given a choice here.

[os8td]:  https://tangentsoft.com/pidp8i/wiki?name=TD8E+vs+TC08
[os8ver]: https://tangentsoft.com/pidp8i/wiki?name=OS/8+V3D+vs+V3F



<a id="overwrite-setup"></a>
## Overwriting the Local Simulator Setup

When you run `sudo make install` step on a system that already has an
existing installation, it purposely does not overwrite two classes of
files:

1.  **The binary PDP-8 media files**, such as the RK05 disk image that
    holds the OS/8 image the simulator boots from by default. These media
    image files are considered "precious" because you may have modified
    the OS configuration or saved personal files to the disk the OS
    boots from, which in turn modifies this media image file out in the
    host operating environment.

2.  **The PDP-8 simulator configuration files**, installed as
    `$prefix/share/boot/*.script`, which may similarly have local
    changes, and thus be precious to you.

Sometimes this "protect the precious" behavior isn't what you want.
(Gollum!) One common reason this may be the case is that you've damaged
your local configuration and want to start over. Another common case is
that the newer software you're installing contains changes that you want
to reflect into your local configuration.

You have several options here:

1.  If you just want to reflect the prior PDP-8 simulator configuration
    file changes into your local versions, you can hand-edit the
    installed simulator configuration scripts to match the changes in
    the newly-generated `boot/*.script` files under the build directory.

2.  If the change is to the binary PDP-8 media image files — including
    the [generated OS/8 disk images](#os8di) — and you're unwilling to
    overwrite your existing ones wholesale, you'll have to mount both
    versions of the media image files under the PDP-8 simulator and copy
    the changes over by hand.

3.  If your previously installed binary OS media images — e.g. the
    [OS/8 RK05 disk image](#os8di) that the simulator boots from by
    default — are precious but the simulator configuration scripts
    aren't precious, you can just copy the generated `boot/*.script`
    files from the build directory into the installation directory,
    `$prefix/share/boot`.  (See the `--prefix` option above for the
    meaning of `$prefix`.)

4.  If neither your previously installed simulator configuration files
    nor the binary media images are precious, you can force the
    installation script to overwrite them both with a `sudo make
    mediainstall` command after `sudo make install`.

    Beware that this is potentially destructive! If you've made changes
    to your PDP-8 operating systems or have saved files to your OS
    system disks, this option will overwrite those changes!


<a id="testing"></a>
## Testing Your PiDP-8/I Hardware

You can test your PiDP-8/I LED and switch functions with these commands:

    $ pidp8i stop
    $ pidp8i-test

You may have to log out and back in before the second command will work,
since the installation script modifies your normal user's `PATH` the
first time you install onto a given system.

It is important to stop the PiDP-8/I simulator before running the test
program, since both programs need exclusive access to the LEDs and
switches on the front panel.  After you are done testing, you can start
the PiDP-8/I simulator back up with:

    $ pidp8i start

See [its documentation][test] for more details.


<a id="sshd"></a>
## Enabling the SSH Server on the Binary OS Images

The OpenSSH server is enabled and running by default on the PiDP-8/I
binary OS images, but for security reasons, the build process we use
to create these OS images wipes out the SSH host keys generated here
on our build system, else everyone's PiDP-8/I would have the same
host keys, which would be a massive security hole. Unfortunately,
the Raspbian `sshd` service is not smart enough to regenerate these
keys if they are missing on boot, so you need to do this once by hand:

    $ sudo dpkg-reconfigure openssh-server

You should be able to log in via SSH immediately after that command
completes.

You may be wondering why we don’t do this automatically, somehow. It’s
because, over time, we’ve removed all dependence on root access in our
software in the name of security, so that we no longer have permission
to make system-wide changes like this in our startup scripts.  We now
rely on you, the system’s administrator, to do it interactively with
`sudo` permissions.


## License

Copyright © 2016-2020 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].


[cprj]: https://tangentsoft.com/pidp8i/
[sm1]:  http://obsolescence.wixsite.com/obsolescence/2016-pidp-8-building-instructions
[sm2]:  https://groups.google.com/d/msg/pidp-8/-leCRMKqI1Q/Dy5RiELIFAAJ
[osd]:  http://obsolescence.wixsite.com/obsolescence/pidp-8-details
[dt2]:  https://github.com/VentureKing/Deeper-Thought-2
[sdoc]: https://tangentsoft.com/pidp8i/uv/doc/simh/main.pdf
[oprj]: http://obsolescence.wixsite.com/obsolescence/pidp-8
[test]: https://tangentsoft.com/pidp8i/doc/trunk/doc/pidp8i-test.md
[thro]: https://tangentsoft.com/pidp8i/doc/trunk/README-throttle.md
[mdif]: https://tangentsoft.com/pidp8i/wiki?name=Major+Differences
[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[ils]:  https://tangentsoft.com/pidp8i/wiki?name=Incandescent+Lamp+Simulator
