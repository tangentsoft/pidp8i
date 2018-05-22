# `os8-run`: A Scripting Language for Driving OS/8

## History and Motivation

In the beginning, the PiDP-8/I project shipped a hand-made and
hand-maintained OS/8 disk image. This image had multiple problems, and
sometimes the by-hand "fixes" to those problems caused other problems.

For the 2017.12.22 release, [we][auth] created a tool called `mkos8`
which creates this disk programmatically based on the user's configuration
choices. Not only does this allow the disk image to be created, it also
allows it to be [tested][tm], because the process is purposefully done
in a way that it is [reproducible][rb].

That process worked fine for the limited scope of problem it was meant
to cover: creation of an OS/8 V3D RK05 image meant for use with SIMH's
PDP-8 simulator, configured in a very particular way. It doesn't solve a
number of related problems that should be simple extensions to the idea:

*   What if we want a different version of OS/8, such as V3F?

*   What if we want a different OS/8 `BUILD` configuration?

*   Not just that, but what if the new `BUILD` configuration changes the
    system device type, such as from an RK05 to an RL01 or RX02?

*   How do we make it drive other tools not already hard-coded into
    `mkos8` or its underlying helper library?

Shortly after release 2017.12.22 came out, Bill Cattey began work on
`os8-run` to solve these problems. This new tool implements a scripting
language and a lot of new underlying functionality so that we can not
only implement all of what `mkos8` did, we can now write scripts to do
much more.

The goal is to entirely replace `mkos8` and then also provide a suite of
scripts and documentation support for creating one's own scripts to
solve problems we haven't even anticipated.


[auth]: https://tangentsoft.com/pidp8i/doc/trunk/AUTHORS.md
[rb]:   https://reproducible-builds.org/
[tm]:   https://tangentsoft.com/pidp8i/doc/trunk/tools/test-mkos8


## <a id="capabilities"></a>Capabilities

`os8-run` is a general script running facility that can:

* attach device image files with options that include but also go beyond what SIMH offers:
    * Protect image by attaching it read-only.
    * Recognize the use case of working with a pre-existing image, and abort the script if the image is not found, rather than creating a new, blank image.
    * Protect a master boot image that will not boot read-only by creating a scratch copy and booting the copy instead.
    * Recognize the use case of creating a new, blank image, but preserving any pre-existing image files of the same name.
* boot OS/8 on an arbitrary attached device image.
* create a duplicate of an existing file. This is the use case of building new image files from an existing baseline while preserving the baseline image file.
* copy files into the running OS/8 from the POSIX environment running SIMH.
* copy files from the running OS/8 environment to the POSIX environment
running SIMH.
* run any OS/8 command as long as it returns immediately to the OS/8 Keyboard
Monitor. This includes BATCH scripts.
* run `ABSLDR` and `FOTP`, cycling an arbitrary number of times through the OS/8
Command Decoder.
* run `PAL8` with either a 3 argument form that produces a listing file,
or a 2 argument form that does not.
* run 'BUILD' with arbitrarily complex configuration scripts, including
the `BUILD` of a system head that inputs `OS8.BN` and `CD.BN`.
* configure the `tti`, `rx`, `td`, and `dt` devices at run time to allow
shifting between otherwise incompatible configurations of SIMH and OS/8
device drivers.
* run included script files so that common code blocks can be written once
in an external included script.
* run of patch scripts that will use `ODT` or `FUTIL` to patch files in
the booted system image.
* perform actions in a script conditionally on feature enablement matching
an arbitrary keyword.
* perform actions in a script unless a disablement keyword has been specified.
* set enable or disable keywords anywhere in the execution of a script.

Under the covers, `run-os8` is a Python script that uses the Python expect
library that is capable of handling complex dialogs with commands.

Sometimes debugging these scripts is challenging because if you fall
out of step with what is `expect`ed, the expect engine will get
confused.  The `os8-run` command hangs for a while and then times out
with a big Python backtrace.

Running`os8-run` with the `-v` option gives verbose output that
enables you to watch every step of the script running.


