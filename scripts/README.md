# `os8-run` Scripts

This directory contains scripts to be run by the `os8-run` tool,
which provides a rich language to automate actions of OS/8
under SIMH.  See also the [os8-run documentation][os8-run-doc].


## OS/8 Scripts

The majority of these scripts are for building distributions of OS/8
proper.  These are in the `os8` subdirectory:

### Creation of Installed Images:

`v3d-dist-rk05.os8` -- Construct the `v3d-dist.rk05` OS/8 v3d rk05 image
from distribution media, as configured by auto.def.  This image is
used to build other subsystems that are packaged into the default boot
image.

`v3d-rk05.os8` -- Transform `v3d-dist-rk05` into the default OS/8 boot
image, `v3d.rk05` by integrating separately built subsystems, and applying
patches.

`v3d-src-rk05.os8` -- Construct `v3d-src.rk05`, the OS/8 rk05 image
containing souces from distribution media.

`all-tu56` -- Based on --enable parameters of v3d or v3f with tc08 or
td12k, construct the 4 bootable tu56 images, `v3d-td12k.tu56`,
`v3d-tc08.tu56`, `v3f-td12k.tu56`, and `v3f-tc08.tu56`.  The v3f
images are built using the `v3f-made.rk05` intermediate object image.

`uni-dist-rk05.os8` -- Construct the `uni-dist.rk05` from the Combined
Kit intermediate object images {`uni-sys-build.os8`,
`uni-cusps-build.os8`, `uni-bf2-build.os8`, `uni-fiv-build.os8`} as
configured by auto.def.  This image is used to build other subsystems
that are packaged into the default `uni.rk05` boot image.

`uni-rk05.os8` -- Transform `uni-dist-rk05` into the OS/8 boot image
for normal use, `v3d.rk05` by integrating separately built subsystems,
and applying patches.

### Intermediate Object Images:

`v3f-control.os8` -- Perform assembly or BATCH operations to produce
`v3f-made.rk05` that contains binaries from v3f sources.

`uni-sys-build.os8` -- Assemble OS/8 Combined Kit SYSTEM sources to
create `uni-sys.rk05`.

`uni-cusps-build.os8` -- Assemble OS/8 Combined Kit CUSPS (Commonly
Used System ProgramS) sources to create `uni-cusps.rk05`.

`uni-bf2-build.os8` -- Assemble OS/8 Combined Kit BASIC and FORTRAN II
sources to create `uni-bf2.rk05`.

`uni-fiv-build.os8` -- Assemble OS/8 Combined Kit FORTRAN IV sources to
create `uni-fiv.rk05`.

`uni-sys-build.os8` -- Assemble OS/8 Combined Kit SYSTEM sources to
create `uni-sys.rk05`.

### Utilities:

`cusp--copyin` --  Include script used by all-tu56 to copy Commonly Used
System Programs onto the tu56 image under construction.

`v3d-reset-tc08` -- Used to reset  the default OS/8 v3d rk05 boot image
to use TC08 DECtape device when some script under test leaves it with
the TD8E DECtape device enabled.


## Miscellaneous Scripts

There are a few one-off scripts for building individual components of
the software system in the `misc` subdirectory.  Their purpose is meant
to be obvious from their file name.


## Test Scripts

There are a number of scripts meant for testing `os8-run` itself. These
are in the `test` subdirectory:

| Script Name | Function
|-------------|-------------------------------------------
| version     | Test `begin enabled version` functionality.
| copy        | Test the `cpto` and `cpfrom` os8-run commands.
| err         | Provoke errors in os8-run and test behavior.
| restart     | Test the `restart` os8-run command.
| patch       | Test the `patch` os8-run command.

[os8-run-doc]: /doc/trunk/doc/os8-run.md


## License

Copyright © 2016-2020 by Bill Cattey. This document is licensed under
the terms of [the SIMH license][sl].
