# U/W FOCAL Manual Supplement for the PiDP-8/I

The [U/W FOCAL Manual][uwfm] is well written as far as it goes, but
there are gaps:

1.  It inspires questions in the reader's mind without providing an answer.

1.  It is written somewhat generically for the whole PDP-8 family as of
    late 1978, where the PiDP-8/I project is focused on a single model
    from 1968.

1.  It doesn't cover all of the topics we wish it would.

This document is [our](#license) attempt to fill these gaps.
[Extensions and corrections][hack] are welcome.

[hack]: https://tangentsoft.com/pidp8i/doc/trunk/HACKERS.md
[uwfm]: https://tangentsoft.com/pidp8i/doc/clean-os8-packs/doc/uwfocal-manual.md


## <a id="starting" name="stopping"></a>Starting and Stopping U/W FOCAL

The section "Starting the Program" in the [U/W FOCAL Manual][uwfm] is
entirely concerned with loading U/W FOCAL from paper tape using the
front panel and the BIN loader.

The PiDP-8/I software project does not currently ship U/W FOCAL in SIMH
paper tape image form. Instead, it's installed by default on the OS/8
system disk, which greatly simplifies starting it:

     .R UWF16K

Yes, that's all. You're welcome. `:)`

To get back to OS/8, just hit <kbd>Ctrl-C</kbd>.


## <a id="pasting"></a>Pasting Text in from a Terminal Emulator

If you are SSHing into your PiDP-8/I, you might think to write your
FOCAL programs in your favorite text editor out in the host OS then copy
and paste that text into U/W FOCAL via the terminal emulator connected
to the PiDP-8/I running it. Currently, that won't work. (2017.10.05) We
believe it is because of the way U/W FOCAL handles terminal I/O and
interrupts.

The result is that you get trash input when you try this. When the
problem is fixed, this section will go away.

Meanwhile, the next section tells how to get around this problem.


## <a id="loading" name="saving"></a>Loading and Saving Programs

When the [U/W FOCAL Manual][uwfm] talks about loading and saving
programs, it is almost entirely concerned with paper tapes, but in the
PiDP-8/I world — and more broadly, the simulated PDP-8 world — we have
many more choices for getting programs into and out of U/W FOCAL.

TODO


## <a id="lowercase"></a>Lowercase Input

The version of U/W FOCAL we include by default on the PiDP-8/I's OS/8
system disk copes with lowercase input only within a fairly narrow
scope. The fact that it copes with lowercase input at all is likely due
to the fact that the version we ship was released late in the commercial
life of OS/8, by which time lowercase terminals were much more common
than at the beginning of OS/8's lifetime.

The examples in the [U/W FOCAL Manual][uwfm] are given in all-uppercase,
which means there is no reason you would immediately understand how U/W
FOCAL deals with lowercase input, having no examples to build a mental
model from. If you just guess, chances are that you will be wrong sooner
or later, because U/W FOCAL's behavior in this area can be surprising!

The two main rules to keep in mind are:

1.  U/W FOCAL is case-sensitive for variable and built-in function
    names, but it is case-insensitive for command names.

2.  U/W FOCAL doesn't support lowercase variable and function names. It
    may sometimes appear to work, but internally, U/W FOCAL isn't doing
    what you want it to.

This doesn't work:

    *type fsin(pi/2)!
     0.000000000E+00*

It fails because there is no built-in function called `fsin` nor a
built-in constant `pi`, but there *is* a `FSIN` function and a `PI`
constant, so you must say this instead:

    *type FSIN(PI/2)!
     1.000000000E+00

U/W FOCAL doesn't care that you gave the `type` command in lowercase,
but it *does* care about the case of the built-in function name.

Violating rule 2 can be even more surpring:

    .R UWF16K               # We need a fresh environment for this demo.
    *s a=1                  # What, no error?  I thought you said...
    *s b=2
    *s c=3
    *type $ !
    *

No, that transcript isn't cut off at the end: the `TYPE` command simply
doesn't give any output! Why?

The reason is that U/W FOCAL can't \[currently\] cope with lowercase
variable names.

But wait, it gets weird:

    *s A=1
    *s foo=42
    *type $ !
    A (+00) 1.000000000E+00
    &/(+00) 4.200000001E+01

We now see output for our uppercased `A` variable, but what is that `&/`
noise? For some reason, U/W FOCAL has chosen to use one of the so-called
"secret variables" to hold the value we tried to asssign to `foo`. Why
now and not above with `a`, `b`, and `c`?

We have not yet tried to investigate the reason for this because the
workaround is simple: keep <kbd>CAPS LOCK</kbd> engaged while typing
FOCAL programs except when typing text you want FOCAL to send back out
to the terminal:

    *1.1 TYPE "Hello, world!"!
    *G
    Hello, world!


## <a id="ascii"></a>ASCII Character & Key Names

Many of the common names for keys and their ASCII character equivalents
have shifted over the years, and indeed they shifted considerably even
during the time when the PDP-8 was a commercially viable machine. The
following table maps names used in the [U/W FOCAL Manual][uwfm] to their
decimal ASCII codes and their common meaning today.

| Old Name    | ASCII | Current Name  |
| ----------- | ----- | ------------- |
| `RUBOUT`    | 127   | Delete or Del |
| `DELETE`    | 127   | Delete or Del |
| `BACKARROW` | 95    | Underscore    |
| `UNDERLINE` | 95    | Underscore    |

Beware that the ASCII values above differ from the values given in the
U/W FOCAL Manual Appendix "Decimal Values for All Character Codes."
FOCAL sets the 8th bit on ASCII characters for reasons unimportant here.
Just add 128 to the values above if you need to get the FOCAL
equivalent.

Some terminals and terminal emulator software may remap Backspace and
Delete, either making one equivalent to the other or swapping them.
Without such remapping, if you hit the key most commonly marked
Backspace on modern keyboards, U/W FOCAL will just insert an ASCII
character 8 at that point in the program entry, almost certainly not
what you want. You either need to remap Backspace to Delete or hit the
key most commonly marked Del.

The U/W FOCAL Manual also references keys that used to appear on some
terminals, most especially teletypes, which no longer appear on modern
keyboards:

| Teletype Key | Modern Equivalent |
| ------------ | ----------------- |
| `LINE FEED`  | <kbd>Ctrl-J</kbd> |
| `FORM FEED`  | <kbd>Ctrl-L</kbd> |


## <a id="front-panel"></a>Front Panel Differences

Whenever the [U/W FOCAL Manual][uwfm] refers to the PDP-8's front panel,
it is speaking generically of all the models it ran on as of October
1978. The PDP-8 models introduced in the decade following the
introduction of the PDP-8/I differ in many ways, and one of the greatest
areas of difference is in their front panel controls and indicators. We
do not intend to fully document all of the differences here, but only to
clarify the differences brought up by the U/W FOCAL Manual.

You normally will not need to use the front panel with the OS/8 version
of U/W FOCAL we distribute with the PiDP-8/I software distribution since
you start and stop U/W FOCAL through OS/8 rather than the front panel.
However, we thought these matters could use clarification anyway.

Beyond this point, when we refer to the PDP-8/e, we also mean the 8/f,
which shared the same front panel design. We also include the 8/m, which
normally came with a minimal front panel, but there was an optional
upgrade for an 8/e/f style front panel. These three models are therefore
interchangeable for our purposes here.


### <a id="clear-regs"></a>`START` vs. `CLEAR` + `CONTINUE` vs. `RESET`

With the PDP-8/e, DEC replaced the `START` front panel switch of the
preceding PDP-8/I with a `CLEAR` switch. Why did they do this?

On a PDP-8/I, the difference between `START` and `CONTINUE` is sometimes
confusing to end users, since in many cases they appear to do the same
thing. Why have both? The difference is that `CONTINUE` simply resumes
operation from the current point in the program where it is stopped,
whereas `START` resets several key registers and *then* continues.

The PDP-8/e change splits this operation up to avoid the confusion: the
old `START` keypress is equivalent to `CLEAR` followed by `CONTINUE`.
(This pair of switches also has a `START` label above them, a clear
functional grouping.)

The U/W FOCAL Manual also speaks of a `RESET` switch in conjunction with
the FOCAL starting and restarting the computer. I haven't been able to
track down which PDP-8 model has such a switch yet, but for our purposes
here, I can say that it just means to load the starting address and hit
`START` on a PDP-8/I.


### <a id="if-df"></a>`EXTD. ADDR LOAD`

The PDP-8/e has many fewer switches on its front panel than the PDP-8/I,
yet it is a more functional machine. One of the ways DEC achieved this
is by removing the `IF` and `DF` switch groups and adding the
`EXTD. ADDR LOAD` switch, which lets you set the `IF` and `DF` registers
using the same 12-bit switch register used by the `ADDR LOAD` switch.

The `ADDR LOAD` switch on a PDP-8/e does the same thing as the
`Load Add` switch on a PDP-8/I.


### <a id="sw-dir"></a>Switch Direction

DEC reversed the meaning of switch direction between the PDP-8/I and the
PDP-8/e, and the [U/W FOCAL Manual][uwfm] follows the 8/e convention: on
the 8/I, up=0=off, whereas on the 8/e, up=1=on. Keep this in mind when
reading the U/W FOCAL Manual's references to front panel switch
settings.


### <a id="sw-order"></a>Switch Ordering

When the [U/W FOCAL Manual][uwfm] talks about the switch register (SR),
it numbers the switches left to right, not by their logical bit number
in the switch register. That is, "Switch 0" is the leftmost (high order
bit) SR switch, not "bit 0" in the SR, which would be the rightmost SR
switch.


## <a id="license"></a>License

Copyright © 2017 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].

[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
