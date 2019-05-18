The Raspbian OS images you can download from [the PiDP-8/I development
site][1] are based on the [official Raspbian images][2], so the
Raspberry Pi Foundation’s [installation guide][3] applies just as well
to our PiDP-8/I software images.  I particularly like using the
[Etcher][7] method, even on a POSIX system, since it can write the SD
card directly from the Zip file, without requiring that you unpack the
`*.img` file within first.

You will need to use a 2 GB or larger SD card.

The contents of the Zip file are:

| File Name         | Description
|--------------------------------
| `README.md`       | this file
| `pidp8i-*.img`    | the OS image, based on Raspbian Stretch Lite
| `MANIFEST.txt`    | SHA-256 hash and file size for the OS image file

Aside from having the PiDP-8/I software installed and running, the
primary difference between this disk image and the official Raspbian
Lite image is the default user name and password:

*   **username:** `pidp8i`
*   **password:** `edsonDeCastro1968`

You will be made to change that password on first login.

Remember, [the S in IoT stands for Security.][5]  If we want security,
we have to see to it ourselves!

If you do not want your PiDP-8/I to be secure, see my other guide, "[How
to Run a Naked PiDP-8/I][6]."


[1]: https://tangentsoft.com/pidp8i/
[2]: https://raspberrypi.org/downloads/raspbian/
[3]: https://raspberrypi.org/documentation/installation/installing-images/
[4]: https://en.wikipedia.org/wiki/Internet_of_things
[5]: http://www.testandverification.com/iot/s-iot-stands-security/
[6]: https://tangentsoft.com/pidp8i/wiki?name=How+to+Run+a+Naked+PiDP-8/I
[7]: https://www.balena.io/etcher
