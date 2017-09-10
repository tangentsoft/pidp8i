# Software Subsystems


## Directory Contents

The files in this directory are used during the PiDP-8/I software build
process along with the DEC original tape images in
[`../os8`](/files/media/os8) to build the `os8v3d-bin.rk05` disk image
used by boot options IF=0 and IF=7.

These files are here rather than in `../os8` because they are not part
of OS/8 *per se*. Some of them do require OS/8 to run and so have "OS/8"
in their description below, while the others could potentially be used
with other PDP-8 operating systems or without an OS entirely.


| DECtape Image File Name | Content Description
| ----------------------------------------------------------------------------
| `advent.tu56`           | [OS/8 Adventure][os8a] v2.4
| `ba.tu56`               | Several \*.BA programs from DEC's book ["101 BASIC Computer Games"][bcg]
| `cc8.tu56`              | Ian Schofield's [CC8 OS/8 C compiler system][cc8]
| `k12.tu56`              | [Kermit-12][k12] for OS/8, OS/78, OS/278, and OS/12
| `music.tu56`            | [RFI-based][rfi] music playback programs


## Controlling the Build Process

Most of these files are merged into the OS/8 binary disk image by
default, but can be excluded by giving a `--without-NAME` flag to the
`configure` script, where `NAME` is the file name above without the
`.tu56` extension. (e.g. `--without-k12` excludes Kermit-12.)

Only one of the files above is currently excluded by default, that being
`music.tu56`, because we have not yet received any report of reliable
playback. We believe this is because the PiDP-8/I realization does not
lend itself to creation of suitable AM frequency RFI. These programs
were written for real PDP-8 hardware which had much longer wires backed
by much stronger drivers than a PiDP-8/I, and which ran at lower
frequencies than a Raspberry Pi. These problems are not insurmountable,
so someone interested in the project may force inclusion of these files
on the OS/8 RK05 boot disk with

     $ ./configure --with-music

Solving this problem may require hardware modifications. If so, we'll
still exclude these programs by default since not all PiDP-8/I machines
will have these modifications.

See the [top-level `README.md` file][tlrm] for further information about
the `--with-*` and `--without-*` configuration options.


[bcg]:  https://archive.org/details/bitsavers_decBooks10Mar75_26006648
[cc8]:  https://groups.google.com/d/msg/pidp-8/ycs_KOI4vdg/Zr0bifJxAgAJ
[k12]:  http://www.columbia.edu/kermit/pdp8.html
[os8a]: http://www.rickmurphy.net/advent/
[rfi]:  https://en.wikipedia.org/wiki/Electromagnetic_interference
[tlrm]: /doc/trunk/README.md
