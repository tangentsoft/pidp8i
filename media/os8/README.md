# OS/8 Media


## Bootable Systems

`os8v3d-bin.rk05` — Bootable OS/8 system made from the binary DECtapes
of OS/8 V3D in the Willem van der Mark Archives described below plus
other tapes in the [`subsys` subdirectory](/file/media/os8/subsys)
during the PiDP-8/I software build process. This is the disk image used
by boot options IF=0 and IF=7. See the [top-level `README.md`
file][tlrm] for instructions on controlling what goes into this image.

`os8.tu56` — Bootable OS/8 DECtape image used by boot option IF=3.
Primarily intended to demonstrate the uncommon "boot and run from tape"
experience offered by early DEC systems.


## Data Disks

`os8v3d-src.rk05` — OS/8 Source RK05 pack made from the source DECtapes
of OS/8 V3D in the Willem van der Mark Archives, described below. This
is not a bootable OS/8 system. It is merely a convenience for use with a
bootable OS/8 system, so you can avoid mounting the seven "Source"
distribution tapes in succession.

If you give the following command to SIMH:

    att rk1 media/os8/os8v3d-src.rk05
   
...this disk will appear as RKA1: and RKB1: under OS/8.


## Willem van der Mark Archives

Willem van der Mark wrote a really cool [PDP-8 emulator in Java][vdms].
He has a well organized archive of DEC media.  The following DECtapes
were copied from his [OS8-V3D software archive][vdms].

The OS/8 Binary DECtapes were compared against those in [Dave Gesswein's
Archive][dga] and found to be nearly identical.  Of the three tapes
reviewed, a total of 2 words differed. Further comparison is possible as
a project for an interested person.

| DECtape Image File Name               | Content Description
| ----------------------------------------------------------------------------
| `al-4711c-ba-os8-v3d-1.1978.tu56`     | DEC OS/8 V3D **Binary** Distribution  1/2
| `al-4712c-ba-os8-v3d-2.1978.tu56`     | DEC OS/8 V3D **Binary** Distribution  2/2
| `al-4761c-ba-os8-v3d-ext.1978.tu56`   | DEC OS/8 V3D Extensions **Binary** Distribution  1/1
| `al-4549d-ba-fr4-v3d-1.1978.tu56`     | DEC OS/8 V3D FORTRAN IV **Binary** Distribution  1/2
| `al-5596d-ba-fr4-v3d-2.1978.tu56`     | DEC OS/8 V3D FORTRAN IV **Binary** Distribution  2/2
| `al-5642a-ba-macrel-linker.1978.tu56` | DEC OS/8 V3D MACREL/LINKER **Binary** Distribution 
| `al-4691c-sa-os8-v3d-1.1978.tu56`     | DEC OS/8 V3D **Source** Distribution  1/7
| `al-4692c-sa-os8-v3d-2.1978.tu56`     | DEC OS/8 V3D **Source** Distribution  2/7
| `al-4693d-sa-os8-v3d-3.1978.tu56`     | DEC OS/8 V3D **Source** Distribution  3/7
| `al-4694c-sa-os8-v3d-4.1978.tu56`     | DEC OS/8 V3D **Source** Distribution  4/7
| `al-4695c-sa-os8-v3d-5.1978.tu56`     | DEC OS/8 V3D **Source** Distribution  5/7
| `al-4696c-sa-os8-v3d-6.1978.tu56`     | DEC OS/8 V3D **Source** Distribution  6/7
| `al-4697c-sa-os8-v3d-7.1978.tu56`     | DEC OS/8 V3D **Source** Distribution  7/7
| `al-4759c-sa-os8-ext-1.1978.tu56`     | DEC OS/8 V3D Extensions **Source** Distribution  1/3
| `al-4760c-sa-os8-ext-2.1978.tu56`     | DEC OS/8 V3D Extensions **Source** Distribution  1/3
| `al-5586c-sa-os8-ext-3.1978.tu56`     | DEC OS/8 V3D Extensions **Source** Distribution  1/3


## Other Files

| File Name       | Content Description
| ----------------------------------------------------------------------------
| `LICENSE.md`    | License provided by DEC that makes our use of OS/8 legal
| `local.tu56`    | Files created for or vetted by the PiDP-8/I project; used at build time

[dga]:  http://www.pdp8online.com/images/images/misc_dectapes.shtml
[tlrm]: /doc/trunk/README.md
[vdms]: http://vandermark.ch/pdp8/index.php?n=OS8.OS8-V3D
[vdma]: http://vandermark.ch/pdp8/index.php