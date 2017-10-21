# OS/8 System Patches

Between major updates to distribution media, DEC would send out important
information and patches to customers through its publication _PDP-8 Digital Software
News_ (DSN for short).

Many issues of DSN can be found on bitsavers.org under [pdf/dec/pdp8/softwarenews][dsn].

To help customers keep track of which patches to apply, DSN added a Cumulative Index.

Using the _PDP-8 DIGITAL Software News Cumulative Index_ found in the
latest available issue of DSN, [October/November 1980][dsn-1980-10], I
created a spreadsheet of all patches relevant to the OS/8 V3D packs
under construction.  That spreadsheet enabled me to go to the
particular issues containing the patches, and keep track of what
action I took with them.

I reviewed all the patches and came up with a list of the mandatory patches,
using OCR of the DSN issues, I created a file per patch.  Then I reviewed the
scanned PDF versions to correct OCR errors.

Then the mkos8 script was enhanced to apply the patches in an automated way.
Most of the patches were for programs available in source form.  So I built the
programs from source, and then bench checked the patch against the source.
In a few cases the code was too obscure, and I marked the patch as "plausable"
rather than "verified" in my spreadsheet.

I conclude that, as time went on, the Software Development group got progressively
less careful.

The file [patch_list.txt][patchlist] contains all the patches that I got
working along with some notes about:

* Which optional patches are recommended.
* What functionality is not recommended for patch enablement.
* What conflicts arise to dis-recommend a patch.

## Review of Recommendations:

`BRTS 31.11.2 O` Is an optional patch disables 8th bit parity. It is
recommended because sometimes we may want to allow output that does
not force the 8th bit.

`BRTS 31.11.3 O` Is an optional patch that enables 132 column
output. It is recommended because it is expected that wide column
output is desirable.

`TECO 31.20.1 O` Is an optional patch that permanently forces no case
flagging.  It is not recommended because we want to allow the option
of case flagging.

`TECO 31.20.2 O` Is an optional patch that turns off verbose
errors. It was for slow terminals and experienced users who didn't
want to wait to see the long error messages they already knew.  It is
not recommended because we expect a majority of users to be on high
speed terminals needing the verbose errors.

`TECO 31.20.3 O` Turns off a warning that you are using the Yank
command to completely overwrite a buffer full of text.  Issuing the
command a second time succeeds.  It was again to avoid experienced
users.  It is not recommended because we expect fewer advanced users
who would be annoyed by the protection.

`TECO 31.20.4 O` Implements rubout support specifically and uniquely
for the VT05 terminal in a way that breaks it for all other video
terminals.  It is not recommended because there are VERY few VT05
deployments that would use it.

Here is here the progressive lack of carefullness begins to bite:

`EDIT 21.17.4 M` Is supposedly a mandatory patch.  It fixes a problem
with line printer output through a specific parall interface card.
Unfortunately, the patch overwrites mandatory patch in 21.17.2 and is
NOT recommended.

`ABSLDR 21.29.1 M` Is a mandatory patch that enables ABSLDR to work
with Save Image files.  Normally ABSLDR only loads `BIN` format
files. The patch sequence number, `21.29` identifies the patch as
being for the OS/8 V3D version of ABSLDR.  But the patch changes
locations that are not used by ABSLDR.SV.  Furthermore, the patch says
it upgrades ABSLDR from version 6B to version 6C.

Version 6 of ABSLDR was part of the OS/8 V3D Device Extensions kit.  I
want to digress into a bit of history of this kit because it's
important to understanding OS/8 versions going forward.

## OS/8 V3D Device Extensions:

The OS/8 V3D Device Extensions kit (product QF026.) was created to
support the newest PDP-8 hardware:

* The KT8A Memory Management option which enables addressing by 4X
beyond the previous maximum of 32K to a whopping 128K of memory.
* RL01 disk supporting 4X the previous usual disk capacity, now up to
nearly 5 Meg.
* RX02 double-density floppy disks.

This distribution contained software updates:

