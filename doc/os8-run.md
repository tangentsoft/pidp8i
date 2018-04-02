# os8-run Script Runner for OS/8 under simh

After having done a lot of exploring of how to create system images,
the design of a generalized script runner for OS/8 under simh emerged.

## Overview

`os8-run` is a general script running facility that can:

* attach desired system images and boot them.
** scratch copies of read-only images can be created and booted.
** an existing image can be copied for use.
* copy files into the running OS/8 from the POSIX environment running SIMH.
* copy files from the running OS/8 environment to the POSIX environment running SIMH.
* run any OS/8 command as long as it returns immediately to command level, including BATCH scripts.
* run `ABSLDR` and `FOTP`, cycling an arbitrary number of times through the command decoder.
* run `PAL8` with either a 3 argument form that produces a listing file, or a 2 argument form that does not.
* run 'BUILD' with arbitrarily complex configuration scripts, including a `BUILD` of a system head that inputs `OS8.BN` and `CD.BN`.
* configure the `tti`, `rx`, `td`, and `dt` devices at run time to allow shifting between otherwise incompatible configurations of SIMH and OS/8 device drivers.
* include script files so that common code blocks can be written once in an external included script.
* run of patch scripts that will use `ODT` or `FUTIL` to patch the booted system image.
* perform actions in a script conditional on feature enablement matching an arbitrary keyword.
* perform actions in a scrupt unless a disablement keyword has been specified.
* set enable or disable keywords anywhere in the execution of a script.

## `mount`

The mount command is used to attach image files to SIMH devices.

`mount` _simh-dev_ _image-file_ [_option_ ...]
option: `must-exist` | `no-overwrite` | `read-only` | `copy_scratch`

### `mount` Options


| `must-exist`   | _image-file_ must exist, otherwise abort the attach.
| `no-overwrite` | if _image-file_ already exists, create a copy with a version number suffix.
|            | This is useful when you want to prevent overwrites of a good image file
|            | with changes that might not work.  `os8-run` steps through version seen
|            | until it can create a new version that doesn't overwrite any of the previous
|            | ones.
| `read-only`    | Passes the `-r` option to SIMH attach to mount the device in read only mode.
| `copy_scratch` | Create a writeable scratch version of the named image file and mount it.
|            | This is helpful when you are booting a distribution DECtape.
|            | Booted DECtape images must be writeable. To protect a distribution DECtape,
|            | use the `copy_scratch` option.  When the script is done the scratch version
|            | is deleted.
         

#TODOs:

* Create a `resume` command that does not reboot OS/8
* Allow underscore and dash in mount options.
* Make `mount` failure abort the script.



### <a id="license"></a>License

Copyright © 2017 by Bill Cattey. Licensed under the terms of
[the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[os8ext]: https://tangentsoft.com/pidp8i/doc/trunk/doc/os8-v3d-device-extensions.md

