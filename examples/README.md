# How to Use the Examples

This directory holds short examples that you can toggle into the front
panel of your PiDP-8/I and run. The `*.pal` files hold the PAL-III
assembly source code for these programs, and the `*.lst` files hold
the assembler listing output, which contains the octal values you can
use to toggle the programs in.

Take `add.lst` as an example. You will find in that file three columns
of numbers at the beginning of many of the lines:

    10 00200 7300
    11 00201 1206
    12 00202 1207
    13 00203 3210
    14 00204 7402
    15 00205 5200
    16           
    17 00206 0002
    18 00207 0003

The first number refers to the line number in `add.pal`, the second is a
PDP-8 memory address, and the third is the value stored at that address.

Therefore, to toggle the above program into memory, twiddle the switches
like so:

| Set SR Switches To... | Then Toggle...
|------------------------------------------------
| 000 010 000 000       | LOAD\_ADD
| 111 011 000 000       | DEPOSIT
| 001 010 000 110       | DEPOSIT
| 011 010 000 111       | DEPOSIT
| 011 010 001 000       | DEPOSIT
| 111 100 000 010       | DEPOSIT
| 101 010 000 000       | DEPOSIT
| 000 000 000 010       | DEPOSIT
| 000 000 000 011       | DEPOSIT

To run it, reset the switch register (SR) to the starting address (0200)
and then toggle START.

We only need one `LOAD_ADD` operation because all of the memory
addresses in this program are sequential; there are no jumps in the
values in the second column. Not all programs are that way, so pay
attention!

Incidentally, the above program also modifies octal location 0210, the
next memory location after the last one explicitly defined, 0207. That
is the source of the "10" in the lower two digits of the fourth
instruction.
