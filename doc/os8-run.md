# `os8-run` Script Runner for OS/8 under simh

After having done a lot of exploring of how to create system images,
the design of a generalized script runner for OS/8 under simh emerged.

## Overview

`os8-run` is a general script running facility that can:

* attach desired system images and boot them.
    * scratch copies of read-only images can be created and booted.
    * an existing image can be copied for use.
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
* perform actions in a script unless a disablement keyword has been specified.
* set enable or disable keywords anywhere in the execution of a script.

Under the covers, run-os8 is a python script that uses the python expect
library that is capable of handling complex dialogs with commands.

However, sometimes debugging these scripts is challenging because if you
fall out of step with what is `expect`ed, the expect engine will get confused.
The os8-run command hangs for a while and then times out with a big python backtrace.

calling `os8-run` with `-v` gives verbose output that enables you to watch
every step of the script running.  Unfortunately, the output often echos characters
twice in a somewhat confusing cacophony.

## `done` -- Script is done.

This is an explicit statement to end processing of our script.

* All temporary files are deleted.
* All attached SIMH image files are gracefully detached with any pending writes completed.
* SIMH is gracefully shut down with a `quit` command.

## `include` -- Execute a subordinate script file.

`include` _script-file-path_

## `mount` -- Mount an image file as a SIMH attached device.

`mount` _simh-dev_ _image-file_ [_option_ ...]

### `mount` Options

| `must-exist`   | _image-file_ must exist, otherwise abort the attach.
| `no-overwrite` | if _image-file_ already exists, create a copy with a version number suffix.
|                | This is useful when you want to prevent overwrites of a good image file
|                | with changes that might not work.  `os8-run` steps through version seen
|                | until it can create a new version that doesn't overwrite any of the previous
|                | ones.
| `read-only`    | Passes the `-r` option to SIMH attach to mount the device in read only mode.
| `copy_scratch` | Create a writeable scratch version of the named image file and mount it.
|                | This is helpful when you are booting a distribution DECtape.
|                | Booted DECtape images must be writeable. To protect a distribution DECtape,
|                | use the `copy_scratch` option.  When the script is done the scratch version
|                | is deleted.
         
### Examples

Mount the `os8v3d-patched.rk05` image, which must exist, on SIMH `rk0`.

    mount rk0 ./os8v3d-patched.rk05 must-exist

Mount the `advent.tu56` image, which must exist, on SIMH `dt1` which will protect
it from inadvertent modification.

    mount dt1 ../media/os8/subsys/advent.tu56 read-only must-exist

Create a writeable copy of the distribution DECtape, `al-4711c-ba-os8-v3d-1.1978.tu56`,
which must exist.  Mount it on SIMH dt0 ready for for a read/write boot.  Delete the copy
when the script is done.

     mount dt0 ../media/os8/al-4711c-ba-os8-v3d-1.1978.tu56 must-exist copy_scratch

Create a new image file `system.tu56`.  If the file already exists, create a new version.
If the numbered version file exists, keep incrementing the version number for the new file
until a pre-existing file is not found.

For example, if `system.tu56` was not found, the new file would be called `system.tu56`.
If it was found the next version would be called `system_1.tu56`.  If `system_1.tu56`
and `system_2.tu56` were found the new file would be called `system_3.tu56`, and so on.

     mount dt0 ./system.tu56 no-overwrite

The `no-overwrite` option turns out to be extremely helpful in experimenting with
scripts that may or may not work the first time.

## umount -- Unmount a SIMH attached device image.

## `boot` -- Boot the named SIMH device.

`boot` _simh-device_

If you attempt to boot a device that is not attached the expect engine
will get confused.

## Other commands to continue execution in OS/8.

## `copy` -- Make a copy of a POSIX file

A common activity for os8-run is to make a copy of an image file,
and edit the image file.  To obviate the need for an external driver
that would create the copy, we added a copy command.

Adding an option to `mount` was considered, but in the interests
of allowing an arbitrary name for the modified image, a separate
command was used.

## `copy_into` -- Copy POSIX file *into* OS/8 environment

`copy_into` _posix-path_ [_option_]

`copy_into` _posix-path_ _os8-filespec_ [_option_]

The option is either empty or exactly one of

| `/A` | OS/8 `PIP` ASCII format. POSIX newlines are converted to OS/8 newlines.
| `/B` | OS/8 `PIP` `BIN` format. Paper tape leader/trailer may be added.
| `/I` | OS/8 `PIP` image format. Bit for Bit copy.

If no option is specified, `/A` is assumed.

Example:

Copy a POSIX file init.cm onto the default OS/8 device `DSK:` under the name `INIT.CM`:

     copy_into ../media/os8/init.cm

## `copy_from` -- Copy *from* OS/8 to a file in POSIX environment. 

Copy files from the running OS/8 environment to the POSIX environment running SIMH.

## `os8` -- Run arbitrary OS/8 command

This command should be used ONLY for OS/8 commands that return immediately to command
level.  `BATCH` scripts do this, and they can be run from here.

`os8` _os8-command-line_

The rest of the line is passed uninterpreted to the OS/8 keyboard monitor with
the expectation that the command will return to the monitor command level and
the command prompt, "`.`" will result.

## `pal8` -- Run OS/8 PAL-8 assembler

* run `PAL8` with either a 3 argument form that produces a listing file, or a 2 argument form that does not.

## `begin` / `end` -- Complex conditionals and sub-command blocks

* run `ABSLDR` and `FOTP`, cycling an arbitrary number of times through the command decoder.
* run 'BUILD' with arbitrarily complex configuration scripts, including a `BUILD` of a system head that inputs `OS8.BN` and `CD.BN`.

## `enable` / `disable` -- Set an enablement or disablement

The `enable` and `disable` commands allow dynamic control over conditional
execution in `begin` / `end` blocks.


As explained above, the `not-disabled` construct enables us to do
things by default without setting any explicit enablement -- an exception.
How do you implement an exception to an exception? Like this:

    # MACREL is enabled by default with no settings.
    # We need to avoid patching FUTIL in that default case
    # So we have to set a disablement of that action
    # Conditional on macrel not-disabled.
    
    begin not-disabled macrel
    disable futil_patch
    end not-disabled macrel
    
    begin not-disabled futil_patch
    # The two FUTIL patches only get applied to FUTIL V7 which comes with
    # OS/8 V3D to bring it up to V7D.  MACREL V2 comes with FUTIL V8B, so
    # these patches are skipped by mkos8 using an RE match on the file name
    # when the user does not pass --disable-os8-macrel to configure.
    patch ../media/os8/patches/FUTIL-31.21.1M-v7B.patch8
    patch ../media/os8/patches/FUTIL-31.21.2M-v7D.patch8
    end not-disabled futil_patch

## `patch` -- Run a patch file

* run of patch scripts that will use `ODT` or `FUTIL` to patch the booted system image.

## `configure` -- Perform certain SIMH configuration activities.

* configure the `tti`, `rx`, `td`, and `dt` devices at run time to allow shifting between otherwise incompatible configurations of SIMH and OS/8 device drivers.


## TODOs:

* Create a `resume` command that does not reboot OS/8
* Allow underscore and dash in mount options.
* Make `mount` failure abort the script.
* What happens if we don't have a done command in the script?



### <a id="license"></a>License

Copyright © 2018 by Bill Cattey. Licensed under the terms of
[the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
