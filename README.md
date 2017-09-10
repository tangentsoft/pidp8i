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


## Configuring, Building and Installing

This software distribution builds and installs in the same way as most
other Linux/Unix software these days.  The short-and-sweet is:

    $ ./configure && make && sudo make install

If you've checked out a new version of the source code and the `make`
step fails, try redoing the `configure` step. Sometimes changes made to
the source code invalidate prior `make` dependencies, which are
implicitly repaired by the `configure` script.


### Configure Script Options

You can change a few things about the way the software is built and
installed by giving options to the `configure` script:


#### --prefix

Perhaps the most widely useful `configure` script option is `--prefix`,
which lets you override the default installation directory,
`/opt/pidp8i`.  You could make it install the software under your home
directory on the Pi with this command:

    $ ./configure --prefix=$HOME/pidp8i && sudo make install

Although this is installing to a directory your user has write access
to, you still need to install via `sudo` because the installation
process does other things that do require `root` access.


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


#### --enable-\* or --with-\*

There are a few file sets not normally installed to the OS/8 RK05 disk
image used by boot options IF=0 and IF=7. You can install them with the
following options:

*   **--with-music** — The `*.MU` files and the player program for it
    are not normally installed to the built OS/8 binary RK05 disk image
    because the Raspberry Pi reportedly does not emit eufficient RFI at
    AM radio frequencies when running these programs to cause audible
    music on an AM radio, the very point of these demos. Until a way is
    found around this problem — what, low RFI is a *problem* now? — this
    option will default to "off".

*   **--with-vtedit** — This option installs a default-run macro pack
    called VTEDIT which causes the OS/8 version of TECO to run in
    full-screen mode and to react to [several special keyboard
    commands](/wiki?name=Using+VTEDIT) not normally recognized by TEDO.

    This feature is currently disabled because it is not yet fully
    tested by the person in charge of the OS/8 disk building process.

    It may remain disabled after that because it changes the behavior of
    the `TECO` and `EDIT` command in OS/8, which violates the
    expectations of people expecting a historically accurate TECO
    experience. On the other hand, people don't go to a ren fair and
    expect to experience the historical ubiquity of typhoid fever
    either, so we might change our mind on this.

The `configure` script is flexible about option naming.
`--enable-music`, `--music`, and `--with-music` are equivalent.


#### --disable-\* or --without-\*

Several components of the built OS/8 binary RK05 disk image which are
installed by default can be left out of the build process to save space
and build time by giving one of these flags:

*   **--without-advent** — Leave out the Adventure game.

*   **--without-ba** - Leave out the BASIC games and demos, which come
    from DEC's book "101 BASIC Computer Games." These are normally
    installed to `RKB0:` as `*.BA`, thus the option's name.

    (We considered naming it `--without-basic-games-and-demos`, but
    that's too long, and it can't be `--without-basic` because that
    implies that it is the OS/8 BASIC subsystem that is being left out,
    which is not the case.)

*   **--without-cc8** - Leave out Ian Schofield's native OS/8 CC8
    compiler normally installed to `RKA0:`

*   **--without-chess** — Leave out John Comeau's CHECKMO-II chess
    implementation.

*   **--without-crt** — Suppress the [console rubout behavior][tty]
    normally applied by default. You probably only want to do this if
    you have attached a real teletype to your PiDP-8/I, and thus do not
    want video terminal style rubout processing.

*   **--without-k12** - Leave out the 12-bit Kermit implementation
    normally installed to `RKA0:`

*   **--without-lcmod** — Suppress the [lowercase console mod][tty]
   normally applied by default.

The `configure` script accepts both the `--disable-*` and `--without-*`
forms of these options.

[tty]: https://tangentsoft.com/pidp8i/wiki?name=OS/8+Console+TTY+Setup


#### --help

Run `./configure --help` for more information on your options here.


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
