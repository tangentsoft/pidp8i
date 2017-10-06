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
programs, it is in terms of the `PUNCH` command, which has two serious
problems from our perspective:

1.  It was designed for use with paper tapes, which are somewhat more
    clumsy to use in their emulated form within SIMH than actual paper
    tapes. We're faced with either documenting the non-obvious method
    for working with simulated paper tapes via SIMH or documenting a
    non-obvious alternative that doesn't use simulated paper tapes at
    all. We chose the latter.

2.  When you use the `PUNCH` format, it dumps the raw contents of the
    memory buffers, which are not relocateable. This means you have to
    remember which memory page the program was saved from in order to
    load it back up again. If you have two programs you want to load at
    once and they both came from page 3, you've got a problem.

Our alternative method solves both of these problems.

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

Now let's save it to an ASCII text file on the OS/8 disk:

    *OPEN OUTPUT TEST.FC,ECHO
    *W
    C U/W-FOCAL:  16K-V4  NO/DA/TE
    
    01.10 T "HELLO",!
    *OUTPUT CLOSE

Next, we'll break out of the U/W FOCAL environment to get back to OS/8
and show that the file is there, but with lines we don't want:

    *^C                                        ⇠ that is, hit Ctrl-C
    .TYPE TEST.FC
    *W
    C U/W-FOCAL:  16K-V4  NO/DA/TE
    
    01.10 T "HELLO",!
    *OUTPUT CLOSE

So, let's fix it. We'll use OS/8's `EDIT` program for this, but you
could just as well use `TECO` or another text editor you like better:
    
    .R EDIT
    *TEST.FC<TEST.FC
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

It's ugly, but that's `EDIT` for you.

Now let's load it back up into U/W FOCAL and try to run it:

    .R UWF16K
    *OPEN INPUT TEST.FC,ECHO
    *C U/W-FOCAL:  16K-V4  NO/DA/TE
    *
    *01.10 T "HELLO",!
    *                                          ⇠ hit Enter
    *G
    HELLO

Success!

We added the `,ECHO` bits in the commands above only to make U/W FOCAL
echo what it's doing to the terminal to make the transcripts clearer.
In your own work, you might want to leave this off.

[domsi]: http://www.pdp8.net/pdp8cgi/query_docs/view.pl?id=366


## <a id="card2"></a>Command Summary (`CARD2.DA`)

The sub-sections below are a reformatted version of the text from the
file `CARD2.DA` contained within the distribution of U/W FOCAL V4E which
we used in building the version of U/W FOCAL currently shipping with the
PiDP-8/I software distribution.

In the descriptions below, arguments in square brakets are optional.
Specify the argument, but don't include the square brakets. If a space
is in the square brackets, a space is required to provide the argument.


### Miscellaneous Commands

| `O D`   | Output Date           | Prints system date in the form `DD.MM.YY`  |
| `L E`   | Logical Exit          | Returns to the OS/8 keyboard monitor       |
| `L B`   | Logical Branch _L1_   | Branches to _L1_ if -no- input from TTY    |
| `J  `   | Jump _L1_.            | Equivalent to the Logical Branch command   |


### Filesystem Directory Commands

| `L A,E` | List All [name][`,E`]         | Lists all files after the one specified  |
| `L O`   | List Only [_name_]\*          | Verifies the existence of one `.FC` file |
| `O L`   | Only List [_name_]\*          | Verifies the existence of one `.DA` file |
| `L L`   | Library List [_name_]%        | Shows files with the same extension      |
| `L D`   | Library Delete name [ _L1_]   | Removes a name from the directory        |
| `L Z`   | Library Zero dev:[_length_]   | Zeros directory using length given       |

Notes on Directory Commands:

`E`  Adding the phrase `,E` will list all of the 'empties' too

*  Omitting the name lists all files with the same extension

%  A null extension will list all files having the same name


### Program Commands

