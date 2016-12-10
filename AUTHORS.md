# Creators and Major Contributors to the PiDP-8/I Project

*   **Oscar Vermeulen <oscar.vermeulen@hotmail.com>**:

    -   Creator of the project (both hardware and software)
    
    -   Author of the modifications to the SimH PDP-8 simulator
        necessary to make it use the PiDP-8/I front panel hardware
        
    -   Curator of the default set of binary demo media
    
    -   Author of the simulator setup scripts
    
    -   Initiator of much else in the project
    
    -   Author of the bulk of the documentation
    
    -   Host and major contributor to the PiDP-8/I support forum on
        Google Groups
        
    -   Hardware kit assembler and distributor

*   **Robert M Supnik** Primary author of the SimH PDP-8 simulator upon
    which this project is based.

*   **Mike Barnes** Ported Oscar Vermeulen's SimH 3.9 based PiDP-8/I
    simulator to the new SimH 4.0 code base.

*   **Dylan McNamee** Creator of the "buildroot" feature used in the
    creation of the official 2015.12.15 release versions.

*   **Mark G. Thomas** Creator of the installation scripts for the
    2015.12.15 release, which were folded into the `make install`
    handler within `Makefile.in`.

*   **Ian Schofield <isysxp@gmail.com>** Modified the LED lamp driving
    code in the simulator to better simulate the incandescent lamps in
    the original PDP-8/I hardware.

*   **Paul R. Bernard <prb@downspout.ca>** wrote `src/test.c`, which
    builds and installs as `pidp8i-test`. He also wrote the core of what
    now appears in `README-test.md`.

*   **Rick Murphy <k1mu.nospam@gmail.com>** optimized `examples/pep001.pal`
    so that it fits into a single page of PDP-8 core.

*   **Warren Young <tangentsoft@gmail.com>** Did everything listed in
    `ChangeLog.md` that is not attributed to anyone else.
