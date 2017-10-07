# U/W FOCAL Manual Supplement for the PiDP-8/I

The [U/W FOCAL Manual][uwfm] is well written as far as it goes, but
there are gaps:

1.  It inspires questions in the reader's mind without providing an
    answer.  While that is actually a hallmark of a good book, the U/W
    FOCAL manual sometimes does it for topics that are properly within
    its scope and so should be answered within.

1.  It omits coverage for some topics we wish it would cover, though
    they are not properly within its scope.

1.  It is written somewhat generically for the whole PDP-8 family as of
    late 1978, whereas the PiDP-8/I project is focused on a single model
    from 1968.

This document is [our](#license) attempt to fill these gaps.
[Extensions and corrections][hack] are welcome.

You might also find the [DECUS submission for U/W FOCAL][duwf] and the
[U/W FOCAL reference cards][uwfr] helpful.

[duwf]: http://www.pdp8.net/pdp8cgi/query_docs/view.pl?id=191
[hack]: https://tangentsoft.com/pidp8i/doc/trunk/HACKERS.md
[uwfm]: https://tangentsoft.com/pidp8i/doc/clean-os8-packs/doc/uwfocal-manual.md
[uwfr]: https://tangentsoft.com/pidp8i/doc/clean-os8-packs/doc/uwfocal-refcards.md


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


## <a id="loading" name="saving"></a>Loading and Saving Programs

There are many ways to get program text into U/W FOCAL other than simply
typing it in. This section gives several methods, because each may be of
use to you in different circumstances. Some of them may not be of direct
use to you, but may open your eyes to techniques that may be useful to
you in other contexts, so we encourage you to read this entire section.


### <a id="ls-pasting"></a>Pasting Text in from a Terminal Emulator: The Na&iuml;ve Way

If you are SSHing into your PiDP-8/I, you might think to write your
FOCAL programs in your favorite text editor on your client PC then copy
and paste that text into U/W FOCAL over SSH. Currently, that won't work.
(2017.10.05) We believe it is because of the way U/W FOCAL handles
terminal I/O and interrupts. If you try, the input ends up trashed in
FOCAL.


### <a id="ls-pip"></a>Pasting Text in from a Terminal Emulator: The Way That Works

"But I really really want to write my FOCAL programs in [my favorite
text editor][mfte] and paste them into my PiDP-8/I," I hear you say.
Dispair not. There is a path.  Follow.

The problem affecting U/W FOCAL which prevents it from handling input at
modern paste-through-SSH speeds doesn't affect OS/8 itself, so we'll use
it as an intermediary:

    .R PIP
    *HELLO.DA<TTY:                  ⇠ use default extension for O I
    01.10 TYPE "Hello, world!"!
    ^Z                              ⇠ Ctrl-Z is the EOF marker in OS/8
    *^C                             ⇠ return to OS/8 from PIP
    .R UWF16K                       ⇠ run U/W FOCAL
    *O I HELLO                      ⇠ open text file for input; "types" pgm in for us
    _G                              ⇠ EOF seen, program started
    Hello, world!                   ⇠ and it runs!

That is, we use OS/8's `PIP` command to accept text input from the
terminal (a.k.a. TTY = teletype) and write it to a text file. Then we
load that text in as program input using commands we'll explain in
detail [below](#ls-write).

[mfte]: https://duckduckgo.com/?q=%22my+favorite+text+editor%22


### <a id="ls-punch"></a>The `PUNCH` Command

When the [U/W FOCAL Manual][uwfm] talks about loading and saving
programs, it is in terms of the `PUNCH` command, because the manual is
focused on the paper tape based version of U/W FOCAL.

The PiDP-8/I software project ships the OS/8 version of U/W FOCAL
instead, which doesn't even have a `PUNCH` command. (It appears to have
been replaced by the `PLOT` command, mentioned on [`CARD1.DA`][uwfr],
but that's of no use to us here, since SIMH doesn't support pen
plotters. (Yet.))

Even if it did work, mounting and unmounting simulated paper tapes under
SIMH is a bit of a hassle. We can do better.


### <a id="ls-library"></a>The `LIBRARY` Command

The effective replacement for `PUNCH` in the OS/8 version of U/W FOCAL
is the `LIBRARY` command.

