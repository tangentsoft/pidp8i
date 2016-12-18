# PiDP-8/I Software Release Process

If you are just a user of this software, you need read no further. This
document is for those producing release versions of the software, or for
those curious about what goes into doing so.


## Update ChangeLog.md

Trawl the Fossil timeline for user-visible changes since the last
release, and write them up in user-focused form into the `ChangeLog.md`
file. If a regular user of the software cannot see a given change, it
shouldn't go in the `ChangeLog.md`; let it be documented via the
timeline only.

Run `make release` to tag the release and check the `ChangeLog.md` file
changes in.


## Update the Home Page Links

The zip and tarball links on the front page produce files named after
the date of the release. Those dates need to be updated immediately
after tagging the release, since they point at the "release" tag applied
by the previous step, so they begin shipping the new release immediately
after tagging it.


## Produce the Normal Binary OS Image

Start with the latest [Raspbian Lite OS image][os].

1.  If the version of the base OS has changed since the last binary OS
    image was created, download the new one and blast it onto an SD card
    used for no other purpose. Boot it up.

2.  After logging in, run the first BOSI script:

		$ exec sudo -i
        # curl -L https://tangentsoft.com/bosi | bash
 
    It will either reboot the system after completing its tasks
    successfully or exit early, giving the reason it failed.

3.  Test that the software starts up as it should.

4.  Reset the OS configuration with `pidp8i/tools/bosi2`.

5.  Move the SD card to a USB reader plugged into the Pi, and boot the
    Pi from its prior SD card.

6.  Shrink the image with `pidp8i/tools/bosi3`.

7.  Move the USB reader to the Mac,¹ then say:

         $ ~/src/pidp8i/trunk/tools/bosi4 BLOCKS TAG

    where `BLOCKS` is the blocks value output at the end of `bosi3` and
    `TAG`, if given at all, should be `nls` if this is the second pass
    through this list for the `no-lamp-simulator` image.

8.  Blast image back onto SD card and test that it still works:

         $ ~/src/pidp8i/trunk/tools/bosi5 TAG

9.  While image uploads, compose the announcement message, and modify
    the front page to point to the new images. Post the announcement
    message and new front page once the uploads complete.

[os]: https://www.raspberrypi.org/downloads/raspbian/


## Produce the "No Lamp Simulator" Binary OS Image

Log into the SD card from which you made the regular image above, then
say `pidp8i/tools/bosi1 no-lamp-simulator` and continue from step 3
above.


----------------------

### Footnotes

1.  The image production steps could just as well be done on a Linux box
    or on a Windows box via Cygwin or WSL, but the commands in that
    final stage change due to OS differences.  Since this document
    exists primarily for use by the one who uses it, there is little
    point in having alternatives for other desktop OSes above.  Should
    someone else take over maintainership, they can translate the above
    commands for their own desktop PC.
