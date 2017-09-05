# Creators and Major Contributors to the PiDP-8/I Project

*   **Oscar Vermeulen <oscar.vermeulen@hotmail.com>**:

    -   Creator of the project (both hardware and software)
    
    -   Author of the initial modifications to the SIMH PDP-8 simulator
        necessary to make it use the PiDP-8/I front panel hardware

    -   Curator of the default set of binary demo media

    -   Author of the simulator setup scripts

    -   Initiator of much else in the project

    -   Author of the bulk of the documentation

    -   Host and major contributor to the PiDP-8/I support forum on
        Google Groups

    -   Hardware kit assembler and distributor

*   **Robert M Supnik** Primary author of the SIMH PDP-8 simulator upon
    which this project is based.

*   **Mike Barnes** Ported Oscar Vermeulen's SIMH 3.9 based PiDP-8/I
    simulator to the new SIMH 4.0 code base.  (September 2015.)

*   **Dylan McNamee** Ported the software to Buildroot for the official
    2015.12.15 binary OS images, and helped to merge the James L-W
    "alt-serial" mode in.

*   **Mark G. Thomas** Creator of the installation scripts for the
    2015.12.15 release, which were folded into the `make install`
    handler within `Makefile.in`. Also wrote the version of the SysV
    init script that came with that release as `rc.pidp8`, shipped here
    as `pidp8i-init`.

*   **Ian Schofield <isysxp@gmail.com>** Modified the LED lamp driving
    code in the simulator to better simulate the incandescent lamps in
    the original PDP-8/I hardware.  (The bulk of his original code has
    since been rewritten, but the core idea remains, and it is doubtful
    whether the current method would exist without his instigation.)

*   **Henk Gooijen <henk.gooijen@boschrexroth.nl>** Pushed the PDP-8
    simulator's internal EAE step counter value down into the PiDP-8/I's
    LED manipulation code, without which the step counter LEDs remain
    dark even when using the EAE.

*   **Paul R. Bernard <prb@downspout.ca>** wrote `src/test.c` and the
    core of what now appears as `doc/pidp8i-test.md`. (The program builds
    and installs as `pidp8i-test`.)  He also provided a one-line fix
    that completes the work of Henk Gooijen's step counter patch.

*   **Rick Murphy <k1mu.nospam@gmail.com>** optimized the `pep001.pal`
    example so that it fits into a single page of PDP-8 core, and
    provided several useful files in his OS/8 disk images that have
    managed to land in this software distribution's OS/8 disk image.

*   **Tony Hill <hill.anthony@gmail.com>** Merged all the upstream SIMH
    changes produced between late September 2015 and late December 2016
    into the PiDP-8/I simulator.  (Since then, this merge process is
    automated as `tools/simh-update`.)

*   **Bill Cattey <bill.cattey@gmail.com>** (a.k.a. poetnerd) did the
    bulk of the work on automatic generation of OS/8 system pack images
    from original source media, replacing the hand-assembled and -hacked
    `os8.rk05` image.

*   **Jonathan Trites <tritesnikov@gmail.com>** wrote the original
    version of what has become `tools/mk-os8-rk05s.in`, the script
    that generates `media/os8/*.rk05` from source media.

*   **Warren Young <tangentsoft@gmail.com>** Did everything listed in
    `ChangeLog.md` that is not attributed to anyone else.