If you've read [the manual][uwfm], you may be wondering if it's
overloaded with `LINK` and `LOOK`, but no: those commands are apparently
missing from the OS/8 version. (Open question: how do you use multiple
fields of core for program code with the OS/8 version, then?)

Briefly, then, I'll show how to use some of these commands:

    .R UWF16K                           ⇠ start fresh
    *1.10 TYPE "Hello, world!"!         ⇠ input a simple one-line program
    *L S HELLO                          ⇠ write program to disk with LIBRARY SAVE
    *L O HELLO                          ⇠ verify that it's really there
    HELLO .FD   1                       ⇠ yup, there it is!
    *E                                  ⇠ ERASE all our hard work so far
    *W                                  ⇠ is it gone?
    C U/W-FOCAL:  16K-V4  NO/DA/TE      ⇠ goneski
    *L C HELLO                          ⇠ load it back in with LIBRARY CALL
    *W                                  ⇠ did it come back?
    C U/W-FOCAL:  HELLO   NO/DA/TE

    01.10 TYPE "Hello, world!"!         ⇠ yay, there it is!
    *L D HELLO                          ⇠ nuke it on disk; it's the only way to
    *L O HELLO                          ⇠ ...be sure
    *                                   ⇠ Houston, we have no program

See the [DECUS submission][duwf] and `CARD2.DA` in the [refcards][uwfr]
for more examples.


### <a id="ls-write"></a>The `WRITE` Command

One problem with using U/W FOCAL's `LIBRARY` command for this is that
it saves programs as core images, which are a) non-relocatable; and b)
non-portable to other versions of FOCAL. We can fix both of these
programs by saving the program to an ASCII text file instead.

With a program already typed in or loaded from disk:

    *O O HELLO; W; O C

