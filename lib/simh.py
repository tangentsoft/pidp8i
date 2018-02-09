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
import subprocess
import tempfile
import time
import re

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
    self._valid_pip_options = ["/A", "/B", "/I"]
    self._os8_file_regex_str = "(\S+):(\S+)?"
    self._os8_file_re = re.compile(self._os8_file_regex_str)


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


  #### os8_get_file ####################################################
  # Rough inverse of os8_send_file.
  #
  # Both paths must be given and are used literally.  (Contrast our
  # inverse, where the destinatinon file name is produced from the
  # source if not given.)
  #
  # When this function is called to pull a file sent by our inverse, the
  # conversion should be lossless except for the transforms done by our
  # underlying utility tools, such as the LF -> CR+LF done by txt2ptp
  # but not undone by ptp2txt.
  #
  # Entry context should be inside OS/8.  Exit context is inside OS/8.

  def os8_get_file (self, intname, extname):
    # Attach a blank paper tape to the simulator.
    ptf = tempfile.NamedTemporaryFile (suffix = '.pt', delete = False)
    ptf.close ()
    ptn = ptf.name
    self.back_to_cmd ('\\.')
    self.send_cmd ('attach ptp ' + ptn)

    # Punch internal file to external paper tape image
    self.os8_restart ()
    self.os8_send_cmd ('\\.', 'PUNCH ' + intname);
    self.back_to_cmd ('\\.')        # wait for transfer to finish

    # Convert text file from SIMH paper tape format
    tool = os.path.join (pidp8i.dirs.build, 'bin', 'ptp2txt')
    self.send_cmd ('detach ptp')
    subprocess.call (tool + ' < ' +  ptn + ' > ' + extname, shell = True)

    # Return to OS/8, just because that's where we were on entry, so we
    # should not change that.
    self.os8_restart ()


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


  #### mk_os8_name # ###################################################
  # Create an OS/8 filename: of the form XXXXXX.YY
  # From a POSIX path.
  def mk_os8_name(self, dev, path):
    bns = os.path.basename (path)
    bns = re.sub("-|:|\(|\)|!", "", bns)
    bns = bns.upper()
    if "." not in bns:
      return dev + bns[:min(6, len(bns))]
    else:
      dot = bns.index('.')
      return dev + bns[:min(6, dot, len(bns))] + "." + bns[dot+1: dot+3]
    

  #### os8_send_file ###################################################
  # Send a copy of a local text file to OS/8.  The local path may
  # contain directory components, but the remote must not, of course.
  #
  # If the destination file name is not uppercase, it will be so forced.
  #
  # If the destination file name is not given, it is taken as the
  # basename of the source file name.
  #
  # The file is sent via the SIMH paper tape device through PIP in its
  # default ASCII mode, rather than character by character for two reasons:
  #
  # 1. It's faster.  It runs as fast as the simulator can process the
  #    I/O instructions, without any os8_kbd_delay() hooey.
  #
  # 2. It allows lowercase input regardless of the way the simulator is
  #    configured.  ASCII is ASCII.
  #
  # Entry context should be inside OS/8.  Exit context is inside OS/8.

  def os8_send_file (self, source, dest = None):
    # Create path and file names not given
    bns = os.path.basename (source)
    if dest == None: dest = bns
    dest = dest.upper ()

    # Convert text file to SIMH paper tape format
    bdir = pidp8i.dirs.build
    pt   = os.path.join (bdir, 'obj', bns + '.pt')
    tool = os.path.join (bdir, 'bin', 'txt2ptp')
    subprocess.call (tool + ' < ' + source + ' > ' + pt, shell = True)

    # Paper tape created, so attach it read-only and copy it in.  We're
    # relying on txt2ptp to insert the Ctrl-Z EOF marker at the end of
    # the file.
    self.back_to_cmd ('\\.')
    self.send_cmd ('attach -r ptr ' + pt)
    self.os8_restart ()
    self.os8_send_cmd ('\\.', 'R PIP')
    self.os8_send_cmd ('\\*', dest + '<PTR:')
    self._child.expect ('\\^')
    self.os8_send_ctrl ('[')      # finish transfer
    self._child.expect ('\\*')
    self.os8_send_ctrl ('[')      # exit PIP


  #### os8_send_pip ###################################################
  # Send a copy of a local file to OS/8 using PIP.
  #
  # The file is sent via the SIMH paper tape device through PIP
  # specifying a transfer option.  If no option is specified,
  # ASCII is assumed.
  #
  # In ASCII mode, we pre-process with txt2ptp which translates
  # POSIX ASCII conventions to OS/8 conventions.  In all other
  # modes, we do not do any translation.
  #
  # However, we should supply a sacrificial NULL as an additional character
  # because the OS/8 PTR driver throws the last character away. (NOT DONE YET)
  #
  # Entry context should be inside OS/8.  Exit context is inside OS/8.

  def os8_send_pip (self, path, os8name, option = None):
    # If os8name is just a device, synthesize an upcased name from
    # the POSIX file basename.
    print 
    m = re.match(self._os8_file_re, os8name)
    print m.group(2)
    if m != None and (m.group(2) == None or m.group(2) == ""):
        dest = self.mk_os8_name(os8name, path)
    else:
        dest = os8name

    if option == "" or option == "/A":
      # Convert text file to SIMH paper tape format in current dir of path.
      bdir = pidp8i.dirs.build
      pt   = path + ".pt_temp"
      tool = os.path.join (bdir, 'bin', 'txt2ptp')
      subprocess.call (tool + ' < ' + path + ' > ' + pt, shell = True)
    elif option not in self._valid_pip_options:
      print "Invalid PIP option: " + option + ". Ignoring send of: " + path
      return
    else:
      pt = path

    # Sacrificial extra character code goes here.

    # Paper tape created, so attach it read-only and copy it in.  We're
    # relying on txt2ptp to insert the Ctrl-Z EOF marker at the end of
    # the file.
    self.back_to_cmd ('\\.')
    self.send_cmd ('attach -r ptr ' + pt)
    self.os8_restart ()
    self.os8_send_cmd ('\\.', 'R PIP')
    self.os8_send_cmd ('\\*', dest + '<PTR:' + option)
    # Error detection goes here.
    pip_replies = ['\\^', ".+DIRECTORY I/O ERROR", "ILLEGAL SYNTAX"]
    reply = self._child.expect (pip_replies)
    print "reply: " + str(reply)
    if reply !=0:
      print "Error from PIP"
      return
    self.os8_send_ctrl ('[')      # finish transfer
    self._child.expect ('\\*')
    self.os8_send_ctrl ('[')      # exit PIP
    # We could detach ptr and restart OS/8 here, but we don't need to.
    # Do remove the temp file we created.
    os.remove (pt)

  #### os8_fetch_pip ###################################################
  # Fetch a file from OS/8 to a local path using PIP.
  #
  # The OS/8 source filename is synthesized from the basename of the path,
  # upcasing if necessary.
  #
  # The file is sent via the SIMH paper tape device through PIP
  # specifying a transfer option.  If no option is specified,
  # ASCII is assumed.
  #
  # In ASCII mode, we post-process with ptp2txt which translates
  # POSIX ASCII conventions to OS/8 conventions.  In all other
  # modes, we do not do any translation.
  #
  # Entry context should be inside OS/8.  Exit context is inside OS/8.

  def os8_fetch_pip (self, path, os8name, option = None):
    # Create path and file names not given
    bns = os.path.basename (path)
    source = bns.upper ()

    if option != "" and option not in self._valid_pip_options:
      print "Invalid PIP option: " + option + ". Ignoring fetch of: " + path
      return

    self.back_to_cmd ('\\.')
    self.send_cmd ('attach ptp ' + path)
    self.os8_restart ()
    self.os8_send_cmd ('\\.', 'R PIP')
    self.os8_send_cmd ('\\*', 'PTP:<' + dev + source + option)
    err_str = dev + source + " NOT FOUND"
    index = self._child.expect ([err_str, '\\*'])
    self.os8_send_ctrl ('[')      # exit PIP
    self.back_to_cmd ('\\.')
    self.send_cmd ('detach ptp')  # Clean flush of buffers.
    self.os8_restart ()

    if index == 0:                # transfer failed. Remove empty file.
      print "Transfer failed: " + err_str
      os.remove (path)
      return                        # and return.

    if option == "" or option == "/A":
      print "Format converting " + path
      # Convert text file to SIMH paper tape format
      bdir = pidp8i.dirs.build
      os.rename(path, path + ".temp")
      tool = os.path.join (bdir, 'bin', 'ptp2txt')
      subprocess.call (tool + ' < ' + path + ".temp" + ' > ' + path, shell = True)
      os.remove(path + ".temp")

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


  #### os8_zero_core ###################################################
  # Starting from OS/8 context, bounce out to SIMH context and zero all
  # of core excepting:
  #
  # 0. zero page - many apps put temporary data here
  # 1. the top pages of fields 1 & 2 - OS/8 is resident here
  # 2. the top page of field 2 - OS/8's TD8E driver (if any) lives here
  #
  # We then restart OS/8, which means we absolutely need to do #1 and
  # may need to do #2.  We could probably get away with zeroing page 0.
  #
  # All of the above explains why we have this special OS/8 alternative
  # to the zero_core() method.

  def os8_zero_core (self):
    self.back_to_cmd ('\\.')
    self.send_cmd ('de 00200-07577 0')
    self.send_cmd ('de 10000-17577 0')
    self.send_cmd ('de 20000-27577 0')
    self.send_cmd ('de 30000-77777 0')
    self.os8_restart ()


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


  #### zero_core #######################################################
  # From SIMH context, zero the entire contents of core, which is
  # assumed to be 32 kWords.
  #
  # SIMH's PDP-8 simulator doesn't start with core zeroed, on purpose,
  # because the actual hardware did not do that.  SIMH does not attempt
  # to simulate the persistence of core memory by saving it to disk
  # between runs, but the SIMH developers are right to refuse to do this
  # by default: you cannot trust the prior state of a PDP-8's core
  # memory before initializing it yourself.
  #
  # See os8_zero_core () for a less heavy-handed alternative for use
  # when running under OS/8.

  def zero_core (self):
    self.send_cmd ('de all 0')
