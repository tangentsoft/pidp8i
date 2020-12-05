# PiDP-8/I Software Release Process

This documents the process for producing release versions of the
software.


## *Can* You Release Yet?

Before release, you must:

*   Fix all Immediate, High, and Medium priority [Bugs](/bugs)
*   Implement all Immediate and High priority [Features](/features)

Or reclassify them, of course.

Most of the bug levels simply aid scheduling: Immediate priority bugs
should be fixed before High, etc. Low priority bugs are "someone should
fix this someday" type of problems; these are the only ones allowed to
move from release to release. Think of bugs at this level as analogous
to the `BUGS` section of a Unix man page: our "low" intent to fix these
problems means they may stay at this level indefinitely, acting only as
advisories to the software's users.

The Features levels may be read as:

*   **Immediate**: ASAP, or sooner. :)
*   **High**: Features for the upcoming release.
*   **Medium**: Features we'll look at lifting to High for the release
    after that.
*   **Low**: "Wouldn't it be nice if..."


## Update SIMH

If `tools/simh-update` hasn't been run recently, you might want to do
that and re-test before publishing a new version.


## Testing

How do we gain confidence that what we will release is working with
minimal regressions?

The buld process operates in layers starting with SIMH and the OS/8
distribution DECtape image files as the lowest level. The layering goes like this:

1. Boot the OS/8 V3D Distribution DECtape image,
.../media/os8/v3d/al-4711c-ba-os8-v3d-1.1978.tu56. This creates
v3d-dist.rk05 which is booted to continue the build process.

2. Boot v3d-dist.rk05 to apply patches to create a useful image with
the latest bug fixes, v3d-patched.rk05. Strictly speaking, this is the
optimal, but minimum platform for continuing to build, and to operate
utilities.

3. Boot v3d-patched.rk05 to build e8, cc8, and anything else that
needs OS/8. This is the platform that should be used to build v3f from
source, and the OS/8 Combined Kit from source. Creating the images
used to assemble v3f and ock from source requires os8-cp.

4. Install packages built with OS/8 onto runable packs. This is where
v3d-patched.rk05 becomes v3d.rk05. At the present moment, the
component rk05 images that will be gathered into ock-dist.rk05 have
been built using v3d-patched.rk05. They could have been built using
v3d.rk05. The choice is made in the build scripts. The ock-dist.rk05
image is constructed similarly to layer 1. The
al-4711c-ba-os8-v3d-1.1978.tu56 image is booted, and used to create an
rk05 image, which is then populated with the other components built in
layer 3. At this point we have bootable tu56 images for v3d and v3f
built from v3d.rk05. We also have ock-dist.rk05 built from source.

5. Boot ock-dist.rk05 and apply patches to create ock-patched.rk05.

6. Install packages such as cc8 and e8 on ock-patched.rk05 to create
ock.rk05. This completes all building.

Layer 1 shows that SIMH basically works.  Each subsequent layer is proof
that the basic operation of the previous layer works.  So a successful
build of everything has provided a fair bit of coverage.

The next challenge is functional tests for leaf node packages like
`cc8` and `advent`.  The `os8-progtest` tool tests these.

