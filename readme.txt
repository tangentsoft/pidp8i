20151215 release of PiDP-8 software     
=======================================================================

Please read carefully... 
- use install script only on a spare SD card with fresh Raspbian, 
  just to avoid any risk.

- IMPORTANT: this script assumes user is 'pdp'. 
  Not 'pi'. Unless you change the install script in ./install/

- This should run on any Raspberry Pi, even without the PiDP-8 front panel hardware. 
  Of course you can't use the front panel switch commands. Without the front panel, it's just a version of simh.


A. The pidp8 program itself
---------------------------

For default version:
1 - Unpack into /opt/pidp8
2 - cd /opt/pidp8/src, sudo make
3 - cd /opt/pidp8/src/scanswitch, sudo make

Only for 'serial mod' version:
1.1 - if you've done the 'serial mod', edit Makefile: add -DSERIALSETUP to CFLAGS line
 

You should be able to run /opt/pidp8/bin/pidp8 after you have disabled the Raspberry Pi's GPIO serial port. The install script below will do that for you, or you can do it manually (google on how to). 
In short: 'sudo /opt/pidp8/bin/pidp8 /opt/pidp8/bootscripts/0.script' would start you up with OS/8. Pidp8 needs sudo, as it accesses the gpio ports.


B. The install script
---------------------

Running the install script: only after compiling the program.

4 - cd /opt/pidp8/install, sudo ./pidp8-setup-default.sh

For 'serial mod' version, run pidp8-setup-serial.sh instead.

The install script:
- installs packages usbmount and screen (useful but not required to run pidp8)
- disables the Pi's GPIO serial port (required for standard version of pidp8)
- inserts pidp8 into the boot process of the Pi,
  making use of the screen command to make it a persistent process.

Please refer to screen's documentation. You'll need to know about Ctrl-A d, and other basic commands of screen. Using screen allows you to connect to the pidp from any terminal. The pdp.sh script does this for you at login.


Feedback and improvements always welcome!

oscar.vermeulen@hotmail.com

Thanks to
- Mike Barnes: port to simh 4.0
- Dylan McNamee: his buildroot version, on which this is based
- Mark G. Thomas: for making the install script
