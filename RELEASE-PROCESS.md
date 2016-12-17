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

Run `make release` to tag the release and check the `ChangeLog.md` file changes in.


## Update the Home Page Links

The zip and tarball links on the front page produce files named after
the date of the release. Those dates need to be updated immediately
after tagging the release, since they point at the "release" tag, so
they begin shipping the new release immediately after tagging it.


## Produce the Binary OS Image

Start with the latest [Raspbian Lite OS image][os].

[os]: https://www.raspberrypi.org/downloads/raspbian/

If the OS version has changed since the last release, use the following
steps to bootstrap the installation:

1.  Blast the OS image onto an SD card used for no other purpose. Boot it up.

2.  After logging in as `pi` for the first time:

        $ sudo apt update && sudo apt upgrade
        $ sudo apt install fossil
        $ mkdir museum pidp8i
        $ fossil clone https://tangentsoft.com/pidp8i museum/pidp8i.fossil
        $ cd pidp8i
        $ fossil open ~/museum/pidp8i.fossil release
        $ ./configure
        $ make
        $ sudo make install
        $ make clean

    The final set of commands are separate because you must carefully
    inspect each one's output to ensure that it doesn't say anything
    surprising.

3.  Reboot and test that the software starts up as it should.

4.  Revert Raspbian's automatic setup steps to force them to be done
    again on the end-user's Pi:

        $ sudo shred -u /etc/ssh/*key*
        $ sudo dphys-swapfile uninstall
        $ sudo dd if=/dev/zero of=/junk bs=1M
        $ sudo rm /junk
        $ history -c ; rm ~/.bash_history

5.  Shut it down, move the SD card to a USB reader plugged into the Pi,
    and boot the Pi from its prior SD card.

6.  Shrink the image in preparation for copying:

        $ sudo umount /dev/sda2         # if it auto-mounted
        $ sudo e2fsck -f /dev/sda2      # resize2fs demands it
        $ sudo resize2fs -M /dev/sda2
        $ sudo parted /dev/sda resizepart 2 $((XXXX * 4096 + 10**8))b
        $ sudo resize2fs /dev/sda2      # take any slack back up

    XXXX in the `parted` command comes from the output of the *first*
    resize2fs command.  A plausible value is 323485, meaning ~1.3 GiB.

    The extra 100 megs accounts for the `/boot` partition, since the
    `resizepart` command takes a partition end value, not a size value.
    It's a bit over, which is just as well since it means an end user
    who fails to expand the FS to fill their SD card won't run out of
    space too soon.

7.  Move the USB reader to the Mac,¹ then produce the updated image:

        $ sudo diskutil unmountDisk /dev/disk9    # it auto-mounted
        $ sudo dd if=/dev/disk9 bs=4k count=YYYY >
          DLDIR/pidp8i-$(date +%Y.%m.%d)-jessie-lite.img
        $ zip -9 DLDIR/pidp8i-$(date +%Y.%m.%d)-jessie-lite.img.zip \
          DLDIR/pidp8i-$(date +%Y.%m.%d)-jessie-lite.img

    YYYY is output from the *second* `resize2fs` command above, and
    gives the end position of the second partition on the SD card, and
    thus the size of the raw image.

    DLDIR is the directory exposed by the web server as `/dl`.

    The "jessie-lite" tag is at the end on purpose so that a directory
    full of these images will sort properly as new OS versions come out.

    If this is the `no-lamp-simulator` version, add an `-nls` tag after
    the date.

8.  Blast image back onto SD card and test that it still works:

        $ sudo dd if=DLDIR/pidp8i-$(date +%Y.%m.%d)-jessie-lite.img \
                  of=/dev/rdisk9 bs=1m

9.  Remove `*.img` files and upload the new `*.zip` files left behind.

10. Start the images uploading, compose the announcement message, and
    modify the front page to point to the new images. Post the
    announcement message and new front page once the uploads complete.


## Shortcut Path

Skip step 1 above if the latest Raspbian Lite OS image is unchanged
since the last release, and reduce the command sequence in step 2 to:

    $ sudo apt update && sudo apt upgrade
    $ cd pidp8i
    $ fossil update
    $ make
    $ sudo make install
    $ make clean

Proceed from there with step 3.  There are no more shortcuts, alas.


----------------------

### Footnotes

1.  The image production steps could just as well be done on a Linux box
    or on a Windows box via Cygwin or WSL, but the commands in that
    final stage change due to OS differences.  Since this document
    exists primarily for use by the one who uses it, there is little
    point in having alternatives for other desktop OSes above.  Should
    someone else take over maintainership, they can translate the above
    commands for their own desktop PC.
