# `os8-run` Scripts

This directory contains scripts to be run by the `os8-run` tool,
which provides a rich language to automate actions of OS/8
under SIMH.  See also the [os8-run documentation][os8-run-doc].


## OS/8 Scripts

The majority of these scripts are for building distributions of OS/8
proper.  These are in the `os8` subdirectory:

| Script Name    | Function
|----------------|-------------------------------------------
| all-tu56       | Construct 4 tu56 images based on --enable
|                | parameters: v3d or v3f with tc08 or td12k
|                | system heads.
| v3d-dist-rk05  | Construct an OS/8 v3d rk05 image from
|                | distribution media, configured by auto.def
| v3d-rk05       | Apply patches to output of v3d-dist.rk05
|                | to construct a default OS/8 rk05 boot image.
| v3d-src-rk05   | Construct an OS/8 rk05 image containing souces
|                | from distribution media.
| v3d-reset-tc08 | Utility: re-configure the default OS/8 v3d
|                | rk05 boot image to use TC08 DECtape (instead
|                | of TD8E) drivers.
| cusp-copyin    | Include script used by all-tu56 to
|                | copy Commonly Used System Programs onto
|                | the tu56 image under construction.
| v3f-control    | Perform assembly or BATCH operations to produce
|                | binaries from v3f sources.
| uni-\*         | Produce OS/8 Combined Kit images; a.k.a. the
|                | “unified” distro


## Miscellaneous Scripts

There are a few one-off scripts for building individual components of
the software system in the `misc` subdirectory.  Their purpose is meant
to be obvious from their file name.


## Test Scripts

There are a number of scripts meant for testing `os8-run` itself. These
are in the `test` subdirectory:

| Script Name | Function
|-------------|-------------------------------------------
| version     | Test the `begin enabled version` functionality
|             | in os8-run.
| copy        | Test the `cpto` and `cpfrom` os8-run commands.
| err         | Used to provoke errors in os8-run and test
|             | behavior.
| restart     | Test the `restart` os8-run command.
| patch       | Test the `patch` os8-run command.

[os8-run-doc]: /doc/trunk/doc/os8-run.md


## License

Copyright © 2016-2020 by Bill Cattey. This document is licensed under
the terms of [the SIMH license][sl].
