# Example Programs


## How to Use the Examples

The `examples` directory holds short example programs for your PiDP-8/I.
For each program there are 3 files:

| Extension | Meaning
-----------------------------
| `*.pal`   | the PAL-III assembly source code for the program
| `*.lst`   | the human-readable assembler output
| `*.pt`    | the machine-readable assembler output (RIM format)

There are three ways to run these on your PiDP-8/I, each starting with
one of the above three files:

1.  Transcribe the assembly program text to a file within a PDP-8
    operating system and assemble it inside the simulator.

2.  Toggle the program in from the front panel. I can recommend this
    method only for very short programs.

3.  Copy the `*.pt` file to a USB stick and use the PiDP-8/I's
    [automatic media mounting feature][howto]. This is the fastest method.

I cover each of these options below, in the same order as the list
above.


## Option 1: Transcribing the Examples into an OS/8 Session

To transcribe [`examples/add.pal`][pal] into the OS/8 simulation on a
PiDP-8/I:

    .R EDIT
    *RKB0:ADD.PA<

    #A                          ← append to ADD.PA
    *0200   CLA CLL
    MAIN,   TAD A
            TAD B
            DCA C
            HLT
    A,      2
    B,      3
    C,
                                ← hit Ctrl-L to leave text edit mode
    #E                          ← saves program text to disk

    .PAL ADD-LS
    ERRORS DETECTED: 0
    LINKS GENERATED: 0

    .DIR ADD.* /A

    ADD   .PA   1             ADD   .BN   1             ADD   .LS    1

     399 FREE BLOCKS

If you see some cryptic line from the assembler like `DE C` instead
of the `ERRORS DETECTED: 0` bit, an error has occurred. Table 3-3 in
my copy the OS/8 Handbook explains these. You will also have an `ADD.ER`
file explaining what happened.

You can instead say `EXE ADD` to assemble and execute that program in a
single step, but beware that because the program halts the processor,
your OS/8 session also halts. If you take the opportunity as intended to
examine memory location `C` — 0207 — pressing `Start` to resume will
cause the processor to try executing the instruction at 0210, and who
knows what that will do? Even if you pass up the opportunity to examine
`C`, pressing `Start` immediately after the halt will do the same,
except that we know what it will do: it will try to execute the 0002
value stored at `A` as an instruction! (I believe it means `AND` the
accumulator with memory location 2.)

The solution to these problems is simple:

    .EDIT ADD                   ← don't need "R" because file exists
    #R                          ← read first page in; isn't automatic!
    #4D                         ← get rid of that pesky DCA line
    #5I                         ← insert above "A" def'n, now on line 5
            JMP 7600            ← Ctrl-L again to exit edit mode
    #E                          ← save and exit

    .EXE ADD

As before, the processor stops, but this time because we didn't move the
result from the accumulator to memory location `C`, we can see the
answer on the accumulator line on the front panel. Pressing `Start` this
time continues to the next instruction which re-enters OS/8. Much nicer!

As you can see, this option is the most educational, as it matches
the working experience of PDP-8 assembly language programmers back
in the day. The tools may differ — the user may prefer `TECO` over
`EDIT` or MACRO-8 over PAL8 — but the idea is the same regardless.


## Option 2: Toggling Programs in Via the Front Panel

Take [`add.lst`][lst] as an example, in which you will find three
columns of numbers on the code-bearing lines:

    10 00200 7300
    11 00201 1205
    12 00202 1206
    13 00203 3207
    14 00204 7402
    16 00205 0002
    17 00206 0003

The first number refers to the corresponding line number in
[`add.pal`][pal], the second is a PDP-8 memory address, and the third is
the value stored at that address.

To toggle the `add` program in, press `Stop` to halt the processor, then
twiddle the switches like so:

| Set SR Switches To... | Then Toggle...
|------------------------------------------------
| 000 010 000 000       | `Load Add`
| 111 011 000 000       | `Dep`
| 001 010 000 101       | `Dep`
| 001 010 000 110       | `Dep`
| 011 010 000 111       | `Dep`
| 111 100 000 010       | `Dep`
| 000 000 000 010       | `Dep`
| 000 000 000 011       | `Dep`

To run it, repeat the first step in the table above, loading the
program's starting address (0200) first into the switch register (SR)
and then into the PDP-8's program counter (PC) via `Load Add`. Then
toggle `Start` to run the program. If you then:

If you then toggle 000 010 001 000 into SR, press `Load Add` followed by
`Exam`, you should see 000 000 000 010 in the third row of lights, the
bit pattern for five, that being the correct answer for "2 + 3", which
is what `add.pal` computes. You could load that address back up again
and `Dep` a different value into that location, then start the program
over again at 0200 to observe that this memory location does, indeed,
get overwritten with 0005.

We only need one `Load Add` operation in the table above because all of
the memory addresses in this program are sequential; there are no jumps
in the values in the second column. Not all programs are that way, so
pay attention!

Incidentally, the above program also modifies octal location 0207, the
next memory location after the last one explicitly defined, 0206. That
is the source of the "07" in the lower two digits of the fourth
instruction.


## Option 3: Loading Programs from Paper Tape

The `example/*.pt` files shipped with the software distribution are in
RIM format, so the simple way to load them into your PiDP-8/I is to copy
them to USB sticks, one file per stick. The following is distilled from
the [How to Use the PiDP-8/I][howto] section of the PiDP-8/I
documentation:

1.  Set the IF switches (first white group) to 001, and toggle `Sing
    Step` to reboot the simulator into the high-speed RIM loader. If the
    simulator wasn't already running, restarting the simulator with IF=1
    will achieve the same end as toggling `Sing Step` while it's
    running. Reset the IF switches to 0.

2.  Insert the USB stick containing the `*.pt` file you want to load
    into the Pi.

3.  Set the DF switches (first brown group) to 001, then toggle `Sing
    Step` again. This attaches the tape to the high-speed paper tape
    reader peripheral within the PDP-8 simulator. Set DF back to 0.

4.  Set the switch register (SR) to 7756 (111 111 101 110) then press
    `Load Add`, then `Start`.

5.  Hit `Stop`, then reset SR to 0200 (000 010 000 000), that being the
    starting location of these example programs. Press `Load Add`, then
    `Start` to run the program.

There is an SVG template for USB stick labels in the distribution under
the [`labels/`][label] directory, for use if you find yourself creating
long-lived USB sticks. See [`labels/README.md`][lread] for more
information.


[lst]:   https://tangentsoft.com/pidp8i/doc/trunk/examples/add.lst
[pal]:   https://tangentsoft.com/pidp8i/doc/trunk/examples/add.pal
[label]: https://tangentsoft.com/pidp8i/dir?ci=trunk&name=labels
[lread]: https://tangentsoft.com/pidp8i/doc/trunk/labels/README.md
[howto]: http://obsolescence.wixsite.com/obsolescence/how-to-use-the-pidp-8
