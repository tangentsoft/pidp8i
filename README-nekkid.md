# How to Run a Nekkid PiDP-8/I


## The Situation

> "The S in IoT stands for Security." — [TVS blog][tvs]

The PiDP-8/I binary OS images shipped from [tangentsoft.com][site] are
fairly well locked down.  They ship with the normal Raspbian default
user name changed to an uncommon name, that user has a very strong
default password which you are forced to change on first login to
something different, the OS won't accept a weak password by default, and
there are no networked services running except for SSH, which itself is
quite secure by default on Raspbian.

Yet, some people want their PiDP-8/I to just be an appliance: they want
it to turn on and run the PDP-8 simulator, and to drop them into a Linux
command prompt by default, no user names, no passwords.  I resist
shipping OS images configured this way because Raspbian is a
full-featured operating system.  When networked, it is capable of being
a powerful island for an attacker on the network.  Heavily protected web
services have been taken down by small armies of such systems pounding
on them in concert.

I refuse to build OS images that contribute to this problem.

I will, however, guide you toward methods for stripping away the
security that's been provided, if that's what you want.


## Caveat Lector

I do not recommend doing any of this on any networked Raspberry Pi
device.  A "naked Pi" without networking isn't much of a threat to
anyone.


## Getting Rid of the Login Prompt

The first thing you have to do on logging in is change your user name
back from `pidp8i` to `pi`, because the final step below is hard-coded
to use `pi` as the user name.

Give these commands, being *very careful* how you type them, because you
can lock yourself out of the system if you fat-finger them:

    $ exec sudo -i
    # usermod -l pi -d /home/pi -m pidp8i
    # exit

Now log back in as user `pi`, with the same password you had for
user `pidp8i` prior to this change.

If you have the PiDP-8/I PCB attached to your Pi, you will notice when
it reboots that the front panel did not light up.  This is becasue the
PiDP-8/I software was built with the knowledge that the default user
name was `pidp8i`.  You need to rebuild and reinstall it to fix this:

    $ cd ~/pidp8i
    $ ./configure && make && sudo make install
    $ sudo systemctl restart pidp8i

That should light the panel back up.

Now we can finally set up auto-login:

    $ sudo raspi-config
    
Then go to **Boot Options** → **Desktop / CLI** → **Console Autologin**.

It will offer to reboot your system to test this.  It should succeed.


## Attaching the Console to the Simulator by Default

Instead of booting to a Linux command prompt, you may prefer to have the
Raspbian console attached to the PiDP-8/I simulator console, so that you
see the program you're simulating: OS/8, TSS/8, Spacewar! or what have
you.

The simplest way I can think of to do that is:

    $ echo pidp8i >> ~/.profile
    $ sudo reboot

That should drop you into the SIMH console after the host OS boots.

If you need to get back to Linux, there are a couple of ways.

If you just need to run a single command, you can press <kbd>Ctrl-E</kbd>
and then run your command from the `sim>` prompt with the `!` shell
escape:

    sim> !df -h
    ...disk free space report...
    sim> c
    ...simulation continues...

If you need to do more than that, I recommend quitting out of SIMH via
<kbd>Ctrl-E</kbd> `quit`.  Do your Linux work, and when done, re-start
the simulator and re-attach to it:

    $ sudo systemctl restart pidp8i
    $ pidp8i


## Rationale

If you are wondering why I'd take the time to write up this guide
instead of putting that time toward building OS images configured this
way, there are a bunch of reasons:

1.  I do not subscribe to the design principles of an OS configured this
    way, yet I also do not believe in restricting access to technical
    information.  This document is as far as I'm willing to go: you must
    take the next steps, if you insist on running a naked PiDP-8/I.

2.  Defaults matter.  If I provide secure OS images and it takes extra
    work to throw open the barriers, I believe the world is a better
    place than if I provide insecure OS images which people who care
    about security much tighten down.

3.  This guide took me about an hour to write, about the amount of time
    it would take to build a third OS image.  Then I'd need to do it
    again for the no-lamp-simulator case.  And then next month I'd need
    to spend two *more* hours to update these two additional images.
    And again and again.  Two OS image updates every month or so is
    plenty of work already.


## License

Copyright © 2017 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].


[tvs]:  http://www.testandverification.com/iot/s-iot-stands-security/
[site]: https://tangentsoft.com/pidp8i/
[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
