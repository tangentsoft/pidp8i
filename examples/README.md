# How to Use the Examples

The `examples` directory holds short example programs for your PiDP-8/I.
For each program there are 3 files:

| Extension | Meaning
-----------------------------
| `*.pal`   | the PAL-III assembly source code for the program
| `*.lst`   | the human-readable assembler output
| `*.pt`    | the machine-readable assembler output

There are three ways to run these on your PiDP-8/I, each starting with
one of the above three files:

1.  Transcribe the assembly program text to a file within a PDP-8
    operating system and assemble it inside the simulator. For example,
	in OS/8:

	    .EDIT         then input the program, saving as RKB0:EXAMPLE.PA
		.R PAL8
		*RKB0:,RKB0:<RKB0:EXAMPLE

    That ugly final line loads `EXAMPLE.PA` from the second half of the
    first RK05 hard disk, then writes out the machine-readable and
    human-readable output files to the same disk as `EXAMPLE.BN` and
    `EXAMPLE.LS`, respectively. (Are you missing your `make` program
    yet?)

    This method is the most educational, as it matches the working
    experience of PDP-8 assembly language programmers back in the day.
    The tools may differ — the user may prefer `TECO` over `EDIT` and
    MACRO-8 over PAL8 — but the idea is the same regardless.

2.  Toggle the program in from the front panel as described in the
    following section. I can recommend this method only for very short
    programs.

3.  Copy the `*.pt` file to a USB stick and use the PiDP-8/I's
    [automatic media mounting feature][1]. This is the fastest method.


## Toggling Programs in Via the Front Panel

Take `add.lst` as an example, in which you will find three columns of
numbers on the code-bearing lines:

    10 00200 7300
    11 00201 1206
    12 00202 1207
    13 00203 3210
    14 00204 7402
    15 00205 5200
    17 00206 0002
    18 00207 0003

The first number refers to the corresponding line number in `add.pal`,
the second is a PDP-8 memory address, and the third is the value stored
at that address.

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


[1]: http://obsolescence.wixsite.com/obsolescence/how-to-use-the-pidp-8
