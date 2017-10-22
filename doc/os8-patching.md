# OS/8 System Patches

Between major updates to distribution media, DEC would send out important
information and patches to customers through its publication _PDP-8 Digital Software
News_ (_DSN_ for short).

Many issues of _DSN_ can be found on bitsavers.org under [pdf/dec/pdp8/softwarenews][dsn].

To help customers keep track of which patches to apply, _DSN_ added a Cumulative Index.

Using the _PDP-8 DIGITAL Software News Cumulative Index_ found in the
latest available issue of _DSN_, [October/November 1980][dsn-1980-10], I
created a spreadsheet of all patches relevant to the OS/8 V3D packs
under construction.  That spreadsheet enabled me to go to the
particular issues containing the patches, and keep track of what
action I took with them.

I reviewed all the patches and came up with a list of the mandatory patches,
using OCR of the _DSN_ issues, I created a file per patch.  Then I reviewed the
scanned PDF versions to correct OCR errors.

Then our `mkos8` script was enhanced to apply the patches in an automated way.
Most of the patches were for programs available in source form.  So I built the
programs from source, and then bench checked the patch against the source.
In a few cases the code was too obscure, and I marked the patch as "plausable"
rather than "verified" in my spreadsheet.

The file [patch_list.txt][patchlist] contains all the patches that I got
working along with some notes about:

* Which optional patches are recommended.
* What functionality is not recommended for patch enablement.
* What conflicts arise to dis-recommend a patch.

## Review of Recommendations:

`BRTS 31.11.2O` Is an optional patch disables 8th bit parity. It is
recommended because sometimes we may want to allow output that does
not force the 8th bit.

`BRTS 31.11.3O` Is an optional patch that enables 132 column
output. It is recommended because it is expected that wide column
output is desirable.

`TECO 31.20.1O` Is an optional patch that permanently forces no case
flagging.  It is not recommended because we want to allow the option
of case flagging.

`TECO 31.20.2O` Is an optional patch that turns off verbose
errors. It was for slow terminals and experienced users who didn't
want to wait to see the long error messages they already knew.  It is
not recommended because we expect a majority of users to be on high
speed terminals needing the verbose errors.

`TECO 31.20.3O` Turns off a warning that you are using the `YANK`
command to completely overwrite a buffer full of text.  Issuing the
command a second time succeeds.  It was again to avoid experienced
users.  It is not recommended because we expect fewer advanced users
who would be annoyed by the protection.

`TECO 31.20.4O` Implements rubout support specifically and uniquely
for the `VT05` terminal in a way that breaks it for all other video
terminals.  It is not recommended because there are VERY few `VT05`
deployments that would use it.

It is at this point that I began to notice that in later years,
patches became less carefully produced, and more prone to errors. Some
not correctable, even today.

`BASIC.UF-31.5.1M` shows:

    4044/4514 4556

