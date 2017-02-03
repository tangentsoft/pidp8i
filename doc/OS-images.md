The Raspbian OS images you can downlod from [the PiDP-8/I development
site][1] install to an SD card in the same way as the [official Raspbian
images][2].  Follow the [installation guide][3] published by the
Raspberry Pi Foundation.  You will need to use a 2 GB or larger SD card.

The contents of the Zip file are:

| File Name         | Description
|--------------------------------
| `README.md`       | this file
| `pidp8i-*.img`    | the OS image, based on Raspbian Jessie Lite
| `MANIFEST.txt`    | SHA 256 hash and file size for the OS image file

One small difference between these OS images and the ones that come from
the Raspberry Pi Foundation is that mine have already booted once, so
they won't automatically expand to fill your SD card's space. Simply run
`sudo raspi-config` after your first boot and select the **Expand
Filesystem** option, then reboot.

Another difference is the default user name and password:

*   **username:** `pidp8i`
*   **password:** `edsonDeCastro1968`

You will be made to change that password on first login.

Remember, [the S in IoT stands for Security.][5]  If we want security,
we have to see to it ourselves!

If you do not want your PiDP-8/I to be secure, see my other guide, "[How
to Run a Nekkid PiDP-8/I][6]."


[1]: https://tangentsoft.com/pidp8i/
[2]: https://raspberrypi.org/downloads/raspbian/
[3]: https://raspberrypi.org/documentation/installation/installing-images/
[4]: https://en.wikipedia.org/wiki/Internet_of_things
[5]: http://www.testandverification.com/iot/s-iot-stands-security/
[6]: https://tangentsoft.com/pidp8i/wiki?name=How+to+Run+a+Naked+PiDP-8/I
