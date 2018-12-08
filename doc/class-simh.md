# How to Control SIMH and OS/8 from Python

## Introduction

While we were building the `libexec/mkos8` tool, we built up a set of
functionality for driving SIMH and OS/8 running under SIMH from the
outside using [Python][py], a very powerful programming language well
suited to scripting tasks. It certainly beats writing PDP-8 code to
achieve the same ends!

When someone on the mailing list asked for a way to automatically drive
a demo script he'd found online, it was natural to generalize the core
functionality of `mkos8` as a reusable Python class, then write a script
to make use of it. The result is `class simh`, currently used by three
different scripts, including `mkos8` and the `teco-pi-demo` demo script.

This document describes how `teco-pi-demo` works, and through it, how
`class simh` works, with an eye toward teaching you how to reuse this
functionality for your own ends.

[py]: https://www.python.org/


## Invocation

Because we do not install these components in the system's Python
library path, you must modify that path to allow your script to find
these components. Simply copy this invocation block into the top of your
script:

    import os
    import sys
    sys.path.insert (0, os.path.dirname (__file__) + '/../lib')
    sys.path.insert (0, os.getcwd () + '/lib')

    from pidp8i import *
    from simh   import *

That adjusts the path, then imports all of the generic functionality
from the PiDP-8/I `lib` directory into the current namespace.

We do not pull the `mkos8` components into `teco-pi-demo` because they
are intended only to be used by `libexec/mkos8`. If you find something
in the `lib/mkos8` directory that you think is widely useful, make a
case for it on the mailing list, and we'll see about moving it to either
the `simh` or `pidp8i` namespace.

The `sys.path.insert` business assumes that your script is installed
into the PiDP-8/I's `bin` directory alongside `teco-pi-demo`. If you've
installed it somewhere else, you'll need to adjust these paths.


## Starting SIMH

The first thing we'll do is start SIMH as a child process of our
Python script under control of an instance of `class simh`:

    s = simh (dirs.build)

We call that instance `s` for short, because we will be calling its
methods a lot in this script. 

We pass `dirs.build` as the first parameter to the constructor, which
tells it how to find the PDP-8 simulator program, derived from the
code shipped on GitHub by the SIMH project, configured and modified
for the needs of the PiDP-8/I project.  We call this the child program,
as it is what `class simh` controls from the outside.

There is an optional second parameter to the constructor, a Boolean
flag that controls whether `class simh` starts the fully-featured
PiDP-8/I simulator or falls back to something closer to the pristine
upstream SIMH PDP-8 simulator.  By default, we do the former, so
that the simulator updates front panel LEDs with internal simulator
state, and toggling front panel switches affect the internal state
of the simulator.

If you don't want the PiDP-8/I GPIO thread to run while your script
runs, pass True here instead, since this is the "skip GPIO" flag,
and its default is therefore False.  We do that from programs like
`os8-run` and `os8-cp` because we want them to run everywhere, even on
an RPi while another simulator is running; we also don't want the front
panel switches to affect these programs' operations.  If your program
never runs on an RPi, passing True here might make it run a bit faster,
since it doesn't try to start the useless GPIO thread, feed it data,
or pull data is is expected to provide back into the simulator.


## Logging

The next step is to tell the `s` object where to send its logging
output:

    s.set_logfile (os.fdopen (sys.stdout.fileno (), 'w', 0))

Contrast the corresponding line in `mkos8` which chooses whether to send
logging output to the console or to a log file:

    s.set_logfile (open (dirs.log + 'mkos8-' + first_act + '.log', 'w') \
        if progmsg else os.fdopen (sys.stdout.fileno (), 'w', 0))


## Finding and Booting the OS/8 Media

If your program will use our OS/8 boot disk, you can find it
programmatically by using the `dirs.os8mo` constant, which means "OS/8
media output directory", where "output" refers to the worldview of
`mkos8`.  Contrast `dirs.os8mi`, which points to the directory holding
the input media for `mkos8`.

This snippet shows how to use it:

    rk = os.path.join (dirs.os8mo, 'os8v3d-bin.rk05')
    if not os.path.isfile (rk):
        print "Could not find " + rk + "; OS/8 media not yet built?"
        exit (1)

Now we attach the RK05 disk image to the PiDP-8/I simulator found by the
`simh` object and boot from it:

    print "Booting " + rk + "..."
    s.send_cmd ("att rk0 " + rk)
    s.send_cmd ("boot rk0")

This shows one of the most-used methods, `simh.send_cmd`, which sends a
line of text along with a carriage return to the spawned child program,
which again is `pidp8i-sim`.


## Driving SIMH and OS/8

After the simulator starts up, we want to wait for an OS/8 `.` prompt
and then send the first OS/8 command to start our demo. We use the
`simh.os8_send_cmd` method for that:

    s.os8_send_cmd ('\\.', "R TECO")

This method differs from `send_cmd` in a couple of key ways.

First, it waits for a configurable prompt character — sent as the first
parameter — before sending the command.  This is critical when driving
OS/8 because OS/8 lacks a keyboard input buffer, so if you send text to
it too early, all or part of your input is likely to be lost, so your
command won't work.