changing location `4044` from `4514` to `4556`.
Such a change would be consistent with the stated purpose of the patch,
to correct references to page zero literals that moved with the `V3D`
version of `BRTS`.  The source around location '4044' looks like this:

    04043  4775          JMS I   (BUFCDF /SET UP USER BUF
    04044  1273          TAD     NSAM
    04045  7041          CIA
    04046  3276          DCA     NCTR    /-#OF BOARDS TO CLAR


In my judgment the `TAD NSAM` to get the subscript into the `AC`
should be retained, and the `4556` call to `UNSFIX` to truncate the
value of the Floating Point Accumulator should NOT be inserted.  I
modified the patch to leave out that change.  It remains to be seen if
calls to User Functions in OS/8 `BASIC` will ever be run to test this
code.  Here at least is an analysis to later explorers.

`EDIT 21.17.4 M` Is supposedly a mandatory patch.  It fixes a problem
with line printer output through a specific parall interface card.
Unfortunately, the patch overwrites mandatory patch in 21.17.2 and is
NOT recommended.

`ABSLDR 21.29.1 M` Is a mandatory patch that enables `ABSLDR` to work
with `SAVE` image files.  Normally `ABSLDR` only loads `BIN` format
files. The patch sequence number, `21.29` identifies the patch as
being for the OS/8 V3D version of `ABSLDR`.  But the patch changes
locations that are not used by `ABSLDR.SV`.  Furthermore, the patch says
it upgrades `ABSLDR` from version `6B` to version `6C`.

Version 6 of `ABSLDR` was part of the OS/8 V3D Device Extensions kit.  I
want to digress into a bit of history of this kit because it's
important to understanding OS/8 versions going forward.

## OS/8 V3D Device Extensions:

The OS/8 V3D Device Extensions kit (product `QF026`.) was created to
support the newest PDP-8 hardware:

* The `KT8A` Memory Management option which enables addressing by a
factor of four beyond the previous maximum of 32K to a whopping 128K
of memory.
* RL01 disk supporting 4 times the previous usual disk capacity, now
up to nearly 5 Meg.
* RX02 double-density floppy disks.

This distribution contained software updates:

* A version of `BUILD` the system builder that could run under `BATCH`.
The previous version would just hang.
* An update to the OS/8 system including `Monitor` version `3S`.
* `ABSLDR` version `6A` supports loading into memory fields  greater than 7.
* `PAL8` version `13A` uses memory fields greater than 7.
* `CCL` version `7A` memory command recognizes up to 128K words of memory.
* `PIP` version `14A` works with new devices and Monitor.
* `RESORC` version `4A` includes new devices.
* `BOOT` version `5A` boots new devices.
* `RXCOPY` version `5A` formats and copies single and double density floppies.
* `FUTIL` version `8A` recognizes new core control block in extended memory.

When reference is made to `PAL8` version 13, that version originally came
from this kit.

Unfortunately the distribution DECtape for this kit, part number
AL-H525A-BA has not been found.  Furthermore the PDP-8 Software
Components Catalog July 1979 gives no part number for a Source DECtape
distribution of this kit.  There is an RK05 source distribution, part
number AN-H529A-SA.

This kit is desirable in both binary and source form.  But we don't
have it, and patches to `ABSLDR` that only work with this version are
mis-labeled as being for the non-Extension version.

The proffered patch for `ABSLDR` is mis-labeled. It applies to a
version of `ABSLDR` I cannot find, and for which I do not have source.
It's definitely not recommended.

## Patch Application Order:

The `patch` routine in `mkos8` applies the patches in the order they
appear in `patch_list.txt` That list is currently in alphabetical
order.  However, there may in future emerge patches that impose an
order.

For example, if the `ABSLDR` patch actually did work, it needs the
`FUTIL 31.21.2 M` in order to patch into the `ABSLDR` overlay bits.
Side note: I am a bit skeptical of that `FUTIL` patch because, when I
load `ABSLDR.SV` into core with GET, the contents of memory showed by
`ODT` are *DIFFERENT* from those shown by `FUTIL`.  This may be because of
overlays that `ODT` does not understand, or it may be because the `FUTIL`
patch does not work.  I'd labeled the `FUTIL` patch "plausable", because
I didn't understand the nuance of `CD` modules embodied in the patch.

## Then there's `MACREL`:

`MACREL` was a late development, an attempt to replace `PAL8` with a
Macro assembler capable of producing relocatable modules.  When
`MACREL` first came on the scene, several companies decided to port
their next major upgrade to `MACREL` from `PAL8`.  `MACREL` was so
buggy that everybody basically had to revert to `PAL8` and back-port
all the new code originally intended for the new major upgrade.  This
situation befell `ETOS` Version 5.

We have a binary distribution DECtape of `MACREL` version 1, DEC part
number `AL-5642A-BA`.  We have no source distribution, so the patches
cannot be verified against source.  Furthermore, the version numbers
don't match, even though the memory contents do.

Version number mismatches sometimes do occur with patches. For
Example, `TECO 31.20.11 M` says that it upgrades `TECO` to version
`5.06`, but got the bits wrong.  Instead of changing contents from
`0771` to `0772`, it looked to change from `0772` to `0773`.  `772`
octal is `506` decimal, and the `TECO` version number is represented
with a 12 bit number.  It's called "5.06" but it's represented as
`0772` octal, or `506` decimal.

I simply changed the version amendment line in that `TECO` patch,
because the rest was correct.  Whoever published the patch got the
version number wrong, and nobody complained.

However, with the `MACREL V1` patches, I need to do more research
before I feel confident recommending application of the patches.
Ideally I'd find binary and source distributions of `MACREL V2`.
That's DEC Part number `AL-5642B-BA` for the binary DECtape and the 4
source DECtapes have part numbers: `AL-5643B-SA`, `AL-5644B-SA`,
`AL-H602B-SA`, and `AL-H602B-SA`.  I dont hold out much hope for
finding these tapes.

I don't know if the failed `MACREL` ports happened with pre-release
versions of `MACREL` or with `V1`. I have found `V2` `MACREL`, `LINK`,
and `KREF` on disk packs imaged from running systems, and the `V2`
`OVERDRV.MA` overlay source code on Dave Gesswein's site.  I plan to
dig them out and offer them as options.

For now, I am in the process of doing what validation I can of the
`V1 MACREL`/`LINK` patches.

## Unfinished business:

There remain the following patches that are still under development,
because they are not simple binary overlays on executables that could
be applied with simple scripts driving `ODT` or `FUTIL`.  Instead they
are either batch scripts, or are applied to source code that is
rebuilt either with an assembler or high level language compiler.

`FORLIB 51.10.1 M` requires crafting a batch script that changes
`DLOG.RA` and then rebulds `FORLIB.RL` with the updated component.

`LQP 21.49.1 M` patches a device driver `.BN` file, then using `BUILD`
to insert it into the system.  At the present time the OS/8 V3D packs
we build do not use the `LPQ` driver.  (We ran out of device ID space
and so we don't have anywhere to put an active `LPQ` driver.)

`OVRDRV 40.6.1` is a source level patch for the `MACREL V1 OVRDRV.MA`
file.  Applying `LINK 40.2.2 M` appears to be a prerequisite.
Work to validate this patch won't begin until the patches to LINK
have been deemed safe and recommended.

## The Tracking Spreadsheet:

Below is the latest snapshot of the tracking spreadsheet.

| A | Patch Applies Successfully                                   |
| V | Patch Source Verified                                        |
| K | Patch Source Probably Ok. Weaker confidence than "Verified". |
| P | Patch Source Plausible . Weaker confidence than "Plausible". |
| N | Not recommended                                              |
| O | OCR Cleaned up. No other verification or application done.   |
| D | Does not apply.                                              |

For better readability of the table, status is abbreviated with combination of the above keys.

|  **Component** | **Issue** | **Sequence** | Mon/Yr | **Notes** | Status |
| ------ | ------ | ------ | ------ | ------ | ------ |
|  | **OS/8 V3D** |  |  |  |  |
|  `HANDLER` | `CTRL/Z` and `NULL` | `01 O *` | Oct-77 | Optional. Not going to apply. |  |
|  `CREF` | Bug with `FIXTAB` | `21.15.1M` | Apr/May-78 | `CREF-21.15.1-v2B.patch8` Corrects bad patch | A |
|   | Input and output file specifications | `21.15.2M` | Feb/Mar-80 | `CREF-21.15.2-v2C.patch8` | A |
|  `EDIT` | `EDIT` Problem with no `FORMFEED` at end of the input file | `21.17.1M` | Mar-78 | `EDIT-21.17.1M-v12B.patch8` | AV |
|   | `EDIT` `Q` command after `L` command | `21.17.2M` | Jun/Jul-79 | `EDIT-21.17.2M-v12C.patch8` | AV |
|   | `EDIT` `Q` command patch | `21.17.3M` | Jun/Jul-79 | `EDIT-21.17.3M-v12D.patch8` | AV |
|   | `EDIT.SV` `V` option will not work with `LPT DKC8-AA` | `21.17.4M` | Feb/Mar-80 | `EDIT-21.17.4M-v12C.patch8` Overwrites patch `21.12.2M` | AVN  |
|  `FOTP` | Incorrect directory validation | `21.19.1M` | Jun/Jul-79 | `FOTP-21.19.1M-v9B.patch8` (Corrected from Aug/Sep 1978, Detailed in Apr/May 79) | AV |
|  `MCPIP` | `DATE-78` Patch for `MCPIP` | `21.21.1M` | Mar-78 | `MCPIP-21.21.1M-v6B.patch8` | AV |
|  `PAL8` | Incorrect core size routine | `21.22.1M` | Aug/Sep-78 | `PAL8-21.22.1M-v10B.patch8` | AV |
|   | Erroneous `LINK` generation noted on `PAGE` directive | `21.22.2M` | Aug/Sep-78 | `PAL8-21.22.2M-v10C.patch8` | AV |
|   | `EXPUNGE` patch to `PAL8` | `21.22.3M` | Feb/Mar-80 | `PAL8-21.22.3M-v10D.patch8` | AK |
|   | `TAB`s are translated incorrectly | `21.22.4M` | Oct/Nov-80 | `PAL8-21.22.4M` (Supercedes June/July 1980 (which had wrong contents of memory.)) | AV |
|  `PIP` | `PIP` `/Y` option does not work properly when transferring a system | `21.23-1M` | Aug/Sep-78 | `PIP-21.23.1M-V12B.patch8` | AK |
|  `PIP10` | `DATE-78` Patch to `PIP 10` | `21.24.1M` | Jun/Jul-79 | `PIP10-21.24.1M-V3B.patch8` (Corrected from Dec 78/Jan 79) | AV |
|  `SET` | Using `SET` with two-page system handlers | `21.26.1M` | Apr/May-78 | `SET-21.26.1M-v1C.patch8` | AV |
|   | `SCOPE` `RUBOUT`s fail in `SET` | `21.26.2M` | Apr/May-78 | `SET-21.26.2M-v1D.patch8` | AV |
|   | Parsing of `=` in `TTY WIDTH` option | `21.26.3M` | Aug/Sep-78 | `SET-21.26.3M-v1E.patch8` | AV |
|  `LPQ` | `LDP01` Handler fails to recognize `TAB`s | `21.49.1M` | Dec/Jan-80 | `LQP-21.49.1M-vB.patch8` (supercedes Mar 1978) | O |
|  `TM8E` | Write protect patch to `TM8E.PA` | `21.61.1H` | Feb/Mar-80 | New `TM8E` Source.  Too hard to correct. |  |
| ------ | ------ | ------ | ------ | ------ | ------ |
|   | **OS/8 EXTENSION KIT V3D** |  |  |  |  |
|  `SABR` | Line buffer problem in `SABR` | `21.91.1M` | Oct/Nov-79 | `SABR-21.91.1M-v18B.patch8` | AV |
|  `BASIC.UF` | `BASIC.UF` Incompatible from OS/8 V3C | `31.5.1M` | Aug/Sep-78 | `BASIC.UF-31.5.1M-V5B.patch8` Source also in _DSN_. | AV |
|  `BLOAD` | `BLOAD` Will not build `CCB` properly | `31.10.1M` | Feb/Mar-80 | `BLOAD-31.10.1M-v5B.patch8` | AV |
|  `BRTS` | `IOTABLE` Overflow | `31.11.1M` | Mar-78 | `BRTS-31.11.1-M-v5b.patch8` | AV |
|   | `BASIC` `PNT` Function | `31.11.2O` | Jun/Jul-78 | `BRTS-31.11.2-O.patch8` (superceds/corrects Mar 1978) | AV |
|   | Line size on output of `BASIC` | `31.11.3O` | Jun/Jul-78 | `BRTS-31.11.3-O.patch8` | AV |
|   | Change line printer width | `31.11.4F` | Oct/Nov-79 | Optional change of width to 132 columns |  |
|   | Patch to `BRTS` for addressing `LAB 8/E` functions | `31.11.5M` | Oct/Nov-79 | `BRTS-31.11.5-x.patch8` (`BASIC.UF` patch is a prerequisite.) | AV |
|  `TECO` | Changing the default `EU` value for no `case` flagging | `31.20.1O` | Mar-78 | `TECO-31.20.01O.patch8` | AVN |
|   | Changing the default `EH` value for one line error printouts | `31.20.2O` | Mar-78 | `TECO-31.20.02O.patch8` | AVN |
|   | Removing `YANK` protection | `31.20.3O` | Mar-78 | `TECO-31.20.03O.patch8` | AVN |
|   | `SCOPE` Support for `VT05` users | `31.20.4O` | Mar-78 | `TECO-31.20.04O.patch8` | AP N |
|   | Problem with `AY` command | `31.20.5M` | Mar-78 | `TECO-31.20.05M-v5A.patch8` | AV |
|   | Conditionals inside iterations | `31.20.6M` | Mar-78 | `TECO-31.20.06M-v5B.patch8` | AV |
|   | Echoing of warning bells | `31.20.7M` | Mar-78 | `TECO-31.20.07M-v5B.patch8` | AV |
|   | `CTRL/U` Sometimes fails after `*` | `31.20.8M` | Apr/May-78 | `TECO-31.20.08M-v5.04.patch8` | AK |
|   | Multiplying by `0` in `TECO` | `31.20.10M` | Apr/May-78 | `TECO-31.20.10M-v5.05.patch8` | AV |
|   | `Q` registers don't work in 8K | `31.20.11M` | Apr/May-78 | `TECO-31.20.11M-v5.06.patch8` | AV |
|   | Can't skip over `W` | `31.20.12M` | Apr/May-78 | `TECO-31.20.12M-v5.07.patch8` | AV |
|   | Unspecified iterations after inserts | `31.20.13M` | Oct/Nov-78 | `TECO-31.20.13M-v5.08.patch8` (Corrected from Jun/Jul 78) | AV |
|   | New features in `TECO V5` | `31.20.14` N | Aug/Sep-78 | Documentation Only |  |
|  `FUTIL` | `FUTIL` Patch | `31.21.1M` | Apr/May-78 | `FUTIL-31.21.1M-v7B.patch8` | AP |
|   | Fix `SHOW CCB` and mapping of `CD` modules | `31.21.2M` | Oct/Nov-78 | `FUTIL-31.21.2M-v7D.patch8` (Corrected from Aug/Sep 78) | AP |
|   | Optional: change `XS` format from `excess-240` to `excess-237`. Useful for viewing `COS` data files. | `31.21.3O` | Aug/Sep-78 | `FUTIL-31.21.3O.patch8` | AVN |
|   | `FUTIL` Patch to `MACREL`/`LINK` overlays | `31.21.4 N` | Jun/Jul-79 | Documentation Only |  |
|  `MSBAT` | `DIM` Statement not working in `MSBAT` | `31.22.1M` | Dec 78/Jan-79 | `MSBAT-31.22.1M-v3B.patch8` | AV |
|  `BATCH` | `MANUAL INTERVENTION REQUIRED` Erroneously | `31.23.1M` | Aug/Sep-78 | `BATCH-31.23.1M-v7B.patch8` | AV |
| ------ | ------ | ------ | ------ | ------ | ------ |
|   | **OS/8 FORTRAN IV V3D** |  |  |  |
|  `F4` | `EQUIVALENCE` Statement | `02M` / `21.1.2M` | Dec/Jan-80 | `F4-21.1.2M-v4B.patch8` (Revised, Oct 77: `F4` and `PASS3` not `FRTS` patched.) | AP |
|  `F4` | `FORTRAN` Compiler fails to recognize `"` as an error | `51.3-1M` | Jun/Jul-78 | `F4-51.3.1M-v4C.patch8` (Corrects March 1978) | AP |
|   | `FORTRAN` Compiler not recognizing syntax error | `51.3.2M` | Jun/Jul-78 | `F4-51.3.2M-v4x.patch8` | AP |
|   | `FORTRAN` runtime system 2-page handler | `51.3-3O` | Oct/Nov-78 | `FRTS-51.3.3-O.patch8` Needed for RL02. (Corrected from Aug/Sep 78) | A |
|   | Restriction with subscripted variables | `51.3-4R` | Aug/Sep-80 | Documentation: `FIV` `FORTRAN IV` will not allow subscripting to be used on both sides of an arithmetic expression. |  |
|  `FORLIB` | `FORTRAN IV` `DLOG` Patch | `51.10.1M` | Feb/Mar-80 | `FORLIB-51.10.1M.patch8` (apply to `DLOG.RA`) | O |
|   | **OS/8 MACREL/LINKER V1A** |  |  |  |
|  `LINK` | Patch `V1D` to `LINK` | `40.2.1M` | Apr/May-78 | `LINK-40.2.1M-v1D.patch8` | O |
|   | Patch `VIE` to `LINK` | `40.2.2M` | Apr/May-78 | `LINK-40.2.2M-v1E.patch8` | O |
|   | `LINK` Corrections | `40.2.3M` | Apr/May-78 | `LINK-40.2.3M-v1F.patch8` | O |
|  `MACREL` | Patch `V1D` to `MACREL` | `40.5.1M` | Apr/May-78 | `MACREL-40.5.1M-v1D.patch8` | OD |
|   | Patch `V1E` to `MACREL` | `40.5.2M` | Apr/May-78 | `MACREL-40.5.2M-v1E.patch8` | OD |
|  `OVRDRV` | Patch `V1B` to `OVRDRV.MA` | `40.6.1M` | Apr/May-78 | `OVRDRV-40.6.1M-v1B-8srccom` | O |
| ------ | ------ | ------ | ------ | ------ | ------ |
|   | **OS/8 V3D DEVICE EXTENSIONS December 1978** |  |  |  DO NOT USE THIS KIT WITHOUT FIRST CONSULTING APR/MAY 79 _DSN_ |  |
|  `FRTS` | `FRTS` Patch | `35.1.3M` | Apr/May-79 |  |  |
|  `MONITOR` | `MONITOR` `V3S` Patch | `35.2.1M` | Apr/May-79 |  |  |
|  `FUTIL` | `FUTIL Under `BATCH` | `35.13.1M` | Apr/May-79 |  |  |
|  `PAL8` | `EXPUNGE` Patch to `PAL8` | `35.14.1M` | Feb/Mar-80 | `PAL8-35.14.1M-v13B.patch8` | AN |
|  `ABSLDR` | Loader problem with `SAVE` image files | `21.29.1M` | Oct/Nov-80 | `ABSLDR-21.29.1M-v6C.patch8` (Supercedes June/July 1980) Bad: v6B was with OS/8 Device Extensions. | ON |
|  `ABSLDR` | `ABSLDR` Patch | `35.18.1M` | Apr/May-79 |  |  |
|  `BLOAD` | `BLOAD` Will not build `CCB` properly | `35.51.1M` | Feb/Mar-80 | `BLOAD-35.51.1M-v5C.patch8` | ON |
|   | **OS/8 MACREL/LINKER V2A |  |  |  |
|   | `EXPUNGE` Documentation error | `41.1.1N` | Jun/Jul-79 |  |  |
|   | `MACREL` Version numbers: `MACREL` is `V2C` not `V2D`; `LINK` is `V2A` not `V2B`. | `41.1.2N` | Jun/Jul-79 |  |  |
|   | Macro restriction in `MACREL` | `41.1.3N` | Aug/Sep-79 |  |  |
|   | Error in `.MCALL` macro example | `41.1.4N` | Feb/Mar-80 |  |  |
|  `KREF` | Correct printing of numeric local symbols | `41.3.1M` | Apr/May-80 |  |  |
|  `MACREL` | `EXPUNGE` Patch to `MACREL` | `41.4.1F` | Jun/Jul-79 |  |  |
|   | Inconsistencies in `MACREL` error reporting | `41.4.2N` | Aug/Sep-79 |  |  |
|   | Forward reference patch to `MACREL` | `41.4.3M` | Aug/Sep-79 |  |  |
|   | Correct macro substring problem | `41.4.4M` | Apr/May-80 |  |  |
|   | Correct printing of numeric local symbols | `41.4.5M` | Apr/May-80 |  |  |
|  `OVRDRV` | Correct `CDF` problem | `41.5.1M` | Dec/Jan-80 |  |  |

[patchlist]: https://tangentsoft.com/pidp8i/doc/trunk/media/os8/patches/patch_list.txt

### <a id="license"></a>License

Copyright © 2017 by Bill Cattey. Licensed under the terms of [the SIMH
license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[dsn]: http://bitsavers.org/pdf/dec/pdp8/softwarenews/
[dsn-1980-10]: http://bitsavers.org/pdf/dec/pdp8/softwarenews/198010_PDP8swNews_AA-K629A-BA.pdf