## <a id="examples"></a>Illustrative Examples

Here are some example os8-run scripts:

Example 1: Begin work on a new rk05 image that gets an updated version
of the OS/8 `BUILD` utility from POSIX source. (Perhaps it was found
on the net.)

    mount rk0 $bin/os8v3d-patched.rk05 must-exist
    mount rk1 $bin/os8-v3f-build.rk05 no-overwrite
    
    copy_into $src/os8/v3f/BUILD.PA RKA1:BUILD.PA
    
    boot rk0
    
    pal8 RKB1:BUILD.BN<RKA1:BUILD.PA
    
    begin cdprog SYS:ABSLDR.SV
    RKB1:BUILD.BN
    end cdprog SYS:ABSLDR.SV
    
    os8 SAVE RKB1:BUILD.SV
    
    done

The above script does the following:

* Attach the system that will do the work on rk0. It must exist.
* Create a new rk05 image, `os8-v3f-build.rk05` but don't wipe pre-exising versions of the same image.
* Copy the source `BUILD.PA` from the POSIX environment into the OS/8 environment.
* Run `PAL8` to assemble `BUILD.PA` into `BUILD.BN`.
* Run `ABSLDR` to load `BUILD.PA` into memory.
* Save the run image of `BUILD` as an executable on `RKB1:` of the new rk05 image.


## <a id="paths"></a>POSIX Path expansions

Notice in the above example the construct `$bin/` and `$src/` in the POSIX path
specifications.

We want `os8-run` to be able to find and use image files and other files both
at build time and after the built system is installed.  So abstract path keys
have been implemented with a syntax modeled on POSIX shell variables.

However these path expansions are currently very limited.  The substitution
can only occur at the very beginning of a POSIX file specification.  The only
values currently defined are:

| $build/   | The absolute path to the root of the build.
| $src/     | The absolute path to the root of the source.
| $bin/     | The directory where executables and runable image files are installed at build time
| $media/   | The absolute path to OS/8 media files
| $os8mi/   | The absolute path to OS/8 media files used as input at build time
| $os8mo/   | The absolute path to OS/8 media files produced as output at build time

To add new values modify `.../lib/pidp8i/dirs.py.in` and rebuild.  The `dirs.py`
file built from `dirs.py.in` is a very deep dependency.  Touching this file will
cause all the OS/8 bootable system image files to be rebuilt.


## <a id="contexts"></a>Execution contexts

It is important to be mindful of the different command contexts when
running scripts under `os8-run`:

* __SIMH context:__  Commands are interpreted by SIMH command processor.
* __OS/8 context:__  Commands are interpreted by the OS/8 Keyboard Monitor.
* __`begin` / `end` blocks:__  These create special interpreter loops with their
own rules.

Examples of `begin` / `end` blocks:

* __Command Decoder:__  Programs like `ABSLDR` and `FOTP` call the OS/8 Command Decoder
to get file specifications and operate on them. `os8-run` uses a `begin` / `end` block to
define set of files to feed to the Command Decoder and to indicate the last file, and
a return to the OS/8 context.
* __OS/8 `BUILD`:__ Commands are passed to `BUILD` and output is interpreted.  The `end`
of the block signifies the end of the `BUILD` program and a return to the OS/8 context.
* __Conditional Execution:__ Blocks of script code, delimited by a `begin` / `block` can be
either executed or ignored depending on the key word that is enabled when that block
is encountered.  This context is very interesting and is more fully documented below.

The commands that execute in the OS/8 environment require a system image
to be attached and booted.  Attempts to run OS/8 commands without having
booted OS/8 kill the script.

Scripting commands such as `mount`, `umount`, and `configure` execute
in the SIMH context. OS/8 is suspended for these commands.

Ideally we would just resume OS/8 with a SIMH continue command when we are
finished running SIMH commands. Unfortunately this does not work under Python
expect.  The expect engine needs a command prompt.

Although hitting the erase character (`RUBOUT`) or the line kill character
(`CTRL/U`) to a terminal-connected SIMH OS/8 session gives a command prompt,
these actions don't work under Python expect. We don't know why.