All of that has to be on a single line, with the semicolons. (See
[below](#ls-hard-way) for what you must go through if you do not!)

What this does is opens a data output file (extension `.DA`) and makes
it the output destination, so that the following `WRITE` command sends
its text there, and then it is immediately closed with `O C`, returning
control back to the terminal.

You can then load that program back into U/W FOCAL with the same command
we used above with the `PIP` solution:

    *O I HELLO

If you `TYPE` that file from OS/8, you might be wondering why the banner
line doesn't cause a problem on loading the file back in:

    C U/W-FOCAL:  HELLO   NO/DA/TE

That leading `C` causes U/W FOCAL to treat it as a comment. Since we're
in "direct mode" at that point, the comment is simply eaten.


### <a id="ls-hard-way"></a>The Hard Way

You might be wondering why we needed to put the `O O` command on a
single line with 3 other commands [above](#ls-write). It is because if
we don't, the `WRITE` and `OUTPUT CLOSE` commands get added as the first
and last lines of our output file, and then we must edit them out before
we can read that file back into U/W FOCAL. As an exercise in practical
use of U/W FOCAL and OS/8's `EDIT` program, we will now show what it
takes to recover from that.

Page 8 of the [DECUS documentation for OMSI FOCAL][domsi] provides a
good description of this issue and how to work around it to place a text
version of a program on the disk:

> ... FOCAL assumes `.FC` and `.FD` as name extensions for program and
> data files respectively. Data files are saved in standard [...] ASCII
> format and are compatible with EDIT and TECO-8.  Program files are
> saved in core image format and may be transferred by PIP only with
> the `/I` option. To produce an ASCII file containing a FOCAL program,
> `OPEN` an `OUTFILE`; `WRITE ALL` then `OUTPUT CLOSE`.

The resulting file contains the `WRITE ALL` command at the beginning and
the `OUTPUT CLOSE` command at the end.  Removing those lines enables the
file to be read back in as a program using the `OPEN INPUT` command.

Let's see how to do this, step by step. First, let's enter a simple
program:

    .R UWF16K
    *1.10 T "HELLO",!
    *G
    HELLO

Now let's save it to an ASCII text file on the OS/8 disk using separate
U/W FOCAL commands, rather than the one-liner we did above:

    *OPEN OUTPUT TEST,ECHO                      ⇠ uses *.FD by default
    *W
    C U/W-FOCAL:  TEST    NO/DA/TE
    
    01.10 T "HELLO",!
    *OUTPUT CLOSE

Next, we'll break out of the U/W FOCAL environment to get back to OS/8
and show that the file is there, but with lines we don't want:

    *^C                                        ⇠ that is, hit Ctrl-C
    .TYPE TEST.FD
    *W
    C U/W-FOCAL:  16K-V4  NO/DA/TE
    
    01.10 T "HELLO",!
    *OUTPUT CLOSE

So, let's fix it. We'll use OS/8's `EDIT` program for this, but you
could just as well use `TECO` or another text editor you like better:
    
    .R EDIT
    *TEST.FD<TEST.FD
    #R
    ?
    #1L
    *W
    
    #1D
    
    #/L
    *OUTPUT CLOSE
    
    #/D
    
    #L
    C U/W-FOCAL:  16K-V4  NO/DA/TE
    
    01.10 T "HELLO",!
    
    #E

The [previous method](#ls-write) avoids all of that `EDIT` ugliness.

Now let's load it back up into U/W FOCAL and try to run it:

    .R UWF16K
    *OPEN INPUT TEST,ECHO
    *C U/W-FOCAL:  16K-V4  NO/DA/TE
    *
    *01.10 T "HELLO",!
    *_                                         ⇠ hit Enter
    *GO
    HELLO

Success!

The `*_` pair above is the asterisk prompt printed by the FOCAL command
interpreter signifying that it is ready for input followed by the
underscore printed by the file handler signifying that it hit the end of
file for `TEST.FD`. The hint above to hit <kbd>Enter</kbd> is optional
and merely causes FOCAL to print another `*` prompt, which clarifies the
transcript.

We added the `,ECHO` bits in the commands above only to make U/W FOCAL
echo what it's doing to the terminal to make the transcripts clearer.
In your own work, you might want to leave this off.

By skipping both of these optional bits and abbreviating the commands,
the final terminal transcript above condenses considerably:

    .R UWF16K
    *O I TEST            ⇠  assumes *.FD, just like O O
    *_G
    HELLO

[domsi]: http://www.pdp8.net/pdp8cgi/query_docs/view.pl?id=366


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

The following gives incorrect output because of a violation of rule 1:

    *type fsin(pi/2)!
     0.000000000E+00*

The correct answer is 1. It fails because there is no built-in function
called `fsin` nor a built-in constant `pi`.

FOCAL gives an answer here instead of detecting our failure to call
things by their right names because it is falling back to its rule to
use a value of 0 where no value or function is available to do what you
asked. Zero divided by 2 is 0; then it tries to subscript a nonexistent
`fsin` variable with index 0, so it punts and gives the answer you see
above, zero.

A better language would have detected your errors and given a
diagnostic, but U/W FOCAL is implemented in less than a page of PDP-8
core memory, roughly the same number of bytes as
[Clang](http://clang.llvm.org/) gives when compiling an empty C program
on the machine I'm typing this on. The fact that U/W FOCAL detects
errors *at all* is somewhat impressive.

To get the expected result, call the `FSIN` function and use the `PI`
constant, which are very much not the same thing as `fsin` and `pi` to
FOCAL:

    *type FSIN(PI/2)!
     1.000000000E+00

U/W FOCAL doesn't care that you gave the `type` command in lowercase,
but it *does* care about the case of the function and variable names.

U/W FOCAL's tolerance of lowercase in command names doesn't extend to
arguments. In particular, the `OPEN` command's argument must be
uppercase: `o o` doesn't work, nor does `O o`, but `o O` does.

Violating rule 2 can be even more surprising:

    .R UWF16K               ⇠ We need a fresh environment for this demo.
    *s a=1                  ⇠ What, no error?  I thought you said...
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
noise? Apparently "`foo`" somehow gets mangled into `&/` by FOCAL's
command parser.

We have not yet tried to investigate the reason `foo` gets saved into a
mangled variable name and `a`, `b`, and `c` above do not, because the
workaround is simple: keep <kbd>CAPS LOCK</kbd> engaged while typing
FOCAL programs except when typing text you want FOCAL to send back out
to the terminal:

    *1.1 TYPE "Hello, world!"!
    *G
    Hello, world!

See the [Variables section][vars] of [`CARD2.DA`][card2] for more
information on variable naming.

[card2]: uwfocal-refcards.md#card2
[vars]:  uwfocal-refcards.md#variables


## <a id="output-format"></a>Default Output Format

FOCAL is primarily a scientific programming language. That, coupled with
the small memory size of the PDP-8 family and the slow terminals of the
day mean its default output format might not be what you initially
expect. Consider these two examples pulled from the [U/W FOCAL
Manual][uwfm]:

    *TYPE FSGN(PI), FSGN(PI-PI), FSGN(-PI) !
     1.000000000E+00 0.000000000E+00-1.000000000E+00
    *TYPE 180*FATN(-1)/PI !
    -4.500000000E+01

This may raise several questions in your mind, such as:

1.  Why is there no space between the second and third outputs of the
    first command?

2.  Why does the ouptut of the first command begin in the second column
    and the second begin at the left margin?

3.  Is the second command giving an answer of -4.5°?

If you've read the U/W FOCAL Manual carefully, you know the answer to
all three of these questions, but those used to modern programming
environments might have skimmed those sections and thus be surprised by
the above outputs.

The first two questions have the same answer: U/W FOCAL reserves space
for the sign in its numeric outputs even if it doesn't end up being
needed. This was done, no doubt, so that columns of positive and
negative numbers line up nicely. It might help to see what's going on if
you mentally replace the spaces in that first output line above with `+`
signs.

This then explains the apparent discrepancy between the first and second
commands' outputs: the first output of the first command is positive,
while the second command's output is negative, so there is a space at
the beginning of the first output for the implicit `+` sign.

As for the third question, the default output format is in scientific
notation with full precision displayed: 4.5&times;10¹ = 45 degrees, the
correct answer.


### Improvements

The following changes to the examples as given in the manual show how
you can get output more suitable to your purposes:

    *TYPE %1, FSGN(PI):5, FSGN(PI-PI):5, FSGN(-PI)!
     1    0    -1

That sets the output precision to 1 significant digit, which is all we
need for the expected {-1, 0, -1} ouptut set. The tabstop formatting
options (`:5`) put space between the answers, but there is a trick which
gives similar output with a shorter command:

    *TYPE %5.0, FSGN(PI), FSGN(PI-PI), FSGN(-PI)!
         1     0    -1

That tells it to use 5 significant digits with zero decimal digits.
Since the answers have only one significant digit, FOCAL right-justifies
each output with 4 spaces. There are 5 spaces between the `1` and `0`
outputs because of that pesky implicit `+` sign, though.

The second example above can be improved thus:

    *TYPE %3.2, 180*FATN(-1)/PI !
    -45.0

That tells FOCAL to display 3 significant digits, and to include up to 2
decimal places even if the traling one(s) would be 0, thus showing all 3
significant digits in an answer expected in degrees. If you'd wanted 4
significant digits with any trailing zeroes instead, you'd give `%4.3`
instead. If you'd given `%3`, the output would be `-45`, the trailing
zero being deemed unnecessary.


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


## Error Codes

The [U/W FOCAL Manual][uwfm] gives a somewhat different error code table
than the one on `CARD4.DA` of the [U/W FOCAL reference cards][uwfr]. For
the most part, the latter is just a simple superset of the former, and
both apply. In some cases, though, the two tables differ, or one of them
differs from the `UWF16K` program we ship on the OS/8 system disk.


### `?` vs `?01.00` — Keyboard interrupt or restart from location 10200

Our current version of FOCAL gives `?` for this condition, not `?01.00`.


### `?18.32` vs `?18.42` — `FCOM` index out of range

The two error code tables give different error codes for this condition.
However, since I have not been able to get this error to happen, I do
not know which code is correct for our current version of FOCAL.


### `?31.<7` — Non-existent program area called by `LOOK` or `LINK`

Our current implementation of U/W FOCAL removed those commands in favor
of `LIBRARY`, so you can't make this one happen. An error in a `LIBRARY`
command is most likely to give `?26.07` instead.


### Irreproducible Errors

There are some errors listed in one or both tables that I have been
unable to cause, though I have tried:

| Code   | Meaning 
| ------ | -------
| ?07.44 | Operator missing or illegal use of an equal sign
| ?18.32 | `FCOM` index out of range (value given in the manual)
| ?18.42 | `FCOM` index out of range (value given on the refcard)
| ?27.90 | Zero divisor


### Untested Error Cases

I have not yet created programs large enough to test the "out of space"
codes `?06.41` (too many variables), `?10.50` (program too large),
`?13.65` (insufficient memory for `BATCH` operation), `?23.18` (too much
space requested in `OUTPUT ABORT` or `CLOSE`), `?23.37` (output file
overflow), and `?25.02` (stack overflow).

There are also some errors I simply have not yet tried to cause:
`?01.03`, `?01.11`, `?12.10`, `?12.40`.


## <a id="miss-hw"></a>Missing Hardware Support

The [U/W FOCAL reference cards][uwfr] and the [DECUS submission][duwf]
talk about features for hardware we don't have. Either the
command/feature doesn't exist at all in the version of U/W FOCAL we
distribute or it doesn't do anything useful, lacking support within the
version of SIMH we distribute.

Broadly, these features are for the PDP-12, the LAB-8/e, Tektronix
terminals, and pen plotters. Should anyone extend SIMH with a way to
control such hardware (or emulations of it) we may consider putting
these features back into our distribution of U/W FOCAL.

In the meantime, the following facilities do not work:

*   The `FADC`, `FJOY`, `FLS`, `FRS`, and `FXL` functions don't exist

*   The `PLOT` and `VIEW` commands don't exist

*   Error code `?14.15` can't happen; we have no "display buffer"

*   Error codes `?14.50` and `?14.56` can't happen; SIMH doesn't
    simulate a PDP-12 or a LAB-8/e


## `FRA` Built-In Function

`CARD3.DA` and `CARD4.DA` in the [U/W FOCAL reference cards][uwfr] refer
to a `FRA` built-in function which the [manual][uwfm] does not document.
Lacking documentation, we have not been able to test it. Once we figure
out what it is supposed to do, it will be documented here.

Until then, the three `?17.XX` error codes listed on the refcard are
untested.


## <a id="diffs"></a>Differences Between U/W FOCAL and Other FOCALs

The [DECUS submission for U/W FOCAL][duwf] lists the following
advantages for the version of U/W FOCAL included with the PiDP-8/I
software distribution as compared to FOCAL,1969, FOCAL-8, and OMSI PS/8
FOCAL:

1.  Extended library features with device-independent chaining and
    subroutine calls between programs.

2.  File reading and writing commands, 10 digit precision, full 32k
    memory support, 36 possible functions, 26 possible command letters.

3.  Computed line numbers and unlimited line lengths.

4.  Tabulation on output, format control for scientific notation.

5.  Double subscripting allowed.

6.  Negative exponentiation operators permitted.

7.  `FLOG`, `FEXP`, `FATN`, `FSIN`, `FCOS`, `FITR`, and `FSQT` rewritten
    for 10-digit accuracy.

8.  Character manipulations handled with `FIN`, `FOUT`, and `FIND`.

9.  Function return improvements:

    *   `FSGN(0)=0` in U/W FOCAL; `=1` in FOCAL,1969
    *   `FOUT(A)=0` in U/W FOCAL; `=A` in PS/8 FOCAL

10. n/a; see [above](#miss-hw)

11. 6 special variables are protected from the `ZERO` command: `PI`,
    `!`, `"`, `$`, `%`, and `#`.

    `PI` is initialized as 3.141592654.

12. The limit on the number of variables is 676

13. Text buffer expanded to 15 blocks

14. Two-page handlers permitted

15. Program and file names are wholly programmable. File size may be
    specified. OS/8 block numbers may be used in place of file names.

16. The `OPEN` and `DELETE` commands can have programmed error returns.

17. Improved distribution and random initialization of `FRAN`.

18. `ERASE`, `MODIFY`, and `MOVE` do not erase variables.

19. `WRITE` doesn't terminate a line; `MODIFY` doesn't echo form feed.

20. U/W FOCAL's starting address is 100 (field 0) or 10200 (field 1).


## <a id="converting"></a>Converting Programs from Other Versions of FOCAL

Programs saved by other versions of FOCAL generally don't have the same
format as the core images used by U/W FOCAL. You must therefore use one
of the [text based loading methods](#loading) to save your program text
out of the other FOCAL and load it into U/W FOCAL.

Also, while the `ERASE` command may be used to zero variables in other
FOCALs, you must use `ZERO` for that in U/W FOCAL. If your program
starts off with `ERASE` commands to initialize its variables, there's a
pretty good chance your program will just erase itself under U/W FOCAL.


## <a id="license"></a>License

Copyright © 2017 by Warren Young and Bill Cattey. Licensed under the
terms of [the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