| `L C`   | Library Call name          | Loads a program, then Quits        |
| `L G`   | Library Gosub name [ _G1_] | Calls a program as a subroutine    |
| `L R`   | Library Run name [ _L1_]   | Loads a program and starts at _L1_ |
| `L N`   | Library Name [_name_]      | Changes the program header         |
| `L S`   | Library Save name [ _L1_]  | Saves the current program          |

[ _G1_] indicates which line or group will be called by `L G`

[ _L1_] specifies an error return, except for the `L R` command


### Input / Output Commands

| `O A`   | Output Abort [_E1_]                  | Terminates output file with length _E1_  |
| `O B`   | Output Buffer                        | Dumps buffer without closing the file    |
| `O C`   | Output Close [_E1_]                  | Ends output, saves file with length _E1_ |
| `O I,E` | Open Input [`,Echo`]                 | Selects the terminal for input           |
| `O O`   | Open Output                          | Selects the terminal for output          |
| `O S`   | Output Scope                         | Selects CRT for output (if available)    |
| `O I -` | Open Input name [`,E`] [ _L1_]       | Switches input to an OS/8 device         |
| `O S -` | Open Second name [`,Echo`] [ _L1_]   | Selects a second input file              |
| `O O -` | Open Output name [`,Echo`] [ _L1_]   | Initiates OS/8 (file) output             |
| `O E -` | Output Everything device [`,Echo`]   | Changes error/echo device                |
| `O R R` | Open Restart Read [`,Echo`]          | Restarts from the beginning              |
| `O R I` | Open Resume Input [`,Echo`] [ _L1_]  | Returns to file input                    |
| `O R O` | Open Resume Output [`,Echo`] [ _L1_] | Returns to file output                   |
| `O R S` | Open Resume Second [`,Echo`] [ _L1_] | Returns to second input file             |

The `INPUT ECHO` sends characters to the current `OUTPUT` device

The `OUTPUT ECHO` sends characters to the current 'O E' device


### Filename Expressions

Device and filenames may be written explicitly: `RXA1:`, `MYSTUF`,
`0123.45`.

Numeric parts can be computed from (expressions):
`DTA(N):PROG(X).(A+B)`.

Negative values specify single characters:
`F(-201)L(-197,.5,PI)=FILE03`.

An \<OS/8 block number\> can be substituted for the name:
`LTA1:<20*BN+7>`.

Expressions in square brackets indicate the size: `TINY[1]`,
`<LOC>[SIZE]`.


### <a id="variables"></a>Variables

