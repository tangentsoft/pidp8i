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


#### --no-idle

By default, the PDP-8 simulator configuration files are generated with
the PDP-8 CPU idling option set appropriately for your configuration.
Idling causes the simulator to go into a low-CPU usage mode when it
detects that the simulated PDP-8 software isn't doing any real work,
such as spinning in a tight loop waiting for a keypress.

Idling is incompatible with the incandescent lamp simulator (ILS)
because it throws off the timing used to calculate the LED brightness
values, so when building with the ILS, the build system implicitly sets
the `--no-idle` option. This option therefore only takes effect when
building with the `--no-lamp-simulator` option or when the ILS is
automatically disabled, as when configuring the software on a
single-core Raspberry Pi. See the next item for details.

If you're:

1.  updating an installation made before 2017.03.30;

2.  it uses the ILS feature; and

3.  you have added any `set cpu idle` options in your PDP-8
    confifguration scripts

...see the "Overwriting the Local Simulator Setup" section below.

You may also need to use one of the solutions in that section if you
first install with the NLS enabled, then later decide that you want to
try the ILS.

You know you need to fix your local PDP-8 simulator configuration if
you're using the ILS and the display is correct only while the simulated
PDP-8 is doing real work. If the display dims to zero brightness and
then flutters between off, dim, and on states seemingly randomly while
the PDP-8 is idle, you've got a `set cpu idle` setting somehwere. Again,
see the "Overwriting the Local Simulator Setup" section for the options
you have to fix this.

The only practical reason I know of for setting `--no-idle` in NLS mode
is that it can result in slightly higher SIMH benchmark results when
running on a multi-core Pi without any [throttled][thro] restriction.

tl;dr: You probably don't need to give this option, ever.


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
that the newer softare you're installing contains changes that you want
to reflect into your local configuration.

You have several options here:

1.  If you just want to reflect upstream PDP-8 simulator configuration
    file changes into your local versions, you can hand-edit the
    installed simulator configuration scripts to match the changes in
    the newly-generated `boot/*.script` files under the build directory.

2.  If the upstream change you want to merge into your local
    configuration is to the OS media instead, you'll have to mount both
    versions of the media image files under the PDP-8 simulator and copy
    the changes over by hand, if you're unwilling to overwrite the media
    image files wholesale.

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
[sdoc]: http://simh.trailing-edge.com/pdf/simh_doc.pdf
[prj]:  http://obsolescence.wixsite.com/obsolescence/pidp-8
[test]: https://tangentsoft.com/pidp8i/doc/trunk/doc/pidp8i-test.md
[thro]: https://tangentsoft.com/pidp8i/doc/trunk/README-throttle.md
[mdif]: https://tangentsoft.com/pidp8i/wiki?name=Major+Differences
[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[ils]:  https://tangentsoft.com/pidp8i/wiki?name=Incandescent+Lamp+Simulator