Second, because OS/8 can only accept so many characters of input per
second, `os8_send_cmd` inserts a small delay between each input
character to prevent character losses.

(See the commentary for `simh._kbd_delay` if you want to know how that
delay value was calculated.)

The bulk of `teco-pi-demo` consists of more calls to `simh.os8_send_cmd`
and `simh.send_cmd`. Read the script if you want more examples.

**IMPORTANT:** The `\\.` syntax for specifying the OS/8 `.` command
prompt is tricky. If you pass just `'.'` here instead, Python's
[regular expression][re] matching engine will interpret it to mean
that it should match *any* character as the prompt, almost certainly
breaking your script's state machine, since it is likely to cause the
call to return too early. If you instead pass `'\.'`, Python's string
parser will take the backslash as escaping the period and again pass
just a single period character to the regex engine, giving the same
result. You must specify it exactly as shown above to escape the
backslash so that Python will send an escaped period to the regex
engine, which in turn is necessary to cause the regex engine to treat
it as a literal period rather than the "any character" wildcard.

Much the same is true when your script needs to await the common
<code>*</code> prompt character: you must pass it like so:

    s.os8_send_cmd ('\\*', 'COMMAND')

[re]: https://en.wikipedia.org/wiki/Regular_expression


## Escaping OS/8 to SIMH

Sometimes you need to escape from OS/8 back to SIMH with a
<kbd>Ctrl-E</kbd> keystroke so that you can send more SIMH commands
after OS/8 starts up. This accomplishes that:

    s.os8_send_ctrl ('e')

While out in the SIMH context, you *could* continue to call the
`simh.os8_*` methods, but since SIMH can accept input as fast as your
program can give it, it is best to use methods like `simh.send_cmd`
which don't insert artificial delays.  For many programs, this
difference won't matter, but it results in a major speed improvement in
a program like `mkos8` which sends many SIMH and OS/8 commands
back-to-back!


## Getting Back to OS/8 from SIMH

There are several ways to get back to the simulated OS/8 environment
from SIMH context, each with different tradeoffs.


### Rebooting

You saw the first one above: send a `boot rk0` command to SIMH. This
restarts OS/8 entirely. This is good if you need a clean environment.
If you need to save state between one run of OS/8 and the next, save it
to the RK05 disk pack or other SIMH media, then re-load it when OS/8
reboots.


### Continuing

The way `teco-pi-demo` does it is to send a `cont` command to SIMH.

The problem with this method is that it sometimes hangs the simulator.
The solution is to insert a small delay *before* escaping to the SIMH
context. I'm not sure why this is sometimes necessary. My best guess is
required to give OS/8 time to settle into an interruptible state before
escaping to SIMH, so that on "continue," we re-enter OS/8 in a sane
state.

You can usually avoid the need for that delay by waiting for an OS/8
command prompt before escaping to SIMH, since that is a reliable
indicator that OS/8 is in such an interruptible state.

You don't see these anomalies when using OS/8 interactively because
humans aren't fast enough to type commands at OS/8 fast enough to cause
the problem.  That is doubtless why there this bug still exists in OS/8
in 2017.


### Re-Entering

If your use of OS/8 is such that all required state is saved to disk
before re-entering OS/8, you can call the `simh.os8_restart` method to
avoid the need for a delay *or* a reboot.  It re-calls OS/8's entry
point from SIMH context, which we've found through much testing is
entirely reliable, as compared to sending a SIMH `cont` command without
having delayed before escaping to SIMH context.

`mkos8` uses this option extensively.


## Sending Escape Characters

Several OS/8 programs expect an <kbd>Escape</kbd> (a.k.a. `ALTMODE`)
keystroke to do things. Examples are `TECO` and `FRTS`. There isn't a
specific method to do this because we can do that in terms of one we've
just described:

    s.os8_send_ctrl ('[')

Yes, <kbd>Escape</kbd> is <kbd>Ctrl-\[</kbd>. Now you can be the life of
the party with that bit of trivia up your sleeve. Or maybe you go to
better parties than I do.


## But There's More!

The above introduced you to most of the functionality of `class simh`
used by `teco-pi-demo`, but there's more to the class than that,
primarily because the `mkos8` script's needs are broader.  Rather than
just recapitulate the class documentation here, please read through [the
class's source code][ssc], paying particular attention to the method
comments. It's a pretty simple class, making it a quick read.

Another useful module is [`pidp8i.dirs`][dsc] which contains paths to
many directories in the PiDP-8/I system, which you can reuse to avoid
having to hard-code their locations.  This not only makes your script
independent of the installation location, which is configurable at build
time via `./configure --prefix=/some/path`, but also allows it to run
correctly from the PiDP-8/I software's build directory, which has a
somewhat different directory structure from the installation tree.

[ssc]: https://tangentsoft.com/pidp8i/file/lib/simh.py
[dsc]: https://tangentsoft.com/pidp8i/file/lib/pidp8i/dirs.py


## <a id="license" name="credits"></a>Credits and License

Written by and copyright © 2017 by Warren Young. Licensed under the
terms of [the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