Booting OS/8 gives a fresh prompt.

Restarting the OS/8 Monitor with a SIMH command line of \"`go 7600`\"
works.

The least disruptive way we have found to resume OS/8 under Python expect
after having escaped to SIMH is to issue the SIMH `continue` command, then
pause for an keyboard delay, then send `CTRL/C` then pause again, then send
`\r\n`.  That wakes OS/8 back up and produces a Keyboard Monitor prompt.

The simh.py code that underlies all this keeps track of the switch
between the SIMH and OS/8 contexts.  However it does not presume to
do this resumption because the `CTRL/C` will quit out of any program
being run under OS/8, and return to the keyboard monitor level.

Because `os8-run` creates the `begin` / `end` blocks with their own
interpreter loops, around commands with complex command structures,
it guarantees that the switch into SIMH context will only happen
when OS/8 is quiescent in the Keyboard Monitor.

Although `os8-run` provides a `resume` command that can appear in
scripts after the commands that escape out to SIMH, using it is optional.
`os8-run` checks the context and issues its own resume call if needed.


## <a id="usage"></a>Usage

> `os8-run` [`-h`] [`-d`] [`-v`] [`-vv`] [_optional-arguments_ ...]  _script-file_ ...

|                           | **Positional Arguments**
| _script-file_             | One or more script files to run
|                           | **Optional Arguments**
| `-h`                      | show this help message and exit
| `-d`                      | add extra debugging output, normally suppressed
| `-v`                      | verbose script status output instead of the usual few progress messages
| `-vv`                     | very verbose: Includes SIMH expect output with the verbose output.
|                           | **Enable/Disable Options**
| `--disable-ba`            | Leave *.BA `BASIC` games and demos off the built OS/8 RK05 image
| `--enable-focal69`        | Add `FOCAL69` to the built OS/8 RK05 image
| `--disable-uwfocal`       | Leave U/W FOCAL (only) off the built OS/8 RK05 image
| `--disable-macrel`        | Leave the `MACREL` assembler off the built OS/8 RK05 image
| `--disable-dcp`           | Leave the `DCP` disassembler off the built OS/8 RK05 image
| `--disable-k12`           | Leave 12-bit Kermit off the built OS/8 RK05 image
| `--disable-cc8`           | Leave the native OS/8 CC8 compiler off the built OS/8 RK05 image
| `--disable-crt`           | Leave CRT-style rubout processing off the built OS/8 RK05 image
| `--disable-advent`        | Leave Adventure off the built OS/8 RK05 image
| `--disable-fortran-ii`    | Leave FORTRAN II off the built OS/8 RK05 image
| `--disable-fortran-iv`    | Leave FORTRAN IV off the built OS/8 RK05 image
| `--disable-init`          | Leave the OS/8 INIT message off the built OS/8 RK05 image
| `--enable-music`          | Add *.MU files to the built OS/8 RK05 image
| `--disable-chess`         | Leave the CHECKMO-II game of chess off the built OS/8 RK05 image
| `--enable-vtedit`         | Add the TECO VTEDIT setup to the built OS/8 RK05 image
| `--disable-lcmod`         | disable the OS/8 command upcasing patch; best set when
|                           | SIMH is set to `tti ksr` mode
| `--enable ENABLE`         | Execute script code within `begin enable` _ENABLE_ block.
| `--disable DISABLE`       | Ignore and do not execute script code within `begin
|                           | not-disabled` _DISABLE_ block.


## Script Language command inventory

Here is a list of the `os8-run` scripting language commands in alphabetical order.


