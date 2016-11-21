#!/bin/sh
#
#
# INSTALL SCRIPT FOR PIDP8 ***WITH*** SERIAL MOD
#
# Run this script once to configure Raspberry Pi for operation with pidp8.
#  -Disables serial port.
#  -Installs screen utility for detached emulator operation.
#  -Installs usbmount for auto-mounting USB sticks
#
# Author: Mark G Thomas <mark@misty.com> 2015-05-09
# (modified by Oscar Vermeulen 20151215)
#
# References:  http://www.hobbytronics.co.uk/raspberry-pi-serial-port
#
PATH=/usr/sbin:/usr/bin:/sbin:/bin


# Install usbmount utility
#
test ! -f /etc/usbmount/usbmount.conf && apt-get install usbmount

# Install screen utility if not already installed.
#
test ! -x /usr/bin/screen && apt-get install screen

# Set up pidp8 init script, provided pidp8 is installed in /opt/pidp8.
#
if [ ! -x /opt/pidp8/etc/rc.pidp8 ] || [ ! -x /opt/pidp8/bin/pidp8 ]; then
	Skipping init script setup, /opt/pidp8/etc/rc.pidp8 or /opt/pidp8/bin/pidp8 not found.
else
	ln -s /opt/pidp8/etc/rc.pidp8 /etc/init.d/pidp8
	update-rc.d pidp8 defaults
fi

# set up 'pdp' in home directory
#
ln -s /opt/pidp8/etc/pdp.sh /home/pi/pdp.sh

# add pdp.sh to end of pi's .profile 
#
test ! -f /home/pi/.profile.foo && cp -p /home/pi/.profile /home/pi/.profile.foo
sed -e "\$a/home/pi/pdp.sh" -i /home/pi/.profile

