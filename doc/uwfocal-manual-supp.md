# U/W FOCAL Manual Supplement for the PiDP-8/I

The [U/W FOCAL Manual][uwfm] is well written as far as it goes, but
there are gaps:

1.  It inspires questions in the reader's mind without providing an answer.

1.  It is written somewhat generically for the whole PDP-8 family as of
    late 1978, where the PiDP-8/I project is focused on a single model
    from 1968.

1.  It doesn't cover all of the topics we wish it would.

This document is [our](#license) attempt to fill these gaps.

[uwfm]: https://tangentsoft.com/pidp8i/doc/clean-os8-packs/doc/uwfocal-manual.md


## Starting and Stopping U/W FOCAL

The section "Starting the Program" in the [U/W FOCAL Manual][uwfm] is
entirely concerned with loading U/W FOCAL from paper tape using the
front panel and the BIN loader.

The PiDP-8/I software project does not currently ship U/W FOCAL in SIMH
paper tape image form. Instead, it's installed by default on the OS/8
system disk, which greatly simplifies starting it:

     .R UWF16K

Yes, that's all. You're welcome. `:)`

To get back to OS/8, just hit <kbd>Ctrl-C</kbd>.


## Loading and Saving Programs

When the [U/W FOCAL Manual][uwfm] talks about loading and saving
programs, it is almost entirely concerned with paper tapes, but in the
PiDP-8/I world — and more broadly, the simulated PDP-8 world — we have
many more choices for getting programs into and out of U/W FOCAL.

TODO


## Lowercase Input

The version of U/W FOCAL we include by default on the PiDP-8/I's OS/8
system disk copes gracefully with lowercase input. This is probably
because the version we ship was released late in the commercial life of
OS/8, by which time lowercase terminals were much more common than at
the beginning of OS/8's lifetime.

The examples in the [U/W FOCAL Manual][uwfm] are given in all-uppercase,
however, which means there is no reason you would immediately understand
how U/W FOCAL deals with lowercase input, having no examples to build a
mental model from.

The main rule is that U/W FOCAL is case-sensitive for variable and
built-in function names but case-insensitive for command names.
Therefore, this will not work:

    *type fsin(3.14)!
     0.000000000E+00*

It fails because there is no built-in function called `fsin`, but
there is one called `FSIN`, so you must say instead:

    *type FSIN(3.14)!
     1.592652638E-03

Similarly, `x` and `X` are different variable names.


## ASCII Character & Key Names

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


## Front Panel Differences

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


### `START` vs. `CLEAR` + `CONTINUE` vs. `RESET`

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


### `EXTD. ADDR LOAD`

The PDP-8/e has many fewer switches on its front panel than the PDP-8/I,
yet it is a more functional machine. One of the ways DEC achieved this
is by removing the `IF` and `DF` switch groups and adding the
`EXTD. ADDR LOAD` switch, which lets you set the `IF` and `DF` registers
using the same 12-bit switch register used by the `ADDR LOAD` switch.

The `ADDR LOAD` switch on a PDP-8/e does the same thing as the
`Load Add` switch on a PDP-8/I.


### Switch Direction

DEC reversed the meaning of switch direction between the PDP-8/I and the
PDP-8/e, and the U/W FOCAL Manual follows the 8/e convention: on the
8/I, up=0=off, whereas on the 8/e, up=1=on. Keep this in mind when
reading the U/W FOCAL Manual's references to front panel switch
settings.


## <a id="license"></a>License

Copyright © 2017 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].

[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
