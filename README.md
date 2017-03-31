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


#### --no-idle

By default, the PDP-8 simulator configuration files are generated with
the CPU idling option set appropriately for your configuration. Idling
causes the simulator to go into a low-CPU usage mode when it detects
that the simulated PDP-8 software isn't doing anything CPU-critical,
like waiting for a keypress.

Idling is incompatible with the incandescent lamp simulator (ILS)
because it throws off the timing used to calculate the LED brightness
values, so when building with the ILS, the build system implicitly sets
the `--no-idle` option. This option therefore only takes effect when
building with the `--no-lamp-simulator` option or when the ILS is
automatically disabled, as when configuring the software on a
single-core Raspberry Pi. See the next item for details.

This automatic `--no-idle` feature only went into effect on 2017.03.30,
and the changes it makes aren't normally updated as part of the normal
build-and-install sequence because it involves overwriting files you may
consider precious. Specifically, the SIMH simulator configuration
scripts, installed at `$prefix/share/boot/*.script`. If you want to
enforce no-idle behavior on an existing installation, you therefore have
several options:

1.  Hand-edit the installed SIMH scripts to match the changes in the
    newly-generated `boot/*.script` files in the build directory; or

2.  If your previously installed binary OS media images — e.g. the OS/8
    RK05 disk image that the simulator boots from by default — is
    precious but the SIMH configuration scripts aren't precious, you can
    copy the `boot/*.script` files over the top of your existing
    `$prefix/share/boot/*.script` files; or

3.  If neither your SIMH configuration files nor the binary OS media
    images are precious, you can force the installer to overwrite them
    with a `sudo make mediainstall` command after `sudo make install`.
    Beware that this is potentially destructive! If you've made changes
    to your PDP-8 operating systems or have saved files to your OS
    system disks, this option will overwrite those changes.

The only practical reason I know of for setting `--no-idle` in NLS mode
is that it can result in slightly higher SIMH benchmark results. I don't
know if it actually makes the simulator run faster, particularly when
it's been [throttled][thro], as it always will be when built on a
single-core Pi, which also means you're running in NLS mode. Therefore,
it may only have this tiny benchmarking effect when running on a
multi-core Pi, completely unthrottled.

tl;dr: You probably don't need to give this option, ever.


#### --no-lamp-simulator

If you build the software on a multi-core host, the PDP-8/I simulator is
normally built with the [incandescent lamp simulator][ils] feature,
which drives the LEDs in a way that mimics the incandescent lamps used
in the original PDP-8/I.  This feature currently takes too much CPU
power to run on anything but a multi-core Raspberry Pi, currently
limited to the Pi 2 and Pi 3 series.

If you configure the software on a single-core Pi — models A+, B+, and
Zero — the simulator uses the original low-CPU-usage LED driving method
instead.

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


#### --help

Run `./configure --help` for more information on your options here.


### Installing

The `sudo make install` step in the command above does what most people
want.

That step will not overwrite the operating system and program media
(e.g. the OS/8 RK05 disk cartridge image) when installing multiple times
to the same location, but you can demand an overwrite with:

    $ sudo make mediainstall

This can be helpful if you have damaged your OS/program media or simply
want to return to the pristine versions as distributed.

This will also overwrite the boot scripts in `$prefix/share/boot` with
fresh versions from the source distribution.


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
[sdoc]: http://simh.trailing-edge.com/pdf/simh_doc.pdf
[prj]:  http://obsolescence.wixsite.com/obsolescence/pidp-8
[test]: https://tangentsoft.com/pidp8i/doc/trunk/doc/pidp8i-test.md
[thro]: https://tangentsoft.com/pidp8i/doc/trunk/README-throttle.md
[mdif]: https://tangentsoft.com/pidp8i/wiki?name=Major+Differences
[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[ils]:  https://tangentsoft.com/pidp8i/wiki?name=Incandescent+Lamp+Simulator
