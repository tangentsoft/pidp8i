# OS/8 Scripts

This directory contains scripts to be run by the os8-run tool,
which provides a rich language to automate actions of OS/8
under SIMH.  See also the [os8-run documentation][os8-run-doc].

Here is an inventory of the scripts and their actions.

| Script Name        | Function
|--------------------|-------------------------------------------
| v3d-dist-rk05.os8  | Construct an OS/8 v3d rk05 image from
|                    | distribution media, configured by auto.def
| v3d-rk05.os8       | Apply patches to output of v3d-dist.rk05.os8
| 		     | to construct a default OS/8 rk05 boot image.
| v3d-src-rk05.os8   | Construct an OS/8 rk05 image containing souces
| 		     | from distribution media.
| all-tu56.os8	     | Construct 4 tu56 images based on --enable
| 		     | parameters: v3d or v3f with tc08 or td12k
|		     | system heads.
| cusp-copyin.os8    | Include script used by all-tu56.os8 to
| 		     | copy Commonly Used System Programs onto
|		     | the tu56 image under construction.
| v3f-control.os8    | Perform assembly or BATCH operations to produce
| 		     | binaries from v3f sources.

There are also utility/test scripts:

| Script Name        | Function
|--------------------|-------------------------------------------
| version-test.os8   | Test the `begin enabled version` functionality
| 		     | in os8-run.
| copy-test.os8	     | Test the `cpto` and `cpfrom` os8-run commands.
| err-test.os8	     | Used to provoke errors in os8-run and test
| 		     | behavior.
| restart-test.os8   | Test the `restart` os8-run command.
| patch-test.os8     | Test the `patch` os8-run command.
| v3d-reset-tc08.os8 | Utility: re-configure the default OS/8 v3d
| 		     | rk05 boot image to use TC08 DECtape (instead
|		     | of TD8E) drivers.

[os8-run-doc]: /doc/trunk/doc/os8-run.md

## License

Copyright © 2016-2018 by Bill Cattey. This document is licensed under
the terms of [the SIMH license][sl].
