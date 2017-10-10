# Getting Started with the PiDP-8/I Software

## Prerequisites

*   A Raspberry Pi with the 40-pin GPIO connector.  That rules out the
    first-generation Raspberry Pi model A and B boards which had a
    26-pin GPIO connector.

*   An SD card containing Raspbian or something sufficiently close.
    This software is currently tested with the Jessie Lite distribution.

    Ideally, you will install a fresh OS image onto an unused SD card
    rather than use this software to modify an existing OS installation,
    but there is currently no known hard incompatibilty that prevents
    you from integrating this software into an existing OS.

*   This software distribution, unpacked somewhere convenient within the
    Raspberry Pi filesystem.

    Unlike with the [upstream 2015.12.15 release][upst], this present
    release of the software should *not* be unpacked into `/opt/pidp8`.
    I recommend that you unpack it into `$HOME/src`, `/usr/local/src` or
    similar, but it really doesn't matter where you put it, as long as
    your user has full write access to that directory.

*   A working C compiler and other standard Linux build tools, such as
    `make(1)`.  On Debian type systems — including Raspbian — you can
    install such tools with `sudo apt install build-essential`


<a name="configuring"></a>
## Configuring, Building and Installing

This software distribution builds and installs in the same way as most
other Linux/Unix software these days.  The short-and-sweet is:

    $ ./configure && make && sudo make install

If you've checked out a new version of the source code and the `make`
step fails, try redoing the `configure` step. Sometimes changes made to
the source code invalidate prior `make` dependencies, which are
implicitly repaired by the `configure` script.


<a name="options"></a>
### Configure Script Options

You can change a few things about the way the software is built and
installed by giving options to the `configure` script:


<a name="prefix"></a>
#### --prefix

Perhaps the most widely useful `configure` script option is `--prefix`,
which lets you override the default installation directory,
`/opt/pidp8i`.  You could make it install the software under your home
directory on the Pi with this command:

    $ ./configure --prefix=$HOME/pidp8i && sudo make install

Although this is installing to a directory your user has write access
to, you still need to install via `sudo` because the installation
process does other things that do require `root` access.


<a name="lowercase"></a>
#### --lowercase

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

As a result, PDP-8 software reacts in many strange and wonderful ways
when you give it lowercase input. Some software copes nicely, others
crashes, and some software just sits there dumbly waiting for you to
type something!

This configuration option lets you control how you want your simulated
PDP-8/I to react to lowercase input:

*   **auto** — The default is for the software to attempt to "do the
    right thing." The simulator is configured to send lowercase input to
    the PDP-8 software running on it. Where we have the skill, will,
    need, and time for it, we have [patched][tty] some of the software
    we distribute that otherwise would not do the right thing with
    lowercase input to make it do so.

    This is *not* the option you want if you are a purist.

*   **upper** — This option tells the PDP-8 simulator to turn lowercase
    input into upper case. This is the behavior we used for all versions
    of the PiDP-8/I software up through v2017.04.04.  Essentially, it
    tells the software that you want it to behave as through you've got
    it connected to a Teletype Model 33 ASR.

    The advantage of this mode is that you will have no problems running
    PDP-8 software that does not understand lowercase ASCII text.

    The disadvantage is obvious: you won't be able to input lowercase
    ASCII text.  The SIMH option we enable in this mode is
    bidirectional, so that if you run a program that does emit lowercase
    ASCII text — such as Rick Murphy's version of Adventure — it will be
    uppercased, just like an ASR-33 would do.

    Another trap here is that the C programming language requires
    lowercase text, so you will get a warning if you leave the default
    option **--enable-os8-cc8** set. Pass **--disable-os8-cc8** when
    enabling **upper** mode.

*   **none** — This passes 7-bit ASCII text through to the software
    running on the simulator unchanged, and no patches are applied to
    the PDP-8 software we distribute.

    This is the option for historical purists. If you run into trouble
    getting the software to work as you expect when built in this mode,
    try enabling CAPS LOCK.

