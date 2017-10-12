#!/usr/bin/python
# -*- coding: utf-8 -*-
########################################################################
# simh/__init__.py - A wrapper class around pexpect for communicating
#   with an instance of the PiDP-8/I SIMH simulator running OS/8.
#
# Copyright © 2017 by Jonathan Trites, William Cattey, and Warren Young.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS LISTED ABOVE BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
# OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
# OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the names of the authors above
# shall not be used in advertising or otherwise to promote the sale,
# use or other dealings in this Software without prior written
# authorization from those authors.
########################################################################

import pexpect
import pkg_resources
import time

class simh: 
  # pexpect object instance, set by ctor
  _child = None


  #### ctor ############################################################
  
  def __init__ (self, basedir):
    # Start the simulator instance
    self._child = pexpect.spawn(basedir + '/bin/pidp8i-sim')

    # Turn off pexpect's default inter-send() delay.  We add our own as
    # necessary.  The conditional tracks an API change between 3 and 4.
    pev4 = (pkg_resources.get_distribution("pexpect").parsed_version >
            pkg_resources.parse_version("4.0"))
    self._child.delaybeforesend = None if pev4 else 0


  #### back_to_cmd ######################################################
  # Pause the simulation and return to the SIMH command prompt when the
  # simulated software emits the given prompt string.  Typically used to
  # wait for OS/8 to finish running a command so we can do something
  # down at the SIMH layer instead.

  def back_to_cmd (self, prompt):
    self._child.expect("\n%s$" % prompt)
    self.os8_kbd_delay()
    self._child.sendcontrol('e')


  #### os8_kbd_delay ###################################################
  # Artificially delay the media generation process to account for the
  # fact that OS/8 lacks a modern multi-character keyboard input buffer.
  # It is unsafe to send text faster than a contemporary terminal could.
  #
  # The constant is based on advice we received that OS/8 would begin to
  # become unreliable when run with a terminal speaking more than about
  # 600 bps.  We divide that by 7-bit ASCII plus necessary start, stop,
  # and parity bits to get characters per second.  Then we multiply that
  # by 4, the minimum number of times faster our SIMH instance runs as
  # compared to a real PDP-8.  (Keep in mind that we initially run the
  # simulator without any throttling.)  Finally, we invert that value to
  # get secs per character instead of characters per second.  OS/8 must
  # be able to accept input at least this fast on our simulator.

  _kbd_delay = 1 / (600 / (7 + 1 + 1 + 1) * 4)
  def os8_kbd_delay (self):
    time.sleep (self._kbd_delay)


  #### os8_send_cmd ######################################################
  # Wait for an OS/8 command prompt running within SIMH, then send the
  # given line.
  #
  # The prompt string is passed in because OS/8 has several different
  # prompt types.

  def os8_send_cmd (self, prompt, send_line):
    self._child.expect ("\n%s$" % prompt)
    self.os8_send_line (send_line)


  #### os8_send_ctrl #####################################################
  # Send a control character to OS/8 corresponding to the ASCII letter
  # given.  We precede it with the OS/8 keyboard delay, since we're
  # probably following a call to os8_send_line or os8_send_cmd.

  def os8_send_ctrl (self, char):
    self.os8_kbd_delay ()
    self._child.sendcontrol (char[0].lower ())


  #### os8_send_line #####################################################
  # Core of os8_send_cmd.  Also used by code that needs to send text
  # "blind" to OS/8, without expecting a prompt, as when driving EDIT.

  def os8_send_line (self, send_line):
    for i in xrange(0, len (send_line)):
      self._child.send (send_line[i])
      self.os8_kbd_delay ()
    self._child.send ("\r")


  #### os8_restart #######################################################
  # Called while in the SIMH command prompt, this restarts OS/8.
  #
  # This one-line function exists to abstract the method we use and to
  # document the reason we do it this way.
  #
  # Currently we do this by calling the OS/8 command entry point, which
  # has the virtue that it forces another . prompt, which keeps the
  # send/expect sequencing simple when switching between OS/8 and SIMH
  # command modes.
  #
  # That is why we don't use "cont" here instead: it requires that the
  # caller always be aware of when the . prompt went out; complicated.
  #
  # Another simple alternative is "boot rk0", which actually benchmarks
  # a smidge faster here.  We choose this method instead because we
  # expect that some of our callers will want to do several different
  # things in a single OS/8 session, which rebooting would screw up.

  def os8_restart (self):
    self.send_cmd ("go 7600")


  #### os8_squish ########################################################
  # Wraps the OS/8 SQUISH command for a given device.

  def os8_squish (self, device):
    self.os8_send_cmd ("\.", "SQUISH " + device + ":")
    self.os8_kbd_delay ()
    self._child.send ("Y\r");


  #### quit ############################################################
  # Quits the simulator and waits for it to exit

  def quit (self):
    self.send_cmd ("q")
    self._child.expect(pexpect.EOF)


  #### send_cmd ########################################################
  # Wait for a SIMH command prompt and then send the given command

  def send_cmd (self, cmd):
    self._child.expect ("sim> $")
    self._child.sendline (cmd)


  #### set_logfile #####################################################

  def set_logfile (self, lf):
    self._child.logfile = lf