* A version of `BUILD` the system builder that could run under BATCH.
The previous version would just hang.
* An update to the OS/8 system including Monitor version 3S.
* `ABSLDR` version 6A supports loading into memory fields  greater than 7.
* `PAL8` version 13A uses memory fields greater than 7.
* `CCL` version 7A memory command recognizes up to 128K words of memory.
* `PIP` version 14A works with new devices and Monitor.
* `RESORC` version 4A includes new devices.
* `BOOT` version 5A boots new devices.
* `RXCOPY` version 5A formats and copies single and double density floppies.
* `FUTIL` version 8A recognizes new core control block in extended memory.

Unfortunately the distribution DECtape for this kit, part number
AL-H525A-BA has not been found.  Furthermore the PDP-8 Software
Components Catalog July 1979 gives no part number for a Source DECtape
distribution of this kit.  There is an RK05 source distribution, part
number AN-H529A-SA.

This kit is desirable in both binary and source form.  But we don't
have it, and patches to ABSLDR that only work with this version are
mis-labeled as being for the non-Extension version.

The proffered patch for ABSLDR is mis-labeled. It applies to a
version of ABSLDR I cannot find, and for which I do not have source.
It's definitely not recommended.

Currently the patches apply in alphabetical order.
However, there may in future emerge patches that impose an order.

For example, if the ABSLDR patch actually did work, it needs the
`FUTIL 31.21.2 M` in order to patch into the ABSLDR overlay bits.
Side note: I am a bit skeptical of that FUTIL patch because, when I
load ABSLDR.SV into core with GET, the contents of memory showed by
ODT are DIFFERENT from those shown by FUTIL.  This may be because of
overlays that ODT does not understand, or it may be because the FUTIL
patch does not work.  I'd labeled the FUTIL patch "plausable", because
I didn't understand the nuance of `CD` modules embodied in the patch.

## Then there's `MACREL`:

MACREL was a late development, an attempt to replace PAL8 with a Macro
assembler capable of producing relocatable modules.  When MACREL first
came on the scene, several companies decided to port their next major
upgrade to MACREL from PAL8.  MACREL was so buggy that everybody
basically had to revert to PAL8 and back-port all the new code
originally intended for the new major upgrade.  This situation befell
ETOS Version 5.

We have a binary distribution DECtape of MACREL version 1, DEC part
number AL-5642A-BA.  We have no source distribution, so the patches
cannot be verified against source.  Furthermore, the version numbers
don't match, even though the memory contents do.

Version number mismatches sometimes do occur with patches. For Example,
`TECO 31.20.11 M` says that it upgrades TECO to version 5.06, but got
the bits wrong.  Instead of changing contents from 0771 to 0772,
it looked to change from 0772 to 0773.  772 octal is 506 decimal, and
the TECO version number is represented with a 12 bit number.  It's
called "5.06" but it's represented as 0772 octal, or 506 decimal.

I corrected the TECO patch, because the rest was correct.  Whoever
published the patch got the version number wrong, and nobody
complained.

However, with the MACREL V1 patches, I need to do more research before
I feel confident recommending application of the patches.  Ideally I'd
find binary and source distributions of MACREL V2.  That's DEC Part
numbers AL-5642B-BA for the binary DECtape and the 4 source DECtapes
have part numbers: AL-5643B-SA, AL-5644B-SA, AL-H602B-SA, AL-H602B-SA.
I dont hold out much hope for finding these tapes.

I don't know if the failed MACREL ports happened with pre-release
versions of MACREL or with V1. I have found MACREL, LINK, and KREF on
disk packs imaged from running systems, and the OVERDRV.MA overlay
source code on Dave Gesswein's site.  I plan to dig them out and offer
them as options.

For now, I am in the process of doing what validation I can of the
V1 MACREL/LINK patches.

## Unfinished business:

There remain the following patches that are still under development,
because they are not simple binary overlays on executables that could
be applied with simple scripts driving ODT or FUTIL.  Instead they are
either batch scripts, or are applied to source code that is rebuilt
either with an assembler or high level language  compiler.

`FORLIB 51.10.1 M` requires crafting a batch script that changes
DLOG.RA and then rebulds FORLIB.RL with the new component.

