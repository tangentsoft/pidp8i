# PCB Test Program


## Compiling and Installing

`pidp8i-test` is a simple program to test [Oscar Vermeulen's PiDP-8/I
Kit][project] during construction. It is built and installed alongside
the other software with the normal `make` process.


## Running It

If you are building the software on the Pi for the first time, log out
of your user account after installing it, then back in so that the
install script's changes to your user's `PATH` take effect.

Thereafter, simply give these commands:

    $ sudo systemctl stop pidp8i
	$ pidp8i-test

The first command ensures that the modified PDP-8 simulator is stopped
during the test, since only one program can be talking to the switch and
LED array at a given time. (This also applies to other programs like
[Deeper Thought 2][dt2].)


## Test Procedure

You can at any time hit Ctrl-C to stop the test.

The test proceeds as follows:

*   All On test:

    It turns on all LEDs for 5 seconds.

*   All Off test:

    It turns off all LEDs for 5 seconds.

*   Row test:

    It turns on one full row of LEDs and pauses for 5 seconds, then
    switches to the next row.  There are eight rows of LEDs of up to 12
    LEDs each.

*   Column test:

    It then turns on one full column of LEDs and pauses for 5 seconds,
    then switches to the next column.  There are 12 columns of LEDs with
    up to 8 LEDs each.  (Some of the LEDs positions in a column are
    sometimes rather chaotic, it will require intimate knowledge of the
    schematic to verify.  It's somewhat of a useless test but it might
    turn up an assembly error for someone.)

*   Switch test:

    It then goes into a single LED chase pattern and starts looking at
    switches.  This loop is infinite.  Every time it detects a change in
    the switch positions it prints out the full Octal bit pattern for
    the three switch banks.  No attempt is made to name the actual
    switch that has been flipped.  The goal is to verify switch
    functionality, not to debug the design of the circuit or the driver.

    When running this test, if you get a new line printed with a single
    bit change when you flip a single switch, the switch in question is
    working.  If you get no output printed or multiple bits changed in
    the output printed something is wrong.

    If for some reason you need to decode the output bits to physical
    switches they appear as follows:

    | A    | B    | C   
    |-------------------
    | 4000 | 0000 | 0000

    The first twelve bits (labelled A) is the Switch Register.  The bits
    left to right correspond to the SR switches also left to right.  So
    above the SR1 switch is toggled down, ie 1.  Every other SR switch
    is up, ie 0.

    The leftmost 6 bits (labelled B) are the 3 DF switches followed by
    the 3 IF switches.  Again left to right.  The rest of the bits are
    unused in the B section.

    The leftmost 8 bits (labelled C) are the remaining 8 switches
    starting at "START" and ending at "SING INST".  Again Left to right.


[project]: http://obsolescence.wix.com/obsolescence#!pidp-8
[dt2]:     https://github.com/VentureKing/Deeper-Thought-2