[sa]:  http://homepage.cs.uiowa.edu/~jones/pdp8/faqs/#charsets
[tty]: https://tangentsoft.com/pidp8i/wiki?name=OS/8+Console+TTY+Setup


<a name="nls"></a>
#### --no-lamp-simulator

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


<a name="serial-mod"></a>
#### --serial-mod

If you have done [Oscar's serial mod][sm1] to your PiDP-8/I PCB and the
Raspberry Pi you have connected to it, add `--serial-mod` to the
`configure` command above.

If you do not give this flag at `configure` time with these hardware
modifications in place, the front panel will not work correctly, and
trying to run the software may even crash the Pi.

If you give this flag and your PCBs are *not* modified, most of the
hardware will work correctly, but several lights and switches will not
work correctly.


<a name="alt-serial-mod"></a>
#### --alt-serial-mod

This flag is for an [alternative serial mod by James L-W][sm2]. It
doesn't require mods to the Pi, and the mods to the PiDP-8/I board are
different from Oscar's.  This flag changes the GPIO code to work with
these modifications to the PiDP-8/I circuit design.

See the linked mailing list thread for details.

As with `--serial-mod`, you should only enable this flag if you have
actually done the mods as specified by James L-W.


#### --throttle

See [`README-throttle.md`][thro] for the values this option takes.  If
you don't give this option, the simulator runs as fast as possible, more
or less.


#### --disable-cc8-cross

Give this option if you do not want to build Ian Schofield's `cc8` C
cross-compiler on the host.

Note that this is different from `--disable-os8-cc8`, which disables the
*native OS/8* C compiler. They are two different C compilers: one runs
outside the SIMH PDP-8 simulator and the other runs inside the simulator
under OS/8.


<a name="disable-os8"></a>
#### --disable-os8-\*

Several default components of the OS/8 RK05 disk image used by boot
options IF=0 and IF=7 can be left out to save space and build time:

*   **--disable-os8-advent** — Leave out the Adventure game.

*   **--disable-os8-ba** - Leave out the BASIC games and demos which
    come from DEC's book "101 BASIC Computer Games." These are normally
    installed to `RKB0:` as `*.BA`, thus the option's name.

    (We considered naming it `--disable-os8-basic-games-and-demos`, but
    that's too long, and it can't be `--disable-os8-basic` because that
    implies that it is the OS/8 BASIC subsystem that is being left out,
    which is not even currently an option.)

*   **--disable-os8-chess** — Leave out John Comeau's CHECKMO-II chess
    implementation.

*   **--disable-os8-cc8** - Leave out Ian Schofield's native OS/8 CC8
    compiler normally installed to `RKA0:`

*   **--disable-os8-crt** — Suppress the [console rubout behavior][tty]
    enabled while building the OS/8 binary RK05 disk image. You
    probably only want to do this if you have attached a real teletype
    to your PiDP-8/I, and thus do not want video terminal style rubout
    processing.

*   **--disable-os8-focal** - Do not install any version of FOCAL on the
    OS/8 system disk. This option sets `--disable-os8-uwfocal` and
    overrides `--enable-os8-focal69`, both discussed below.

*   **--disable-os8-init** - Generate `SYS:INIT.TX` but do not display
    it on OS/8 boot.  Rather than disable the default on-boot init
    message, you may want to edit `media/os8/init.tx.in` to taste and
    rebuild.

*   **--disable-os8-k12** - Leave out the Kermit-12 implementation
    normally installed to `RKA0:`

*   **--disable-os8-src** - Do not build the `os8v3d-src.rk05` disk
    image from the OS/8 source tapes.  This is not controlled by
    `--os8-minimal` because that only affects `os8v3d-bin.rk05`.

*   **--disable-os8-uwfocal** - Leave out the U/W FOCAL V4E programming
    environment normally installed to `RKA0:`.
    
    Note that the default installation only installs `UWF16K.SV`, not
    the rest of the files on `media/os8/subsys/uwfocal*.tu56`. There is
    much more to explore here, but we cannot include it in the default
    installation set because that would overrun OS/8's limitation on the
    number of files on a volume.


<a name="enable-os8"></a>
#### --enable-os8-\*

There are a few file sets not normally installed to the OS/8 RK05 disk
image used by boot options IF=0 and IF=7. You can install them with the
following options:

*   **--enable-os8-music** — The `*.MU` files and the player program for
    it are not normally installed to the built OS/8 binary RK05 disk
    image because the Raspberry Pi reportedly does not emit eufficient
    RFI at AM radio frequencies when running these programs to cause
    audible music on a typical AM radio, the very point of these demos.
    Until a way is found around this problem — what, low RFI is a
    *problem* now? — this option will default to "off".

*   **--enable-os8-vtedit** — This option installs a default-run macro
    pack called VTEDIT which causes the OS/8 version of TECO to run in
    full-screen mode and to react to [several special keyboard
    commands](/wiki?name=Using+VTEDIT) not normally recognized by TEDO.

    This feature is currently disabled because it is not yet fully
    tested by the person in charge of the OS/8 disk building process.

    It may remain disabled after that because it changes the behavior of
    the `TECO` command in OS/8, which violates the expectations of
    people expecting a historically accurate TECO experience. On the
    other hand, people don't go to a ren fair and expect to experience
    the historical ubiquity of typhoid fever either, so we might change
    our mind on this.

*   **--enable-os8-focal69** — Because the default installation includes
    U/W FOCAL, we have chosen to leave FOCAL 69 out by default to save
    space on the O/S 8 system disk. You can give this option to install
    this implementation alongside U/W FOCAL, or you can couple this
    option with `--disable-os8-uwfocal` to reverse our choice of which
    FOCAL implementation to install by default.

    You should know that the reason we made this choice is that the
    version of FOCAL 69 we are currently shipping is fairly minimal: we
    believe we are shipping the original DEC version of FOCAL 69 plus a
    few carefully-selected overlays. There are many more overlays and
    patches available on the Internet for FOCAL 69, but we have not had
    time to sort through these and make choices of which ones to ship or
    how to manage which ones get installed. Thus our choice: we want to
    provide the most functional version of FOCAL by default, and within
    the limitations of the time we have chosen to spend on this, that is
    U/W FOCAL today.

    (See our [U/W FOCAL manual supplement][suppd] for a list of
    differences between these versions of FOCAL, which implicitly
    explains why we chose it.)

    It is possible that we will eventually add enough patches and
    overlays to FOCAL 69 that it will become more powerful than U/W
    FOCAL, so we might then choose to switch the defaults, but that is
    just speculation at the time of this writing.

[suppd]: https://tangentsoft.com/pidp8i/doc/trunk/doc/uwfocal-manual-supp.md#diffs


#### --os8-minimal

If you set this flag, it sets all `--enable-os8-*` flags to false and
all `--disable-os8-*` flags to true. If you explicitly give any of these
flags to the `configure` script, this flag overrides them.

This flag only affects the optional installs made after the `BUILD`
step: it does not remove optional features of OS/8 itself, such as its
BASIC interpreter.

Giving this option therefore gets you an empty OS/8 `DSK:` device and
nothing in `SYS:` beyond what was left after the OS/8 `BUILD` step.

There are a few exceptions:

1.  This option does not affect the `--lowercase` option because that
    affects only OS/8's command interpreter and OS/8's BASIC
    implementation, so we deem it to be orthogonal to the purpose of the
    `--os8-minimal` flag, which only affects the optional post-`BUILD`
    features. If you want a *truly* pristime OS/8 disk, you should
    therefore also give `--lowercase=none`.

2.  This option does not affect `--disable-os8-src`, because it only
    suppresses optional features in the "bin" media.  If you want a
    minimal OS/8 bin disk and no src disk, give that option as well.


#### --help

Run `./configure --help` for more information on your options here.


<a name="overwrite-setup"></a>
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

1.  If you just want to reflect upstream PDP-8 simulator configuration
    file changes into your local versions, you can hand-edit the
    installed simulator configuration scripts to match the changes in
    the newly-generated `boot/*.script` files under the build directory.

2.  If the upstream change is to the binary PDP-8 media image files and
    you're unwilling to overwrite them wholesale, you'll have to mount
    both versions of the media image files under the PDP-8 simulator and
    copy the changes over by hand.

3.  If your previously installed binary OS media images — e.g. the OS/8
    RK05 disk image that the simulator boots from by default — are
    precious but the simulator configuration scripts aren't precious,
    you can just copy the generated `boot/*.script` files from the build
    directory into the installation directory, `$prefix/share/boot`.
    (See the `--prefix` option above for the meaning of `$prefix`.)

4.  If neither your previously installed simulator configuration files
    nor the binary media images are precious, you can force the
    installation script to overwrite them both with a `sudo make
    mediainstall` command after `sudo make install`.

    Beware that this is potentially destructive! If you've made changes
    to your PDP-8 operating systems or have saved files to your OS
    system disks, this option will overwrite those changes!


<a name="testing"></a>
## Testing

You can test your PiDP-8/I LED and switch functions with these commands:

    $ sudo systemctl stop pidp8i
    $ pidp8i-test

You may have to log out and back in before the second command will work,
since the installation script modifies your normal user's `PATH` the
first time you install onto a given system.

It is important to stop the PiDP-8/I simulator before running the test
program, since both programs need exclusive access to the LEDs and
switches on the front panel.  After you are done testing, you can start
the PiDP-8/I simulator back up with:

    $ sudo systemctl start pidp8i

See [its documentation][test] for more details.


<a name="using"></a>
## Using the Software

For the most part, this software distribution works like the upstream
[2015.12.15 distribution][usd].  Its [documentation][prj] therefore
describes this software too, for the most part.

The largest user-visible difference between the two software
distributions is that all of the shell commands affecting the software
were renamed to include `pidp8i` in their name:

1.  To start the simulator:

        $ sudo systemctl start pidp8i

    This will happen automatically on reboot unless you disable the
    service, such as in order to run one of the various [forks of Deeper
    Thought][dt2].

2.  To attach the terminal you're working on to the simulator:

        $ pidp8i

3.  To detach from the simulator's terminal interface while leaving the
    PiDP-8/I simulator running, type <kbd>Ctrl-A d</kbd>.  You can
    re-attach to it later with a `pidp8i` command.

4.  To shut the simulator down while attached to its terminal interface,
    type <kbd>Ctrl-E</kbd> to pause the simulator, then at the `simh>`
    prompt type `quit`.  Type `help` at that prompt to get some idea of
    what else you can do with the simulator command language, or read
    the [SIMH Users' Guide][sdoc].

5.  To shut the simulator down from the Raspbian command line:

        $ sudo systemctl stop pidp8i

There are [other major differences][mdif] between the upstream
distribution and this one.  See that linked wiki article for details.


## License

Copyright © 2016-2017 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].


[prj]:  https://tangentsoft.com/pidp8i/
[upst]: http://obsolescence.wixsite.com/obsolescence/pidp-8
[sm1]:  http://obsolescence.wixsite.com/obsolescence/2016-pidp-8-building-instructions
[sm2]:  https://groups.google.com/d/msg/pidp-8/-leCRMKqI1Q/Dy5RiELIFAAJ
[usd]:  http://obsolescence.wixsite.com/obsolescence/pidp-8-details
[dt2]:  https://github.com/VentureKing/Deeper-Thought-2
[sdoc]: https://tangentsoft.com/pidp8i/uv/doc/simh/main.pdf
[prj]:  http://obsolescence.wixsite.com/obsolescence/pidp-8
[test]: https://tangentsoft.com/pidp8i/doc/trunk/doc/pidp8i-test.md
[thro]: https://tangentsoft.com/pidp8i/doc/trunk/README-throttle.md
[mdif]: https://tangentsoft.com/pidp8i/wiki?name=Major+Differences
[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[ils]:  https://tangentsoft.com/pidp8i/wiki?name=Incandescent+Lamp+Simulator
