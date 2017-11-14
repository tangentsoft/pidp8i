#!/usr/bin/python
# -*- coding: utf-8 -*-
########################################################################
# simh/__init__.py - A wrapper class around pexpect for communicating
#   with an instance of the PiDP-8/I SIMH simulator running OS/8.
#
#   See ../doc/class-simh.md for a usage tutorial.
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

import os.path
import pexpect
import pkg_resources
import time

import pidp8i

class simh: 
  # pexpect object instance, set by ctor
  _child = None

  # Constant used by os8_kbd_delay, assembled in stages:
  #
  # 1. PDP-8 RS-232 bits per character: 7-bit ASCII plus necessary
  #    start, stop, and parity bits.
  #
  # 2. The ratio of the instructions per second ratios of a PDP-8/I to
  #    that of the host hardware running the simulator.  The former is
  #    an approximate value; see lib/pidp8i/ips.py.in for the value and
  #    its defense.  The latter is either the IPS rate for:
  #
  #    a) a Raspberry Pi Model B+, that being the slowest host system
  #       we run this simulator on; or
  #
  #    b) the IPS rate of the actual host hardware if you have run the
  #       "bin/teco-pi-demo -b" benchmark on it.
  #
  # 2. The fact that real PDP-8s ran OS/8 reliably at 300 bps, and have
  #    been claimed to get flaky as early as 600 bps by some.  (Others
  #    claim to have run them up to 9,600 bps.)
  #
  # 3. The "safe BPS" value is the fastest bit per second speed actual
  #    PDP-8 hardware was known to run OS/8 terminal I/O at.  In this
  #    case, it is the high-speed tape reader.
  #
  #    TODO: We may be able to increase this.
  #
  #    We have one report that OS/8 was tested with terminals up to
  #    about ~600 bps before becoming unreliable.
  #
  #    We have another report that OS/8 could run under ETOS with
  #    9,600 bps terminals, but we don't know if that tells us anything
  #    about OS/8 running without the ETOS multitasking hardware.
  #
  # 4. Given above, calculate safe characters per second for host HW.
  #
  # 5. Invert to get seconds per character, that being the delay value.
  _bpc = 7 + 1 + 1 + 1                                       # [1]
  _ips_ratio = float (pidp8i.ips.current) / pidp8i.ips.pdp8i # [2]
  _pdp8i_safe_bps = 300                                      # [3]
  _host_safe_cps = _pdp8i_safe_bps * _ips_ratio / _bpc       # [4]
  _os8_kbd_delay = 1 / _host_safe_cps                        # [5]


  #### ctor ############################################################
  # The first parameter must be given as the parent of bin/pidp8i-sim.
  #
  # The second defaults to false, meaning that a failure to lock the
  # GPIO for the caller's exclusive use is a fatal error.  If you pass
  # True instead, we just complain if the GPIO is already locked and
  # move on.  This tolerant mode is appropriate for programs that need
  # the simulator alone, not actually the PiDP-8/I front panel display.
  
  def __init__ (self, basedir, ignore_gpio_lock = False):
    # Start the simulator instance
    self._child = pexpect.spawn(basedir + '/bin/pidp8i-sim')

    # Turn off pexpect's default inter-send() delay.  We add our own as
    # necessary.  The conditional tracks an API change between 3 and 4.
    pev4 = (pkg_resources.get_distribution("pexpect").parsed_version >
            pkg_resources.parse_version("4.0"))
    self._child.delaybeforesend = None if pev4 else 0

    # Wait for either an error or the simulator's configuration line.
    if not self.try_wait (\
        '^PiDP-8/I [a-z].*\[.*\]', \
        'Failed to lock /dev/gpiomem', timeout = 3):
      if ignore_gpio_lock:
        print "WARNING: Failed to lock GPIO for exclusive use.  Won't use front panel."
      else:
        raise RuntimeError ('GPIO locked')


  #### back_to_cmd ######################################################
  # Pause the simulation and return to the SIMH command prompt when the
  # simulated software emits the given prompt string.  Typically used to
  # wait for OS/8 to finish running a command so we can do something
  # down at the SIMH layer instead.

  def back_to_cmd (self, prompt):
    self._child.expect ("\n%s$" % prompt)
    self.os8_kbd_delay ()
    self._child.sendcontrol ('e')


  #### os8_kbd_delay ###################################################
  # Artificially delay the media generation process to account for the
  # fact that OS/8 lacks a modern multi-character keyboard input buffer.
  # It is unsafe to send text faster than a contemporary terminal could,
  # though we can scale it based on how much faster this host is than a
  # real PDP-8.  See the constants above for the calculation.
  def os8_kbd_delay (self):
    time.sleep (self._os8_kbd_delay)


  #### os8_send_cmd ####################################################
  # Wait for an OS/8 command prompt running within SIMH, then send the
  # given line.
  #
  # The prompt string is passed in because OS/8 has several different
  # prompt types.

  def os8_send_cmd (self, prompt, line):
    self._child.expect ("\n%s$" % prompt)
    self.os8_send_line (line)


  #### os8_send_ctrl ###################################################
  # Send a control character to OS/8 corresponding to the ASCII letter
  # given.  We precede it with the OS/8 keyboard delay, since we're
  # probably following a call to os8_send_line or os8_send_cmd.

  def os8_send_ctrl (self, char):
    self.os8_kbd_delay ()
    self._child.sendcontrol (char[0].lower ())


  #### os8_send_file ###################################################
  # Send a copy of a local text file to OS/8.  The local path may
  # contain directory components, but the remote must not, of course.
  #
  # If the destination file name is not uppercase, it will be so forced.
  #
  # If the destination file name is not given, it is taken as the
  # basename of the source file name.

  def os8_send_file (self, source, dest = None):
    if dest == None: dest = os.path.basename (source)

    self.os8_send_cmd ('\.', 'CREATE ' + dest.upper())
    self.os8_send_cmd ("#", "A")        # append text to file
    with open (source, 'r') as f:
      for line in f:
        self.os8_send_line(line)
    self.os8_send_ctrl ('L')
    self.os8_send_cmd ("#", "E")        # save and exit


  #### os8_send_line ###################################################
  # Core of os8_send_cmd.  Also used by code that needs to send text
  # "blind" to OS/8, without expecting a prompt, as when driving EDIT.

  def os8_send_line (self, line):
    self.os8_send_str (line)
    self._child.send ("\r")


  #### os8_send_str ########################################################
  # Core of os8_send_line.  Also used by code that needs to send text
  # "blind" to OS/8, without expecting a prompt and without a CR, as
  # when driving TECO.

  def os8_send_str (self, str):
    for i in xrange (0, len (str)):
      self._child.send (str[i])
      self.os8_kbd_delay ()


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
    self._child.expect (pexpect.EOF)


  #### read_tail #######################################################
  # Watch for a literal string, then get what follows on that line.

  def read_tail (self, head, timeout = -1):
    self._child.expect_exact ([head], timeout)
    return self._child.readline ()


  #### send_cmd ########################################################
  # Wait for a SIMH command prompt and then send the given command

  def send_cmd (self, cmd):
    self._child.expect ("sim> $")
    self._child.sendline (cmd)


  #### send_line #######################################################
  # Sends the given line "blind", without waiting for a prompt.

  def send_line (self, line):
    self._child.sendline (line)


  #### set_logfile #####################################################

  def set_logfile (self, lf):
    self._child.logfile = lf


  #### spin ############################################################
  # Let child run without asking anything more from it, with an optional
  # timeout value.  If no value is given, lets child run indefinitely.

  def spin (self, timeout = None):
    self._child.expect (pexpect.EOF, timeout = timeout)


  #### try_wait ########################################################
  # Wait for one of two strings to come back from SIMH, returning true
  # if we get the first, else false.

  def try_wait (self, success, failure, timeout = -1):
    return self._child.expect ([success, failure], timeout = timeout) == 0
