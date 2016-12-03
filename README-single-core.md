# Running the PiDP-8/I Software on a Single-Core Pi

The software as shipped in release tarballs and on the trunk branch of
the Fossil repository assumes you will be running it on one of the more
modern multicore Pi boards such as the Pi 2 or the Pi 3.

If you want to run it on a Zero, the Raspberry Pi 1 A+ or the 1 B+, you
need to build the software without the incandescent lamp simulator
feature, as that takes too much CPU power to run on these lower-powered
Pi boards.

As of this writing, the only way to do that is to check the software out
of the Fossil repository directly rather than use one of the release
packages. The [`no-lamp-simulator` branch][nls] does this. So, follow
the instructions on the [front page][fp] of the repository to clone the
Fossil repository, then within your checkout, say:

    $ fossil update no-lamp-simulator

That will modify your local copy, removing the incandescent lamp
simulator feature.

[fp]:  https://tangentsoft.com/pidp8i/
[nls]: https://tangentsoft.com/pidp8i/timeline?n=100&r=no-lamp-simulator
