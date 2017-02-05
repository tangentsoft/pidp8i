# PiDP-8/I Software Release Process

This documents the process for producing release versions of the
software.


## Update ChangeLog.md

Trawl the Fossil timeline for user-visible changes since the last
release, and write them up in user-focused form into the `ChangeLog.md`
file.  If a regular user of the software cannot see a given change, it
shouldn't go in the `ChangeLog.md`; let it be documented via the
timeline only.


## Update the Release Branch

Run `make release` to check the `ChangeLog.md` file changes in, tagging
that checkin with a release version tag of the form vYYYYMMDD and merge
those changes into the `release` branch.

It runs entirely automatically unless an error occurs, in which case it
stops immediately, so check its output for errors before continuing.


## Update the Home Page Links

The zip and tarball links on the front page produce files named after
the date of the release. Those dates need to be updated immediately
after tagging the release, since they point at the "release" tag applied
by the previous step, so they begin shipping the new release immediately
after tagging it.


## Produce the Normal Binary OS Image

Start with the latest version of [Raspbian Lite][os] on a multi-core
Raspberry Pi.

1.  If the version of the base OS has changed since the last binary OS
    image was created, download the new one and blast it onto an SD card
    used for no other purpose.  Boot it up.

2.  After logging in, retreive and initialize the BOSI process:

        $ wget https://tangentsoft.com/bosi
        $ chmod +x bosi
        $ ./bosi init
 
    It will either reboot the system after completing its tasks
    successfully or exit early, giving the reason it failed.

3.  Test that the software starts up as it should.

4.  Reset the OS configuration:

        $ exec sudo ./bosi reset

    The `exec` bit is required so that the `bosi` invocation is run as
    root without any processes running as `pi` in case the `init` step
    sees that user `pi` hasn't been changed to `pidp8i` here: the
    `usermod` command we give to make that change will refuse to do what
    we ask if there are any running processes owned by user `pi`.

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

Copyright © 2016-2017 by Warren Young. This document is licensed under
the terms of [the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