Variable names may be any length, but only the first two characters are
stored; the first character may not be an `F`. Both single and double
subscripts are allowed - a subscript of 0 is assumed if none is given.
The variables `!`, `"`, `#`, `$`, `%` and `PI` are protected from the
`ZERO` command and do not appear in table dumps. `!` is used for double
subscripting and should be set to the number of rows in the array.  `#`,
`$`, `%` are used by [FOCAL Statement Functions](#fsf). The `ZVR`
feature permits non-zero variables to replace any which are zero. This
includes `FOR` loop indices, so use a protected variable if the index
runs through zero.  Undefined or replaced variables are automatically
set to zero before their first use.


### <a id="fsf"></a>FOCAL Statement Functions

`F(G1,E1,E2,E3)` executes line or group `G1` after first setting the
variables `#`,`$`,`%` to the values of `E1`,`E2`,`E3` (if any).  The
function returun with the value of the last arithmetic expression
processed by the sub routine, including line number & subscript
evaluations.   For example:

    8.1 S FSIN(#)/FCOS(#) is the TANGENT function = F(TAN,A) if 'TA' = 8.1
    9.1 S FEXP($*FLOG(#)) computes X^Y for any value of Y using F(9.1,X,Y)


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
mangled variable name and `a`, `b`, and `c` above do not because the
workaround is simple: keep <kbd>CAPS LOCK</kbd> engaged while typing
FOCAL programs except when typing text you want FOCAL to send back out
to the terminal:

    *1.1 TYPE "Hello, world!"!
    *G
    Hello, world!

See the [Variables section](#variables) of [`CARD2.DA`](#card2) for more
information on variable naming.


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

The first two qustions have the same answer: U/W FOCAL reserves space
for the sign in its numeric outputs even if it doesn't end up being
needed. This was done, no doubt, so that columns of positive and
negative numbers line up nicely. It might help to see what's going on if
you mentally replace the spaces in that first output line above with `+`
signs.

This then explains the discrepancy between the first and second
commands' outputs raised by the second question above: the first output
of the first command is positive, while the second command's output is
negative, so there is a space at the beginning of the first output for
the implicit `+` sign.

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

## Error Code Table

For extreme economy of memory, FOCAL does not print error message strings.
Instead, an error routine prints a question mark followed by a four digit
fixed point number corresponding to where in the FOCAL runtime executable
the error was encountered.

I.E. If an error was encountered in the FOCAL interpreter's parsing
of a variable name, the error message prints out the error message
traceable to that parser within FOCAL.

This means that an error table must be produced, and every time code shifts
around, the error table must be updated.

The U/W FOCAL manual contains an error table, but it is incomplete.
Here is a complete one which comes from the file CARD4.DA in the
U/W FOCAL archive from which this distribution is taken.

Errors appearing in bold face denotes an error from a command with an
optional error return.

|     Error     |        Meaning                                              |
| ------------- | ----------------------------------------------------------- |
|   `?01.00`    | Keyboard interrupt or restart from location 10200           |
| __`?01.03`__  | Secondary input file missing                                |
| __`?01.11`__  | No secondary input file to resume                           |
|   `?01.50`    | Group number greater than 31                                |
|   `?01.93`    | Non-existent line number in a MODIFY or MOVE command        |
|   `?03.10`    | Line called by `GO`, `IF`, `J`, `R`, `Q`, `L` `B`, or `L R` is missing      |
|   `?03.30`    | Illegal command                                             |
|   `?03.47`    | Line or group missing in `DO`, `ON`, `JUMP`, `L GOSUB` or a `FSF`     |
|               |                                                             |  
|   `?04.35`    | Bad syntax in a `FOR` command (missing semicolon?)            |
|   `?06.03`    | Illegal use of a function or number: `ASK`, `YNCREMENT`, `ZERO`   |
|   `?06.41`    | Too many variables (ZERO unnecessary ones)                  |
|   `?07.44`    | Operator missing or illegal use of an equal sign            |
|   `?07.67`    | Variable name begins with `F` or improper function call     |
|   `?07.76`    | Double operators or an unknown function                     |
|   `?08.10`    | Parentheses don't match                                     |
|   `?10.50`    | Program too large                                           |
|               |                                                             |
|   `?12.10`    | Error detected in the `BATCH` input file                      |
|   `?12.40`    | Keyboard buffer overflow (eliminated in 8/e versions)       |
|   `?13.65`    | Insufficient memory for `BATCH` operation                     |
|   `?14.15`    | Display buffer overflow                                     |
|   `?14.50`    | Bad Sense Switch number on a PDP12 (range is 0-5)           |
|   `?14.56`    | Illegal external sense line (PDP12 range is 0-11)           |
|   `?17.22`    | `FRA` not initialized                                         |
|   `?17.33`    | `FRA` index too large (exceeds file area)                     |
|   `?17.62`    | `FRA` mode error: only modes 0,1,2,4 allowed                  |
|               |                                                             |
|   `?18.42`    | `FCOM` index too large: reduce program size                   |
|   `?19.72`    | Logarithm of zero                                           |
|   `?21.57`    | Square root of a negative number                            |
|   `?22.65`    | Numeric overflow: too many digits in a string               |
|   `?23.18`    | `OUTPUT` `ABORT` or `CLOSE` requested too much space              |
|   `?23.37`    | Output file overflow: recover with: `O O name;O A FLEN()`     |
| __`?23.82`__  | Cannot open output file (file open, too big or no name)     |
| __`?24.05`__  | No output file to resume                                    |
|               |                                                             |
|   `?24.25`    | Illegal `OPEN` command                                        |
|   `?24.35`    | Illegal `RESUME` command                                      |
| __`?24.40`__  | Input file not found (wrong name? wrong device?)            |
|   `?24.47`    | No input file to restart                                    |
| __`?24.52`__  | No input file to resume                                     |
|   `?25.02`    | Stack overflow: reduce nested subroutines and expressions   |
| __`?25.60`__  | Device does not exist or illegal 2-page handler             |
|   `?26.07`    | Illegal `LIBRARY` command                                     |
|   `?26.32`    | File specified is already deleted (wrong extension?)        |
|               |                                                             |
|   `?26.39`    | File loaded is not a FOCAL program - __better reload UWF!__     |
|   `?26.56`    | Program requested is missing (wrong device?)                |
|   `?26.66`    | `LIBRARY SAVE` error: no name, device full, or no directory   |
|   `?27.18`    | Attempted `LIBRARY` operation on a device without a directory |
|   `?27.75`    | No length specified in a `LIBRARY ZERO` command               |
|   `?27.90`    | Zero divisor                                                |
|   `?29.25`    | Cannot use the '<>' construction with `OPEN OUTPUT`           |
|   `?29.38`    | Device error (write-lock, bad checksum or illegal request)  |



`_`   Indicates EOF detected in input - I/O continues from terminal

`?....?`   TRACE feature: Text enclosed by `?` marks is typed during execution
to help find the source of an error. The value of each expression in a SET
command is also printed

## Summary Command, Operator, and Internal Function Reference Cards

The reference card files CARD1.DA and CARD3.DA provide a helpful overall
summary:

### Single Letter Commands

| `A` | Ask [`"QUERY"`,`X`,`:`,`!`]  | Accepts value of `X` from input device         |
| `B` | Break [_L1_]%                | Exits from a FOR loop, continuing at _L1_      |
| `C` | Comment                      | Ignores the rest of the line                   |
| `D` | Do [_G1_,_G2_,_G3_,etc.]     | Calls a line or a group as a subroutine        |
| `E` | Erase [_G1_]                 | Deletes all or part of the program             |
| `F` | `For X=`_E1_`,`[_E2_`,`] _E3_`;`(_commands_) | Executes line `1+(_E3_-_E1_)/_E2_` times               |
| `G` | Goto [_L1_]                  | Branches to line _L1_                            |
| `H` | Hesitate [_E1_]*             | Delays (or synchronizes) the program           |
| `I` | If `(E1)` [_L1_,_L2_,_L3_]%  | Transfers to _L1_,_L2_,_L3_ on sign of _E1_            |
| `J` | Jump `(`_E1_`)` [_G1_,_G2_,_G3_,_G4_...]%  |   Calls the subroutine selected by _E1_          |
| `K` | Kontrol [_E1_,_E2_,etc]*     | Controls relays or other digital output        |
| `L` | Library/List                 | Two-letter commands, see the next page         |
| `M` | Modify [_L1_,_L2_]           | Edits and/or Moves line L1 - see below         |
| `N` | Next [_L1_]%                 | Ends a `FOR` loop, branches to _L1_ when finished  |
| `O` | On `(`_E1_`)` [_G1_,_G2_,_G3_]%          | Calls subroutine selected by sign of _E1_        |
| `P` | Plot [_X_,_Y_,_L_,_M_]*              | Controls an analog or digital plotter          |
| `Q` | Quit [_L1_]%                   | Stops program, allows restart at _L1_            |
| `R` | Return [_L1_]%                 | Exits from a subroutine call, continuing at _L1_ |
| `S` | Set [_E1_,_E2_,_E3_,etc.]          | Evaluates arithmetic expressions               |
| `T` | Type [_E1_,`"TEXT"`,`!`,`#`,`:`,`%`,`$`]   | Generates alphanumeric output                  |
| `U` | User                         |                                                |
| `V` | View [_X_,_Y_,_Z_]*                | Generates graphic output on a CRT              |
| `W` | Write [_G1_,_G2_,_G3_,etc.]        | Lists all or part of a program                 |
| `X` | Xecute                       |  Equivalent to SET                             |
| `Y` | Yncrement [_X_,_Y_`-`_Z_]            | Increments or decrements variables             |
| `Z` | Zero [_X_,_Y_,...]               | Sets some or all of the variables to zero      |

* Indicates a non-standard (installation dependent) feature

% If the line number is omitted (or=0) no branch will occur

_En_ are Arithmetic Expressions - - [] Enclose optional items
       
_Ln_ are Line Numbers from `0.01` to `31.99` - excluding integers

_Gn_ are Line or Group Numbers from `0` to `+31` (`0` = next or all)

Line numbers `.01` to `.99` refer to lines in the current group
Negative or Integer line numbers denote a 'Group' operation

Arithmetic expressions may be used as Line or Group numbers

### Arithmetic Operators

|   | ( )  [ ]  < >           | Three equivalent sets of enclosures     |
| ' | Character value         | `'A` is the value of the letter `A`     |
| ^ | Exponentiation          | Positive or negative integer powers     |
| * | Multiplication          | Note especially that multiplication     |
| / | Division                | has a higher priority than division     |
| - | Subtraction or Negation | Example: (to illustrate priorities)     |
| + | Addition                | `-5^4/3*A=2+1` is `0-<5^4>/[3*(A=2+1)]` |
| = | Replacement             | May be used anywhere in expressions     |

### Ask/Type Operators

| , | COMMA or SPACE           | Separates variables and/or expressions         |
| ! | Carriage return/linefeed | Starts a new line for input or output          |
| " | String delimiter         | Case shift option uses `\`: `"A\B\C"=AbC`      |
| # | Return or Clear Screen   | Used for plotting or overprinting              |
| $ | Symbol table listing     | `TYPE $4` prints 4 variables per line          |
| : | Tabulation               | `ASK :-15`  skips over the next 15 characters  |
|   | (:0 is ignored)          | `TYPE :15`  spaces to column 15 if not beyond  |
| % | Format control           | `%3` Produces 3 Digits in an integer format    |
|   | (for output only)        | `%0.04` =  4 Digits using scientific notation  |
|   | (input is unformatted)   | `%5.02` =  5 Digits, 2 decimal places maximum  |

Letters (but only one E) are legal numeric input: `YES=25E19`

`ALTMODE` or `ESCAPE` aborts input, with the variable unchanged

`_` deletes all digits during input -- `RUBOUT` is ignored


### Modify / Move Operators

| `CTRL/F`                     | Aborts the command leaving the line unchanged |
| `CTRL/G` (bell)              | Selects a new search character                |
| `CTRL/L` (does not echo)     | Searches for next occurrence of character     |
| `_` (backarrow or underline) | Deletes all characters to the left            |
| `RETURN`                     | Terminates the line at the current position   |
| `LINEFEED`                   | Copies the remainder of the line unchanged    |
| `RUBOUT`/`DELETE`            | Removes the previous character, echos a `\`   |


`RUBOUT` or `DELETE` and `_` also work during command input

`LINEFEED` retypes the corrected input line for verification

### Internal Functions

| `FABS(`_E1_`)` |               Returns the absolute value of the argument |
| `FADC(`_N_`)` |           Reads A/D converter channel N (LAB/8e or PDP12`)` |
| `FATN(`_A_`)` |           Computes the arctangent of _A_, result in radians |
| `FBLK(``)` |                OS/8 block number of the current input file |
| `FBUF(`_I_`,`_V_`)` |               Display buffer storage (single-precision) |
| `FCOM(`_I_`,`_V_`)` |               Extended data storage in Fields 2 and 4-7 |
| `FCOS(`_A_`)` |                Computes the cosine of _A_ (_A_ is in radians) |
| `FCTR(`_N_`)` |                Reads a frequency counter using timebase _N_ |
| `FDAC(`_N_`,`_V_`)` |             Sets D/A converter channel _N_ to the value _V_ |
| `FDAY(`_MONTH*256+DAY*8+YEAR-78_`)` |     Reads/Sets the OS/8 system date |
| `FDIN(`_B1_`,`_B2_`,`...`,`_Bn_`)` |    Reads selected bits from the input register |
| `FDVM(`_N_`,`_R_`)` |           Reads a digital voltmeter, channel _N_, range _R_ |
| `FEXP(`_E1_`)` |                 Base 'e' exponential function  `\|`_E1_`\|<1420` |
| `FIN()` |           Reads a single character, returns the ASCII value |
| `FIND(`_C_`)` |       Searches for code _C_, returning _C_ if found, 0 if `EOF` |
| `FITR(`_E1_`)` |                 Returns the integer part of the argument |
| `FJOY(`_I_`)` |         Places the cursor (joystick) coordinates in _XJ_,_YJ_ |
| `FLEN(`_I_`)` |        File length: _I_=`0` for `O`utput, _I_=`1` for `I`nput |
| `FLOG(`_E1_`)` |            Natural logarithm of the absolute value of _E1_ |
| `FLS()` |         Returns unsigned value of the Left Switches (PDP12) |
| `FMIN(`A_`,`_B`)` |       Returns the minimum or argument |
| `FMAX(`A_`,`_B`)`  |       Returns the maximum argument |
| `FMQ(`_N_`)` |         Displays the lower 12 bits of _N_ in the `MQ` register |
| `FOUT(`_C_`)` |             Outputs character code _C_, returns the value `0` |
| `FRA(`_I_`,`_V_`)` |           Reads or writes in a binary file at location I |
| `FRAC(`_E1_`)` |              Returns the fractional part of the argument |
| `FRAN(`__`)` |                   Pseudo-random number function, range 0-1 |
| `FSAM(`_N_`)` |           Samples _N_ channels and stores results in buffer |
| `FSGN(`_E1_`)` |          Returns `-1`,`0`,`+1` for _E1_ negative, zero, positive |
| `FSIN(`_A_`)` |                  Computes the sine of _A_ (_A_ is in radians) |
| `FSQT(`_E1_`)` |              Finds the square root using Newton's method |
| `FSR()` |      Reads the Switch Register |
| `FRS()` |       Reads the Right Switches on a PDP-12 |
| `FSS(`_N_`)` |                    Tests Sense Switch _N_: `-1` = `OFF`, `+1` = `ON` |
| `FTIM(`_N_`)` |            Reads, sets or clears the elapsed time counter |
| `FTRG(`_N_`)` |               Returns status and clears Schmitt trigger _N_ |
| `FTRM(`__`)` |                          Returns the last input terminator |
| `FXL(`_N_`)` |          Tests external level _N_ (PDP12) returning `-1` or `+1` |

And others. There are a total of 36 possible function names

Functions indicated by a * are not available in all versions

The functions FBLK & FLEN are useful in filename expressions

`FIN`, `FOUT`, `FIND` and `FTRM` use decimal ASCII codes - see below.

### Decimal ASCII Character Codes

| Code | Character            | Code  | Char             | Code  | Char   | Code  | Char    |
| ---- | -------------------- | ----- | ---------------- | ----- |------- | ----- | ------- |
| 128  | `CTRL/@` (leader/    | 152   | `CTRL/X`         | 176   | `0`    | 201   | `I`       |
|      | trailer-ignored)     | 153   | `CTRL/Y`         | 177   | `1`    | 202   | `J`       |
| 129  | `CTRL/A`             | 154   | `CTRL/Z` (`EOF`) | 178   | `2`    | 203   | `K`       |
| 130  | `CTRL/B`             | 155   | `ESCAPE`  or     | 179   | `3`    | 204   | `L`       |
| 131  | `CTRL/C` (OS/8)      |       | `CTRL/[`         | 180   | `4`    | 205   | `M`       |
| 132  | `CTRL/D`             | 156   | `CTRL/\`         | 181   | `5`    | 206   | `N`       |
| 133  | `CTRL/E`             | 157   | `CTRL/]`         | 182   | `6`    | 207   | `O`       |
| 134  | `CTRL/F` (`BREAK`)   | 158   | `CTRL/^`         | 183   | `7`    | 208   | `P`       |
| 135  | `CTRL/G` (`BELL`)    | 159   | `CTRL/_`         | 184   | `8`    | 209   | `Q`       |
| 136  | `CTRL/H` (`BACKSP`)  | 160   | `SPACE`          | 185   | `9`    | 210   | `R`       |
| 137  | `CTRL/I` (`TAB`)     | 161   |  `!`             | 186   | `:`    | 211   | `S`       |
| 138  | `LINEFEED`           | 162   |  `"`             | 187   | `;`    | 212   | `T`       |
| 139  | `CTRL/K` (`LINEUP`)  | 163   |  `#`             | 188   | `<`    | 213   | `U`       |
| 140  | FORMFEED             | 164   |  `$`             | 189   | `=`    | 214   | `V`       |
| 141  | RETURN               | 165   |  `%`             | 190   | `>`    | 215   | `W`       |
| 142  | `CTRL/N`             | 166   |  `&`             | 191   | `?`    | 216   | `X`       |
| 143  | `CTRL/O`             | 167   |  `'` (`APOST`)   | 192   | `@`    | 217   | `Y`       |
| 144  | `CTRL/P`             | 168   |  `(`             | 193   | `A`    | 218   | `Z`       |
| 145  | `CTRL/Q` (`X-ON`)    | 169   |  `)`             | 194   | `B`    | 219   | `[`       |
| 146  | `CTRL/R`             | 170   |  `*`             | 195   | `C`    | 220   | `\`       |
| 147  | `CTRL/S` (`X-OFF`)   | 171   |  `+`             | 196   | `D`    | 221   | `]`       |
| 148  | `CTRL/T`             | 172   |  `,` (comma)     | 197   | `E`    | 222   | `^`       |
| 149  | `CTRL/U`             | 173   |  `-` (minus)     | 198   | `F`    | 223   | `_`       |
| 150  | `CTRL/V`             | 174   |  `.` (period)    | 199   | `G`    | 253   | `ALTMODE` |
| 151  | `CTRL/W`             | 175   |  `/`             | 200   | `H`    | 255   | `RUBOUT`  |

Codes 224-250 are lower case letters.  Codes 000-127 are similar
to codes 128-255 except that the parity bit has been eliminated.

Many keyboards use `SHIFT/K`, `/L`, `/M`, `/N`, `/O` for `[`, `\`, `]`, `^` and `_`

A single quote before a character indicates the-value-of: `'A=193`
Use `CTRL/@` to page the TV display to avoid getting error `12.40`

To erase the screen on a Tektronix terminal: `S FOUT(27) FOUT(12)`

To make a copy: `S FOUT(27) FOUT(23)`.  Note:  `FOUT(27)` = `ESCAPE`

To make bold letters on a Centronics printer: `T :FOUT(14) "text"`

To set 'Hold Screen' mode (VT50 terminals):  `S FOUT(27) FOUT(91)`

To rubout the last character on the PDP12/LAB8e display `FOUT(92)`


## <a id="license"></a>License

Portions copyright © 2017 by Warren Young and Bill Cattey, whose
contributions are licensed under the terms of [the SIMH license][sl].

The sections referencing `CARD[1-4].DA` are reformatted versions of
material from the U/W FOCAL distribution and are copyrighted by their
authors.

[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
