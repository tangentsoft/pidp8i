# Getting Started with the PiDP-8/I Software

## Prerequisites

*   A Raspberry Pi with the 40-pin GPIO connector. That rules out the
    first series of Raspberry Pi boards with the 26-pin connector.

*   An SD card containing Raspbian or something sufficiently close.
    PipaOS may also work, for example.  This software is currently
    tested with the Jessie Lite distribution.

    Ideally, you will install a fresh OS image onto an unused SD
    card rather than use this software to modify an existing OS
    installation, but there is currently no known hard incompatibilty
    that prevents you from integrating this software into an
    existing OS.

*   This software distribution, unpacked somewhere convenient
    within the Raspberry Pi filesystem.

    Unlike with the upstream 2015.12.15 release, this present release
    of the software should *not* be unpacked into `/opt/pidp8`. I
    recommend that you unpack it into `$HOME/src`, `/usr/local/src`
    or similar, but it really doesn't matter where you put it, as
    long as your user has full write access to that directory.

*   A working C compiler and other standard Linux build tools, such as
    `make(1)`.
    
    On Raspbian, you can install such tools with `sudo apt install
    build-essential`


## Configuring, Building and Installing

This software distribution builds and installs in the same way as most
other Linux/Unix software these days.  The short-and-sweet is:

    $ ./configure && make && sudo make install


### Configure Script Options

The `configure` script accepts most of the common flags for such
scripts:


#### --prefix

Perhaps the most widely useful `configure` script option is `--prefix`,
which lets you override the default installation directory,
`/opt/pidp8i`.  You could make it install the software under your home
directory on the Pi with this command:

    $ ./configure --prefix=$HOME/pidp8i && sudo make install

Although this is installing to a directory your user has write access
to, you still need to install via `sudo` because the installation
process does other things that do require `root` access.


#### --throttle

See `README-throttle.md` for the values this option takes.  If you don't
give this option, the simulator runs as fast as possible, more or less.


#### --help

Run `./configure --help` for more information on your options here.


### Installing

The installer normally will not overwrite the operating system and
program media (e.g. the OS/8 RK05 disk cartridge image) when installing
multiple times to the same location. If you do want the OS/program media
overwritten with fresh copies, say:

    $ sudo make mediainstall

This can be helpful if you have damaged your OS/program media or simply
want to return to the pristine versions as distributed.


## The Serial Mod

If you have done the [serial mod][smod] to your PiDP-8/I PCB and the
Raspberry Pi you have connected to it, add `--serial-mod` to the
`configure` command above.

If you do not give this flag at `configure` time with these hardware
modifications in place, the front panel will not work correctly,
and trying to run the software may even crash the Pi.

If you give this flag and your PCBs are *not* modified, most of the
hardware will work correctly, but several lights and switches will
not work correctly.


## Testing

You can test your PiDP-8/I LED and switch functions with the
`pidp8i-test` program. It will be in the `PATH` after installing the
software.

This program cannot run while the PiDP-8/I simulator is running in the
background. Therefore, before running it, say:

    $ sudo systemctl stop pidp8i

You may have to log out and back in for this to work, since the
installation script modifies your normal user's `PATH` since the normal
installation prefix is not in the stock Raspbian user `PATH`.

See [`README-test.md`][rmt] for more details.


## Using the Software

For the most part, this software distribution works like the upstream
[2015.12.15 distribution][usd]. Its [documentation][prj] therefore
describes this software too, for the most part.

The largest user-visible difference between the two software
distributions is that all of the shell commands affecting the software
were renamed to include `pidp8i` in their name:

1.  To start the simulator:

        $ sudo systemctl start pidp8i

    (This normally happens automatically on reboot after `sudo make install`
    above, but you may have the service stopped or disabled, such as
    in order to run one of the various [forks of Deeper Thought][dt2].)

2.  To enter the simulator:

        $ pidp8i

    This works because `$prefix/bin` is added to the installing user's
    `PATH` on `make install`.  This script does the same thing as
    `pdp.sh` in the upstream distribution.

3.  To return to the Raspbian command prompt without shutting the
    simulator down, type <kbd>Ctrl-A d</kbd>. Then you can re-enter
    with a `pidp8i` command.

4.  To shut the simulator down from within, type <kbd>Ctrl-E</kbd>
    to pause the simulator, then at the `simh>` prompt type
    `quit`. Type `help` at that prompt to get some idea of what
    else you can do with the simulator command language, or read the
    [SimH Users' Guide][sdoc].

5.  To shut the simulator down from the Raspbian command line:

        $ sudo systemctl stop pidp8i

There are [other major differences][mdif] between the upstream
distribution and this one. See that linked wiki article for details.


## License

Copyright © 2016-2017 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].


[smod]: http://obsolescence.wixsite.com/obsolescence/2016-pidp-8-building-instructions
[usd]:  http://obsolescence.wixsite.com/obsolescence/pidp-8-details
[dt2]:  https://github.com/VentureKing/Deeper-Thought-2
[sdoc]: http://simh.trailing-edge.com/pdf/simh_doc.pdf
[prj]:  http://obsolescence.wixsite.com/obsolescence/pidp-8
[rmt]:  https://tangentsoft.com/pidp8i/doc/trunk/README-test.md
[mdif]: https://tangentsoft.com/pidp8i/wiki?name=Major+Differences
[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