The leaf node package management files live in the `pspec` subdirectory
of the source tree.  Tests for the packages live in `scripts/os8-progtest`.
`auto.def knows how to discover new packages and new tests and
incorporate them into `Makefile`.

`Makefile` contains a `make test` target that runs os8-progtest
on all discovered tests.

So the "basic smoke test" of this distribution consists of:

1. Successful completion of the build process.
2. Successful run of `make test`.

## Publish OS/8 RK05s

Re-configure the software with default settings, remove `bin/*.rk05`,
rebuild, and run `tools/publish-os8` to send the "final" OS/8 disk
images for this version of the software up to tangentsoft.com as
unversioned assets.

Update the date stamp in the "OS/8 RK05 Media" section of the project
home page.


## Update ChangeLog.md

Trawl the Fossil timeline for user-visible changes since the last
release, and write them up in user-focused form into [the `ChangeLog.md`
file][cl]. If a regular user of the software cannot see a given change,
it shouldn't go in the `ChangeLog.md`; let it be documented via the
timeline only.

[cl]: https://tangentsoft.com/pidp8i/doc/trunk/ChangeLog.md


## Update the Release Branch

Run `make release` to check the `ChangeLog.md` file changes in, merge
trunk to the `release` branch, and apply a tag of the form vYYYYMMDD to
that marge checkin.

It runs entirely automatically unless an error occurs, in which case it
stops immediately, so check its output for errors before continuing.


## Update the Home Page Links

The zip and tarball links on the front page produce files named after
the date of the release. Those dates need to be updated immediately
after tagging the release, since they point at the "release" tag applied
by the previous step, so they begin shipping the new release immediately
after tagging it.


## <a id="bosi"></a>Produce the Normal Binary OS Image

Start with the latest version of [Raspberry Pi OS Lite][os] (né Raspbian) on a multi-core
Raspberry Pi.

1.  If the version of the base OS has changed since the last binary OS
    image was created, download the new one.

    While the OS is downloading, zero the SD card you're going to use
    for this, so the prior contents don't affect this process.
    
    Blast the base OS image onto the cleaned SD card.

2.  Boot it up on a multi-core Pi.

    Log in, then retreive and initialize BOSI:

        $ wget https://tangentsoft.com/bosi
        $ chmod +x bosi
        $ exec sudo ./bosi init

    The `exec` bit is required so that the `bosi` invocation is run as
    root without any processes running as `pi` in case the `init` step
    sees that user `pi` hasn't been changed to `pidp8i` here: the
    `usermod` command we give to make that change will refuse to do what
    we ask if there are any running processes owned by user `pi`.

    It will either reboot the system after completing its tasks
    successfully or exit early, giving the reason it failed.

3.  Clone the software repo and build the software:

        $ ./bosi build

    On reboot, say `top -H` to make sure the software is running and
    that the CPU percentages are reasonable for the platform: if the CPU
    is mostly idle, the simulator isn’t running, and you need to figure
    out why before proceeding.

    You may also want to check that it is running properly with a
    `pidp8i` command.  Is the configuration line printed by the
    simulator correct?  Does OS/8 run?  Are there any complaints from
    SIMH, such as about insufficient CPU power?

4.  Do the final inside-the-image steps:

        $ ./bosi prepare

5.  Move the SD card to a USB reader plugged into the Pi, boot the Pi
    from its prior SD card, and shrink the OS image:

        $ wget https://tangentsoft.com/bosi
        $ chmod +x bosi
        $ ./bosi shrink

6.  Move the USB reader to the Mac,¹ then say:

        $ bosi image [nls]

    For the ILS images, you can give "ils" as a parameter or leave it
    blank.

7.  The prior step rewrote the SD card with the image file it created.
    Boot it up and make sure it still works.  If you're happy with it,
    give this command *on the desktop PC*.

        $ bosi finish [nls]

    As above, the parameter can be "ils" or left off for the ILS images.

[os]: https://www.raspberrypi.org/downloads/raspbian/


## Produce the "No Lamp Simulator" Binary OS Image

Do the same series of steps above on a single-core Raspberry Pi, except
that you give "nls" parameters to the `image` and `finish` steps.


## Publicizing

While the NLS image uploads — the ILS image was already sent in step 7
in the first pass through the list above — compose the announcement
message, and modify the front page to point to the new images.  You
might also need to update the approximate image sizes reported on that
page.  Post the announcement message and new front page once that second
upload completes.


----------------------

### Footnotes

1.  The image production steps could just as well be done on a Linux box
    or on a Windows box via Cygwin or WSL, but the commands in that
    final stage change due to OS differences.  Since this document
    exists primarily for use by the one who uses it, there is little
    point in having alternatives for other desktop OSes above.  Should
    someone else take over maintainership, they can translate the above
    commands for their own desktop PC.


### License

Copyright © 2016-2020 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
