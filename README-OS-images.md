These OS images install to an SD card in the same way as the [official
Raspbian images][1].  Follow their [installation guide][2].  You will
need to use a 2 GB or larger SD card.

One small difference between these OS images and the ones that come from
the Raspberry Pi Foundation is that mine have already booted once, so
they won't automatically expand to fill your SD card's space. Simply run
`sudo raspi-config` after your first boot and select the **Expand
Filesystem** option, then reboot.

Another difference is the default user name and password:

*   **username:** `pidp8i`
*   **password:** `edsonDeCastro1968`

You will be made to change that password on first login.

(The "S" in ["IoT"][3] stands for "security." If we want security, we
have to see to it ourselves!)


[1]: https://www.raspberrypi.org/downloads/raspbian/
[2]: https://www.raspberrypi.org/documentation/installation/installing-images/README.md
[3]: https://en.wikipedia.org/wiki/Internet_of_things