`LQP 21.49.1 M` patches a device driver `.BN` file, then using BUILD
to insert it into the system.  At the present time the OS/8 V3D packs
we build do not use the LPQ driver.  (We ran out of device ID space
and so we don't have anywhere to put an active LPQ driver.)

`OVRDRV 40.6.1` is a source level patch for the MACREL V1 OVRDRV.MA
file.  Applying `LINK 40.2.2 M` appears to be a prerequisite.
Work to validate this patch won't begin until the patches to LINK
have been deemed safe and recommended.


Below is the latest snapshot of the tracking spreadsheet:


|  **Component** | **Issue** | **Sequence** | Mon/Yr | **Notes** | Status |
|  ------ | ------ | ------ | ------ | ------ | ------ |
|  | **OS/8 V3D** |  |  |  |  |
|  HANDLER | CTRL/Z AND NULL | 01 0* | Oct 77 | Optional. Not going to apply. |  |
|  CREF | BUG WITH FIXTAB | 21.15.1 M | Apr/May 78 | CREF-21.15.1-v2B.patch8 Corrects bad patch | Applies |
|   | INPUT AND OUTPUT FILE SPECIFICATIONS | 21.15.2 M | Feb/Mar 80 | CREF-21.15.2-v2C.patch8 | Applies |
|  EDIT | EDIT PROBLEM WITH NO FORMFEED AT END OF THE INPUT FILE | 21.17.1 M | Mar 78<br/> | EDIT-21.17.1M-v12B.patch8 | Applies; Src verified |
|   | EDIT Q COMMAND AFTER L COMMAND | 21.17.2 M | Jun/Jul 79 | EDIT-21.17.2M-v12C.patch8 | Applies; Src verified |
|   | EDIT Q COMMAND PATCH | 21.17.3 M | Jun/Jul 79 | EDIT-21.17.3M-v12D.patch8 | Applies; Src verified |
|   | EDIT.SV "V" OPTION WILL NOT WORK WITH LPT DKC8-AA | 21.17.4 M | Feb/Mar 80 | EDIT-21.17.4M-v12C.patch8 (Dubious value) | Carelessly overwrote patch 21.12.2M |
|  FOTP | INCORRECT DIRECTORY VALIDATION | 21.19.1 M | Jun/Jul 79 | FOTP-21.19.1M-v9B.patch8 (Corrected from Aug/Sep 1978, Detailed in Apr/May 79) | Applies; Src verified |
|  MCPIP | DATE-78 PATCH FOR MCPIP | 21.21.1 M | Mar 78 | MCPIP-21.21.1M-v6B.patch8 | Applies; Src verified |
|  PAL8 | INCORRECT CORE SIZE ROUTINE | 21.22.1 M | Aug/Sep  78 | PAL8-21.22.1M-v10B.patch8 | Applies; Src verified |
|   | ERRONEOUS LINK GENERATION NOTED ON PAGE DIRECTIVE | 21.22.2 M | Aug/Sep  78 | PAL8-21.22.2M-v10C.patch8 | Applies; Src verified |
|   | EXPUNGE PATCH TO PAL8 | 21.22.3 M | Feb/Mar   80 | PAL8-21.22.3M-v10D.patch8 | Applies; src probable |
|   | TABS ARE TRANSLATED INCORRECTLY | 21.22.4 M | Oct/Nov 80 | PAL8-21.22.4M (Supercedes June/July 1980 (which had wrong contents of memory.)) | Needs FUTIL Support |
|  PIP | PIP /Y OPTION DOES NOT WORK PROPERLY WHEN TRANSFERRING A SYSTEM | 21.23-1 M | Aug/Sep 78 | PIP-21.23.1M-V12B.patch8 | Applies; Src prob. ok |
|  PIP10 | DATE '78 PATCH TO PIP 10 | 21.24.1 M | Jun/Jul 79 | PIP10-21.24.1M-V3B.patch8 (Corrected from Dec 78/Jan 79) | Applies; Src verified |
|  SET | USING SET WITH TWO-PAGE SYSTEM HANDLERS | 21.26.1 M | Apr/May 78 | SET-21.26.1M-v1C.patch8 | Applies; Src verified |
|   | SCOPE RUBOUTS FAIL IN SET | 21.26.2 M | Apr/May 78 | SET-21.26.2M-v1D.patch8 | Applies; Src verified |
|   | PARSING OF = IN TTY WIDTH OPTION | 21.26.3 M | Aug/Sep 78 | SET-21.26.3M-v1E.patch8 | Applies; Src verified |
|  LPQ | LDP01 HANDLER FAILS TO RECOGNIZE TABS | 21.49.1 M | Dec/Jan 1980 | LQP-21.49.1M-vB.patch8 (supercedes Mar 1978) | OCR* Comments \n |
|  TM8E | WRITE PROTECT PATCH TO TM8E.PA | 21.61.1 H | Feb/Mar 80 | New TM8E Source.  Too hard to correct. |  |
|  **OS/8 FORTRAN IV V3D** |  |  |  |  |  |
|  F4.SV V4A | EQUIVALENCE STATEMENT | 02 M / 21.1.2 M | Dec/Jan 1980 | F4-21.1.2M-v4B.patch8 (Note clarified Oct 77: F4 not FRTS, PASS3 too.) | Applies; src plausable |
|  **OS/8 EXTENSION KIT V3D** |  |  |  |  |  |
|  SABR | LINE BUFFER PROBLEM IN SABR | 21.91.1 M | Oct/Nov 79 | SABR-21.91.1M-v18B.patch8 | Applies; Src verified |
|  BASIC.UF | BASIC.UF INCOMPATIBLE FROM OS/8 V3C | 31.5.1 M | Aug/Sep 78 | BASIC.UF-31.5.1M-V5B.patch8 Source also in DSN. | Applies; Src verified |
|  BLOAD | BLOAD WILL NOT BUILD CCB PROPERLY | 31.10.1 M | Feb/Mar 80 | BLOAD-31.10.1M-v5B.patch8 | Applies; Src verified |
|  BRTS | IOTABLE OVERFLOW | 31.11.1 M | Mar 78 | BRTS-31.11.1-M-v5b.patch8 | Applies; Src verified |
|   | BASIC PNT FUMCTION | 31.11.2 O | Jun/Jul 78 | BRTS-31.11.2-O.patch8 (superceds/corrects Mar 1978) | Applies; Src verified |
|   | LINE SIZE ON OUTPUT OF BASIC | 31.11.3 O | Jun/Jul 78 | BRTS-31.11.3-O.patch8 | Applies; Src verified |
|   | PATCH TO CHANGE LINE PRINTER WIDTH | 31.11.4 F | Oct/Nov 79 | Optional change of width to 132 columns |  |
|   | PATCH TO BRTS FOR ADDRESSING LAB 8/E FUNCTIONS | 31.11.5 M | Oct/Nov 79 | BRTS-31.11.5-x.patch8 (BASIC.UF patch is a prerequisite.) | Applies; Src verified |
|  TECO | CHANGING THE DEFAULT EU VALUE for no case flagging | 31.20.1 0 | Mar 78 | TECO-31.20.01O.patch8 | Applies; Src verified; Not recommended |
|   | CHANGING THE DEFAULT EH VALUE for one line error printouts | 31.20.2 0 | Mar 78 | TECO-31.20.02O.patch8 | Applies; Src verified; Not recommended |
|   | REMOVING YANK PROTECTION | 31.20.3 0 | Mar 78 | TECO-31.20.03O.patch8 | Applies; Src verified; Not recommended |
|   | SCOPE SUPPORT FOR VT05 USERS | 31.20.4 0 | Mar 78 | TECO-31.20.04O.patch8 | Applies; src plausable Not Recommended |
|   | PROBLEM WITH AY COMMAND | 31.20.5 M | Mar 78 | TECO-31.20.05M-v5A.patch8 | Applies; Src verified |
|   | CONDITIONALS INSIDE ITERATIONS | 31.20.6 M | Mar 78 | TECO-31.20.06M-v5B.patch8 | Applies; Src verified |
|   | ECHOING OF WARNING BELLS | 31.20.7 M | Mar 78 | TECO-31.20.07M-v5B.patch8 | Applies; Src verified |
|   | CTRL/U SOMETIMES FAILS AFTER * | 31.20.8 M | Apr/May 78 | TECO-31.20.08M-v5.04.patch8 | Applies; Src prob. ok |
|   | MULTIPLYING BY 0 IN TECO | 31.20.10 M | Apr/May 78 | TECO-31.20.10M-v5.05.patch8 | Applies; Src verified |
|   | Q-REGISTERS DON'T WORK IN 8K | 31.20.11 M | Apr/May 78 | TECO-31.20.11M-v5.06.patch8 | Applies; Src verified |
|   | CAN'T SKIP OVER A "W | 31.20.12 M | Apr/May 78 | TECO-31.20.12M-v5.07.patch8 | Applies; Src verified |
|   | UNSPECIFIED ITERATIONS AFTER INSERTS | 31.20.13 M | Oct/Nov 78 | TECO-31.20.13M-v5.08.patch8 (Corrected from Jun/Jul 78) | Applies; Src verified |
|   | NEW FEATURES IN TECO V5 | 31.20.14 N | Aug/Sep 78 | Documentation Only |  |
|  FUTIL | FUTIL PATCH | 31.21.1 M | Apr/May 78 | FUTIL-31.21.1M-v7B.patch8 | Applies; src plausable |
|   | PATCH TO FIX 'SHOW CCB' AND MAPPING OF 'CD' MODULES | 31.21.2 M | Oct/Nov 78 | FUTIL-31.21.2M-v7D.patch8 (Corrected from Aug/Sep 78) | Applies; src plausable |
|   | Optional: change XS format from excess 240 to excess 237. Useful for viewing COS data files. | 31.21.3 0 | Aug/Sep 78 | FUTIL-31.21.3O.patch8 | Applies; Src verified; Not recommended |
|   | FUTIL PATCH TO MACREL/LINK OVERLAYS | 31.21.4 N | Jun/Jul 79 | Documentation Only |  |
|  MSBAT | DIM STATEMENT NOT WORKING IN MSBAT | 31.22.1 M | Dec 78/Jan 79 | MSBAT-31.22.1M-v3B.patch8 | Applies; Src verified |
|  BATCH | MANUAL INTERVENTION REQUIRED ERRONEOUSLY | 31.23.1 M | Aug/Sep 78 | BATCH-31.23.1M-v7B.patch8 | Applies; Src verified |
|   | **OS/8 FORTRAN IV V3D** |  |  |  |
|  F4 | FORTRAN COMPILER FAILS TO RECOGNIZE " AS AN ERROR | 51.3-1 M<br/> | Jun/Jul 78 | F4-51.3.1M-v4C.patch8 (Corrects March 1978) | Applies; src plausable |
|   | FORTRAN COMPILER NOT RECOGNIZING SYNTAX ERROR | 51.3.2 M | Jun/Jul 78 | F4-51.3.2M-v4x.patch8 | Applies; src plausable |
|   | FORTRAN RUNTIME SYSTEM 2-PAGE HANDLER | 51.3-3 0 | Oct/Nov 78 | FRTS-51.3.3-O.patch8 Needed for RL02. (Corrected from Aug/Sep 78) | Applies; build of FRTS a mystery |
|   | RESTRICTION WITH SUBSCRIPTED VARIABLES | 51.3-4 R | Aug/Sep 80 | Documentation: FIV FORTRAN IV will not allow subscripting to be used on both sides of an arithmetic expression. |  |
|  FORLIB | FORTRAN IV DLOG PATCH | 51.10.1 M | Feb/Mar 80 | FORLIB-51.10.1M.patch8 (apply to DLOG.RA) | OCR* Comments \c |
|   | **OS/8 MACREL/LINKER V1A** |  |  |  |
|  LINK | PATCH V1D TO LINK | 40.2.1 M | Apr/May 78 | LINK-40.2.1M-v1D.patch8 | OCR* Comments \n |
|   | PATCH VIE TO LINK | 40.2.2 M | Apr/May 78 | LINK-40.2.2M-v1E.patch8 | OCR* Comments \n |
|   | LINK CORRECTIONS | 40.2.3 M | Apr/May 78 | LINK-40.2.3M-v1F.patch8 | OCR* Comments \n |
|  MACREL | PATCH V1D TO MACREL | 40.5.1 M | Apr/May 78 | MACREL-40.5.1M-v1D.patch8 | Needs Verify |
|   | PATCH V1E TO MACREL | 40.5.2 M | Apr/May 78 | MACREL-40.5.2M-v1E.patch8 | Needs Verify |
|  OVRDRV | PATCH V1B TO OVRDRV.MA | 40.6.1 M | Apr/May 78 | OVRDRV-40.6.1M-v1B-8srccom | OCR matches scan |
|   | **OS/8 V3D DEVICE EXTENSIONS December 1978 |  |  |  |  |  |
|   | DO NOT USE THIS KIT WITHOUT FIRST CONSULTING APR/MAY 79 DSN |  |  |  |  |
|  FRTS | FRTS PATCH | 35.1.3  M | Apr/May 79 |  |  |
|  MONITOR | MONITOR V3S PATCH | 35.2.1     M | Apr/May 79 |  |  |
|  FUTIL | FUTIL UNDER BATCH PATCH | 35.13.1   M | Apr/May 79 |  |  |
|  PAL8 | EXPUNGE PATCH TO PAL8 | 35.14.1   M | Feb/Mar 80 | PAL8-35.14.1M-v13B.patch8 | Applies; src plausable |
|  ABSLDR | LOADER PROBLEM WITH SAVE IMAGE FILES | 21.29.1 M | Oct/Nov 80 | ABSLDR-21.29.1M-v6C.patch8 (Supercedes June/July 1980 (which didn't have old contents of memory.)) | Wrong. v6B was with OS/8 Device Extensions. |
|  ABSLDR | ABSLDR PATCH | 35.18.1   M | Apr/May 79 |  |  |
|  BLOAD | BLOAD WILL NOT BUILD CCB PROPERLY | 35.51.1   M | Feb/Mar 80 | BLOAD-35.51.1M-v5C.patch8<br/>Use BLOAD-31 epoch instead. | OCR* Comments \n |
|   | **OS/8 MACREL/LINKER V2A<br/>Not doing these patches lacking a distribution.** |  |  |  |
|   | EXPUNGE DOCUMENTATION ERROR | 41.1.1 N | Jun/Jul 79 |  |  |
|   | MACREL VERSION NUMBERS | 41.1.2 N | Jun/Jul 79 |  |  |
|   | MACRO RESTRICTION IN MACREL | 41.1.3 N | Aug/Sep 79 |  |  |
|   | ERROR IN .MCALL MACRO EXAMPLE | 41.1.4 N | Feb/Mar 80 |  |  |
|  KREF | PATCH TO CORRECT PRINTING OF NUMERIC LOCAL SYMBOLS | 41.3.1 M | Apr/May 80 |  |  |
|  MACREL | EXPUNGE PATCH TO MACREL | 41.4.1 F | Jun/Jul 79 |  |  |
|   | INCONSISTENCIES IN MACREL ERROR REPORTING | 41.4.2 N | Aug/Sep 79 |  |  |
|   | FORWARD REFERENCE PATCH TO MACREL | 41.4.3 M | Aug/Sep 79 |  |  |
|   | PATCH TO CORRECT MACRO SUBSTRING PROBLEM | 41.4.4 M | Apr/May 80 |  |  |
|   | PATCH TO CORRECT PRINTING OF NUMERIC LOCAL SYMBOLS | 41.4.5 M | Apr/May 80 |  |  |
|  OVRDRV | PATCH TO OVRDRV TO CORRECT CDF PROBLEM | 41.5.1 M | Dec/Jan 80 |  |  |

[patchlist]: https://tangentsoft.com/pidp8i/doc/trunk/media/os8/patches/patch_list.txt

### <a id="license"></a>License

Copyright © 2017 by Bill Cattey. Licensed under the terms of [the SIMH
license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[dsn]: http://bitsavers.org/pdf/dec/pdp8/softwarenews/
[dsn-1980-10]: http://bitsavers.org/pdf/dec/pdp8/softwarenews/198010_PDP8swNews_AA-K629A-BA.pdf