| [`boot`](#boot-comm)           | Boot the named SIMH device.                      |
| [`begin`](#begin-end-comm)     | Begin complex conditional or sub-command block. |
| [`configure`](#configure-comm) | Perform specific SIMH configuration activities.  |
| [`copy`](#copy-com)            | Make a copy of a POSIX file.                     |
| [`copy_from`](#copy-from-comm) | Copy *from* OS/8 to a file in POSIX environment. |
| [`copy_into`](#copy-into-comm) | Copy POSIX file *into* OS/8 environment.         |
| [`disable`](#en-dis-comm)      | Set disablement of a feature by keyword.         |
| [`done`](#done-comm)           | Script is done.                                  |
| [`enable`](#en-dis-comm)       | Set enablement of a feature by keyword.          |
| [`end`](#begin-end-comm)       | End complex conditional or sub-command block.   |
| [`include`](#include-comm)     | Execute a subordinate script file.               |
| [`mount`](#mount-comm)         | Mount an image file as a SIMH attached device.   |
| [`os8`](#os8-comm)             | Run arbitrary OS/8 command.                      |
| [`pal8`](#pal8-comm)           | Run OS/8 `PAL8` assembler.                       |
| [`patch`](#patch-comm)         | Run a patch file.                                |
| [`resume`](#resume-comm)       | Resume OS/8 at Keyboard Monitor command level.   |
| [`restart`](#restart-comm)     | Restart OS/8.                                    |
| [`umount`](#umount-comm)       | Unmount a SIMH attached device image.            |

These commands are described in subsections of [Script Language
command reference](#scripting) below. That section presents commands
in an order appropriate to building up an understanding of making
first simple and then complex scripts with `os8-run`.


## <a id="scripting"></a>Script Language Command Reference

### <a id="done-comm"></a>`done` -- Script is done.

This is an explicit statement to end processing of the script.

* All temporary files are deleted.
* All attached SIMH image files are gracefully detached with any
pending writes completed.
* SIMH is gracefully shut down with a `quit` command.


### <a id="include-comm"></a>`include` -- Execute a subordinate script file.

`include` _script-file-path_

The script file named in _script-file-path_ is executed.  If no fatal
errors are encountered during that execution, then the main script
continues on.

A fatal error in an included script kills the whole execution of
`os8-run`.

Very few script language commands fail fatally.  The design principle
was to ask, "Is the primary use case of this command a prerequisite
for other work that would make no sense if this command failed?"  If
the answer is, "yes", then the failure of command kills the execution
of the whole script and aborts `os8-run`.  Commands that have fatal
exits are mentioned specifically in the command reference section.


### <a id="mount-comm"></a>`mount` -- Mount an image file as a SIMH attached device.

`mount` _simh-dev_ _image-file_ [_option_ ...]

Because the primary expectation with `os8-run` scripts is that image
files are mounted, booted and operated on, the failure of a `mount`
command is fatal.


#### `mount` Options

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
         

#### `mount` Examples

Mount the `os8v3d-patched.rk05` image, which must exist, on SIMH `rk0`.

    mount rk0 ./os8v3d-patched.rk05 must-exist

Mount the `advent.tu56` image, which must exist, on SIMH `dt1` which
will protect it from inadvertent modification.

    mount dt1 ../media/os8/subsys/advent.tu56 read-only must-exist

Create a writeable copy of the distribution DECtape,
`al-4711c-ba-os8-v3d-1.1978.tu56`, which must exist.  Mount it on SIMH
dt0 ready for for a read/write boot.  Delete the copy when the script
is done.

     mount dt0 ../media/os8/al-4711c-ba-os8-v3d-1.1978.tu56 must-exist copy_scratch

Create a new image file `system.tu56`.  If the file already exists,
create a new version.  If the numbered version file exists, keep
incrementing the version number for the new file until a pre-existing
file is not found.

For example, if `system.tu56` was not found, the new file would be
called `system.tu56`.  If it was found the next version would be
called `system_1.tu56`.  If `system_1.tu56` and `system_2.tu56` were
found the new file would be called `system_3.tu56`, and so on.

     mount dt0 ./system.tu56 no-overwrite

The `no-overwrite` option turns out to be extremely helpful in experimenting with
scripts that may or may not work the first time.


### <a id="umount-comm"></a>umount -- Unmount a SIMH attached device image.

`umount` _simh-device_

This is just a wrapper for the SIMH `detach` command.


### <a id="boot-comm"></a>`boot` -- Boot the named SIMH device.

`boot` _simh-device_

Boot OS/8 on the named _simh-device_ and enter the OS/8 run-time context.

The `boot` command tests to see if something is attached to the
SIMH being booted.  If nothing is attached the command fails with a
fatal error.

This test does not protect against trying to boot an image lacking a
system area and thus not bootable.  This can't be tested in advance
because booting a non-bootable image simply hangs the virtual machine.
Heroic measures, like looking for magic system area bits in the image
file were deemed too much work.

If an attempt is made to boot an image with no system area, `os8-run`
hangs for a while and then gives a timeout backtrace.


### <a id="resume-comm"></a>`resume` -- Resume OS/8 at Keyboard Monitor command level.

`resume`

As explained above in the [Execution contexts](#contexts) section, we
can't just issue a SIMH `continue` command because we need some output
from OS/8 running within SIMH to re-synchronize Python expect to.

After trying several different things that did not work, the least
disruptive action is to send `CTRL/C` and a newline with some keyboard
delays. The `resume` command does this.

However, because the context switches are well-defined, the `resume`
command is completely optional in scripts.  Instead `os8-run`, when it
detects the need to return to OS/8 from SIMH command level, will issue
a `resume` command to force a context switch. 


### <a id="restart-comm"></a>`restart` -- Restart OS/8.

`restart`

Equivalent to the SIMH command line of \"`go 7600`\".

Before `resume` was developed, the next less disruptive way to get an
OS/8 Keyboard Monitor prompt was to restart SIMH at address 07600.
This is considered a soft-restart of OS/8.  It is less disruptive than
a `boot` command, because the `boot` command loads OS/8 into main
memory from the boot device, whereas restarting at location 07600 is
just a resart without a reload.

The restart does re-initilaize some state so it is more disruptive
than the `CTRL/C` resume documented above.

XXX  -- not implemented yet --


### <a id="copy-comm"></a>`copy` -- Make a copy of a POSIX file.

`copy` _source-path_ _destination-path_

The most common activity for `os8-run` is to modify a system image.

However, we often want to keep the original and modify a copy.
For example, `os8v3d-patched.rk05`, a commonly used system image comes
from modifying `os8v3d-bin.rk05`.  We keep the latter around so we
don't have to keep rebuilding the baseline.

Instead of requiring some external caller to carefully preserve the
old file, the "make a copy with arbitrary name" functionality was
added by way of this command.

Adding an option to `mount` was considered, but in the interests
of allowing an arbitrary name for the modified image, a separate
command was created.


### <a id="copy-into-comm"></a>`copy_into` -- Copy POSIX file *into* OS/8 environment.

`copy_into` _posix-path_ [_option_]

`copy_into` _posix-path_ _os8-filespec_ [_option_]

The option is either empty or exactly one of

| `/A` | OS/8 `PIP` ASCII format. POSIX newlines are converted to OS/8 newlines.
| `/B` | OS/8 `PIP` `BIN` format. Paper tape leader/trailer may be added.
| `/I` | OS/8 `PIP` image format. Bit for Bit copy.

If no option is specified, `/A` is assumed.

In the first form of the command, the OS/8 file specification is left
out, and one is synthesized from the file component of the _posix-path_.

This is how you get files *into* OS/8 from the outside world.  For
example, this enables source code management using modern tools.  The
builder script would check out the latest source and use an `os8-run`
script beginning with a `copy_into` command to send it to OS/8 for
assembly, linking, installation, etc.

Example:

Copy a POSIX file init.cm onto the default OS/8 device `DSK:` under the name `INIT.CM`:

     copy_into ../media/os8/init.cm


### <a id="copy-from-comm"></a>`copy_from` -- Copy *from* OS/8 to a file in POSIX environment. 

`copy_from`_os8-filespec_ _posix-path_ [_option_]

The option is either empty or exactly one of

| `/A` | OS/8 `PIP` ASCII format. POSIX newlines are converted to OS/8 newlines.
| `/B` | OS/8 `PIP` `BIN` format. Paper tape leader/trailer may be added.
| `/I` | OS/8 `PIP` image format. Bit for Bit copy.

If no option is specified, `/A` is assumed.

Unlike `copy_into` there is only one form of the command.  Both the
_os8-filespec_ and the _posix-path_ must be specified.  The options
are the same for both `copy_from` and `copy_into`.

Copy files from the running OS/8 environment to the POSIX environment running SIMH.

Example:

Copy a listing file into the current working directory of the
executing `os8-run`:

    copy_from DSK:OS8.LS ./os8.ls /A


### <a id="os8-comm"></a>`os8` -- Run arbitrary OS/8 command.

`os8` _os8-command-line_

Everything on the script command line after \"os8 \" is passed,
uninterpreted, to the OS/8 keyboard monitor with the expectation that
the command will return to the monitor command level and the command
prompt, "`.`" will be produced.

This command should be used ONLY for OS/8 commands that return
immediately to command level.  `BATCH` scripts do this, and they can
be run from here.


### <a id="pal8-comm"></a>`pal8` -- Run OS/8 `PAL8` assembler.

`pal8` _os8-bn-spec_`<`_os8-pa-spec_

`pal8` _os8-bn-spec_`,`_os8-ls-spec_`<`_os8-pa-spec_

Note that the parser for this wrapper for `PAL8` is much too
conservative in what it allows:

* No `PAL8` options are allowed.
* No whitespace within the `PAL8` command call specification.
* Only two ways to call `PAL8`:
    * two argument form with binary and source or
    * three argument form with
* binary, listing, and source.
* _os8-bn-spec_` must specify a binary filename ending in `.BN`
* _os8-ls-spec_` must specify a listing filename ending in `.LS`
* _os8-pa-spec_` must specify a source filename ending in `.PA`

This should be improved.  The reason why this wrapper is so
constrained is that it evolved from extremely rudimentary, hard-coded
scripts, and hasn't been worked on since reaching minimum necessary
functionality.

The three file name specifiers can include an OS/8 device specification.

Run `PAL8` with either a 3 argument form that produces a listing file,
or a 2 argument form that does not.

Examples:

Create a binary `OS8.BN` on partition B of rk05 drive 1 from `OS8.PA`
source file found on partition A of rk05 drive 1.

    pal8 RKB1:OS8.BN<RKA1:OS8.PA

Create a binary `OS8.BN` on partition B of rk05 drive 1 and a listing
file `OS8.LS` on the default device `DSK:` from `OS8.PA` source file
found on partition A of rk05 drive 1.

    pal8 RKB1:OS8.BN,OS8.LS<RKA1:OS8.PA


### <a id="begin-end-comm"></a>`begin` / `end` -- Complex conditionals and sub-command blocks.

`begin` _keyword_ _argument_

`end` _keyword_

_keyword_ is either one of the following:

| `cdprog`       | `Command loop through OS/8 Command Decoder with _argument_ specifying an OS/8 executable program by name and (optionally) device.|
| `build`        | `BUILD` command interpreter with dialogs manged with Python expect.    |
| `enabled`      | Execution block if _argument_ is enabled. (See the [`enable` \ `disable`](#en-dis-comm)) section below. |
| `not-disabled` | Execution block if _argument_ is not disabled. (See the [`enable` \ `disable`](#en-dis-comm))  section below. |

For `cdprog`, and `build`, _argument_ is passed uninterpreted to the
OS/8 `RUN` command.  It is expected that _argument_ will be the name
of an executable, optionally prefixed by a device specification. This
enables running the OS/8 command from specific devices. This is
necessary for running specific `BUILD` command for construction of
system images for specific versions of OS/8 that are __different__
from the default run image.

Example:

Run `FOTP.SV` from device `RKA0` and cycle through the command decoder
to copy files onto a DECtape under construction from two different
places: the old system on `RKA0:` and the newly built components from
`RKB1:`.

    begin cdprog RKA0:FOTP.SV
    DTA0:<RKA0:FOTP.SV
    DTA0:<RKA0:DIRECT.SV
    DTA0:<RKB1:CCL.SV
    DTA0:<RKB1:RESORC.SV
    end cdprog RKA0:FOTP.SV

The `build` command has had a lot of work put into parsing dialogs.
This enables not only device driver related `BUILD` commands of
`LOAD`, `UNLOAD`, `ENABLE` and `DISABLE`, but also answers "yes" to
the "ZERO SYS" question when the `BOOT` command is issued on a brand
new image file.

Example:

Build a rudimentary system for a TC08 DECtape.

    begin build SYS:BUILD
    DELETE SYS,RKA0,RKB0
    DELETE RXA0
    INSERT RK05,RKA0,RKB0
    SYSTEM TC08
    INSERT TC08,DTA0
    INSERT TC,DTA1
    DSK TC08:DTA0
    BOOT
    end build


Most importantly there is full support for the dialog with the `BUILD`
command within the `BUILD` program to create a new OS/8 system head
with new versions of `OS8.BN` and `CD.BN` assembled from source.

Example:

To create a system tape with new OS/8 Keyboard Monitor and Command
Decoder, the above example would add the following just before the
`BOOT` line:

    BUILD DSK:OS8.BN DSK:CD.BN

Note: OS/8 disables the `BUILD` command within the `BUILD`
program after it has been issued during a run.  Traditionally, the
first action after a `BOOT` of a newly built system is to `SAVE` the
just executed memory image of `BUILD`.  That saves all the device
configurations, but also saves a version with the `BUILD` command
within the `BUILD` program disabled.

For this reason, you have to run a fresh version of `BUILD` from
distribution media rather than one saved from a previous run.  This
situation is what drove support for the _argument_ specifier to name
the location of the program to run rather than always running from a
default location.

Also, `BUILD` is too sensitive to the location of the `OS8.BN` and
`CD.BN` files. It pretty much only works if you use `PTR:` or `DSK:`.
Anything else seems to just hang.  I believe the root cause is that,
although the device and file are parsed, the actual device has to be
either `PTR:` or the active system device.

`os8-run` contains two lists of keywords that have been set as enabled
or disabled.  The setting is done either with `os8-run` command line
arguments or with the `enable` and `disable` commands (documented
below.)

Two lists are required because default behavior is different for
enablement versus disablement.

The `begin enabled` block looks on the `enabled` list for the
keyword. If **no** such keyword is found, the block is ignored.

The `begin not-disabled` block looks on the `disabled` list for the
keyword. If such a keyword **is** found the block is ignored.

A `not-disabled` block allows lines of the script to be run unless a
keyword has been set as a disablement.  Whereas the `enabled` block
requires an explicit keyword enablement, the `not-disabled` block
requires no enablement. It is a default that requires explicit
disablement.

This dichotomy of `enabled` versus `not-disabled seems strange until
you start writing scripts where you want conditional behavior that is
enabled by default without having to touch other parts of the build
system to inform them about new enablement keywords.  So
`not-disabled` is for blocks of scripting code that is executed by
default unless explicitly disabled.
 
All these conditional and sub-command blocks must terminate with an
`end` command.  The _keyword_ of the end command must always match the
_begin_ command.  The argument of `enabled` and `not-disabled` blocks
must also match. Nesting is possible, but care should be exercised to
avoid crossing nesting boundaries.

For example:

    begin enabled outer
    begin enabled inner
    end enabled inner
    end enabled outer

is correct, but:

    begin enabled first
    begin enabled second
    end enabled first
    end enabled second

is an error.


### <a id="en-dis-comm"></a>`enable` / `disable` -- Set an enablement or disablement.

`enable` _keyword_

`disable` _keyword_

The `enable` and `disable` commands are used within scripts to
dynamically set enablement and disablement.  This expands the scope of
conditional execution beyond setting passed in from the `os8-run`
command line.

As mentioned above, there are two lists of keywords, one for `enabled`
keywords and one for `disabled` keywords.

The `enable` command not only adds the keyword to the `enabled`
list. It also looks for the keyword on the `disabled` list.  If the
keyword is found on the `disabled` list, it is **removed**.

Similarly, the `disable` command adds the keyword to the `disabled`
list, and searches the `enabled` list for the keyword.  If it is found
on the `enabled` list, it is removed.

A keyword, will appear only once, if present at all, and will be on
only one of the two lists.

The rule is: Last action wins.

Why all this complexity? Here is an example we tripped over and had to
implement:  We normally apply patches to the version of `FUTIL` that
came on the OS/8 v3d distribution DECtapes.  We also have an add-on
for the `MACREL` assembler.  That add-on contains a version of `FUTIL`
with updates required to work with binaries assembled with `MACREL`
v2. The `os8v3d-patch.mkos8` script needed to either avoid trying to
patch an updated `FUTIL` if `MACREL` was present, or to perform the
patching action if `MACREL` was not present.

A further complication is that we opt in to including the `MACREL`
add-on by default.  We deal with this triple negative by setting
`disable futil_patch` by default, unless `macrel` gets disabled:

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


### <a id="patch-comm"></a>`patch` -- Run a patch file.

`patch` _patch-file-path_

Run _patch-file-path_ file as a script that uses `ODT` or `FUTIL` to
patch the booted system image.


### `configure` -- Perform specific SIMH configuration activities.

`configure` _device_ _setting_

The settings are device specific:

| -------- | --------------------------------------------------------------- |
| **tape** | **DECtape device settings**                                |
| `dt`     | Set TC08 operation by enabling `dt` as the SIMH DECtape device. |
| `td`     | Set TD8e operation by enabling `td` as the SIMH DECTape device. |
| -------- | --------------------------------------------------------------- |
| **tti**  | **Console terminal input device settings**               |
| `KSR`    | Upper case only operation. Typed lower case caracters    |
|          | are upcased automatically before being sent to OS/8      |
| `7b`     | SIMH 7bit mode.  All characters are passed to OS/8       |
|          | without case conversion.                                 |
| -------- | --------------------------------------------------------------- |
| **rx**   | **Floppy Disk device settings**                          |
| `RX8E`   | Set the SIMH `rx` to `RX8E` mode compatible with RX01    |
|          | Floppy Disk Drives.                                      |
| `RX28`   | Set the SIMH `rx` to `RX28` mode compatible with RX02    |
|          | Floppy Disk Drives.                                      |
| `rx01`   | Synonym for the `RX8E` option. Compatible with RX01.     |
| `rx02`   | Synonym for the `RX28` option. Compatible with RX02.     |
| -------- | --------------------------------------------------------------- |

This command allows reconfiguration of the SIMH devices during the
execution of a `os8-run` script.  This command makes it possible to
create system images for hardware configurations that are not what are
commony used for OS/8 operation under SIMH.

The best example is the dichotomy between TD8e and TC08 DECTape.

TC08 is a DMA device. It is trivial to emulate. The SIMH device driver
simply copies blocks around in the .tu56 DECtape image.

TD8e is an inexpensive, DECtape interface on a single hex width card
for PDP8 hardware supporting the Omnibus&tm.  The CPU does most of the
work. Although a SIMH emulation is available for TD8e, it runs
perceptably and often unacceptably more slowly than the simple TC08
emulation.

However, hardware in the field most often has the TD8e DECtape because
it was inexpensive.

By allowing reconfiguration inside a script, we can use TC08 by
default, switch to TD8e to run `BUILD` and create .tu55 tape images
suitable for deployment on commonly found hardware out in the real
world. 


## TODOs

* What happens if we don't have a done command in the script?
* Add restart command.
* Allow whitespace on the pal8 command line.
* Allow passing in of arguments to PAL8.
* Add sanity check parse of sub-commands to confirm command. **OR** Change the 
begin command to treat _argument_ not as a full command, but merely
a device from which to fetch the command.  Maybe make _argument_ optional.


## Notes

* Multi word mount options can be separated either by a dash or an underscore.


### <a id="license"></a>License

Copyright © 2018 by Bill Cattey and Warren Young. Licensed under the
terms of [the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
