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

  # Known OS/8 error strings and a flag indicating whether the error
  # dumps us back out to the OS/8 command monitor or leaves us in the
  # called program.
  #
  # This is currently used only by our os8_pip_into() method, but we
  # should probably also write wrappers for other OS/8 commands prone
  # to throwing errors.
  #
  _os8_errors = [
    # The date comment tells when each message is observed and validated
    #
    # OS/8 Handbook 1974 page 1-43/81 Keyboard Monitor Error Messages:
    ["MONITOR ERROR 2 AT \d+ \\(DIRECTORY I/O ERROR\\)", True],   # 2018.02.11
    ["MONITOR ERROR 5 AT \d+ \\(I/O ERROR ON SYS\\)", True],
    ["MONITOR ERROR 6 AT \d+ \\(DIRECTORY I/O ERROR\\)", True],
    ["(\S+) NOT AVAILABLE", False],
    ["(\S+) NOT FOUND", False],                                   # 2018.02.11
    # OS/8 Handbook 1974 page 1-51/89 Command Decoder Error Messages
    ["ILLEGAL SYNTAX", False],                                    # 2018.02.11
    ["(\S+) DOES NOT EXIST", False],
    # ["(\S+) NOT FOUND", False],                                 # See above
    ["TOO MANY FILES", False],
    # OS/8 Handbook 1974 page 1-75/113 CCL Error Messages
    ["BAD DEVICE", False],
    ["BAD EXTENSION", False],
    # OS/8 Handbook 1974 page 1-106/144 PIP Error Messages
    ["ARE YOU SURE", False],
    ["BAD DIRECTORY ON DEVICE #\s?\d+", False],
    ["BAD SYSTEM HEAD", False],
    ["CAN'T OPEN OUTPUT FILE", False],
    ["DEVICE #\d+ NOT A DIRECTORY DEVICE", False],
    ["DIRECTORY ERROR", False],
    ["ERROR DELETING FILE", False],
    ["ILLEGIAL BINARY INPUT, FILE #\d+", False],
    ["INPUT ERROR, FILE #\s?\d+", False],
    ["IO ERROR IN \\(file name\\) --CONTINUING", False],
    ["NO ROOM FOR OUTPUT FILE", False],
    ["NO ROOM IN \\(file name\\) --CONTINUING", False],
    ["OUTPUT ERROR", False],
    ["PREMATURE END OF FILE, FILE #\s?\d+", False],
    ["ZERO SYS?", False],
    # OS/8 Handbook 1974 page 2-81/244: DIRECT Error Messages
    ["BAD INPUT DIRECTORY", False],
    ["DEVICE DOES NOT HAVE A DIRECTORY", False],
    ["ERROR CLOSING FILE", False],
    ["ERROR CLOSING FILE", False],
    ["ERROR READING INPUT DIRECTORY", False],
    ["ILLEGAL \\*", False],
    # OS/8 Handbook 1974 page: 2-109/272: FOTP Error Messages
    ["ERROR ON INPUT DEVICE, SKIPPING \\((\S+)\\)", False],
    ["ERROR ON OUTPUT DEVICE, SKIPPING \\((\S+)\\)", False],
    ["ERROR READING INPUT DIRECTORY", False],
    ["ERROR READING OUTPUT DIRECTORY", False],
    ["ILLEGAL \\?", False],
    ["NO FILES OF THE FORM (\S+)", False],
    ["NO ROOM, SKIPPING \\((\S+)\\)", False],
    ["SYSTEM ERROR-CLOSING FILE", False],
    ["USE PIP FOR NON-FILE STRUCTURED DEVICE", False],
  ]

  # Pattern to match a SIMH command.  The command verb ends up in
  # match().group(1), and anything after the verb in group(3).
  _simh_comm_re = re.compile ("^\s*(\S+)(\s+(.*))?$")

  # Significant prefixes of SIMH command verbs that transition from SIMH
  # command context back into the simulation: BOOT, CONTINUE, and GO.
  # We need only the first letter in all cases, since these particular
  # commands are not ambiguous.  They're uppercase because the code that
  # uses this always uppercases the command before searching this list.
  _enters_os8_context = ["B", "C", "G"]


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
    self._os8_file_re = re.compile("(\S+):(\S+)?")
    self._os8_error_match_strings = []
    self._os8_fatal_check = []

    # We keep track of what our command context is so our caller does
    # not need to explicitly call back_to_cmd() or sendcontrol ('e').
    # We keep track of the command context and transition automatically.
    self._context = "simh"
    
    # Parse our OS/8 Errors table into actionable chunks
    for error_spec in self._os8_errors:
      self._os8_error_match_strings.append(error_spec[0])
      self._os8_fatal_check.append(error_spec[1])

    self._pip_into_replies = ['\\^']
    self._pip_into_replies.extend(self._os8_error_match_strings)
    # Did command start the command decoder or die with a monitor error?
    self._cd_replies = ['\\*']
    self._cd_replies.extend(self._os8_error_match_strings)

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
    self._context = "simh"


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
  #
  # A previous version of this routine attempted to detect when
  # OS/8 was suspended, and we were in the SIMH context.
  # Unfortunately, re-synchronizing OS/8 with python expect requires
  # provoking an additional prompt.
  #
  # Although it would be expected that sending a character delete
  # or line delete would provoke a prompt, this does not work.
  # At the Keyboard Monitor command level, pausing after sending
  # the SIMH 'continue' command, then sending ^C, then pausing again
  # then sending \n\r does provoke a command prompt.
  # However, if you had escaped to SIMH from inside another program,
  # that command sequence would kill your program and return you to
  # the OS/8 keyboard monitor.
  #
  # Therefore, os8_send_cmd requires you to be running OS/8, and does
  # not presume to try and get you there from SIMH.

  def os8_send_cmd (self, prompt, line, debug=False):
    if self._context != 'os8': 
      print "OS/8 is not running. Cannot execute: " + line 
      return
    if debug:
      print "os8_send_cmd: expecting: " + prompt
      print "\tLast match before: {" + self._child.before + "}"
      print "\tLast match after: {" + self._child.after + "}"
    self._child.expect ("\n%s$" % prompt)
    self.os8_send_line (line)


  #### os8_send_ctrl ###################################################
  # Send a control character to OS/8 corresponding to the ASCII letter
  # given.  We precede it with the OS/8 keyboard delay, since we're
  # probably following a call to os8_send_line or os8_send_cmd.

  def os8_send_ctrl (self, char):
    cc = char[0].lower ()
    self.os8_kbd_delay ()
    self._child.sendcontrol (cc)
    
    if cc == 'e': self._context = 'simh'


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


  #### pip_error_handler ###############################################
  # Common error handler for os8_pip_into and os8_pip_from

  def pip_error_handler(self, caller, reply):
    print "PIP error from inside " + caller + ": "
    print "\t" + self._child.before.strip()
    print "\t" + self._child.after.strip()
    
    # Was this error fatal or do we need to clean up?
    # Remember we subtract 1 from reply to get index into error tables.
    if not self._os8_fatal_check[reply - 1]:
      # Non fatal error.  Exit pip to the monitor
      self.os8_send_ctrl ('[')      # exit PIP

    
  #### os8_pip_into ###################################################
  # Send a copy of a local file into OS/8 using PIP.
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

  def os8_pip_into (self, path, os8name, option = None):
    if option == None: option = ""
    # If os8name is just a device, synthesize an upcased name from
    # the POSIX file basename.
    if not os.path.exists(path):
      print path + " not found. Skipping."
      return
    m = re.match(self._os8_file_re, os8name)
    if m != None and (m.group(2) == None or m.group(2) == ""):
        dest = self.mk_os8_name(os8name, path)
    else:
        dest = os8name

    did_conversion = False
    if option == "" or option == "/A":
      # Convert text file to SIMH paper tape format in current dir of path.
      print "Format converting " + path
      bdir = pidp8i.dirs.build
      pt   = path + ".pt_temp"
      tool = os.path.join (bdir, 'bin', 'txt2ptp')
      subprocess.call (tool + ' < ' + path + ' > ' + pt, shell = True)
      did_conversion = True
    elif option not in self._valid_pip_options:
      print "Invalid PIP option: " + option + ". Ignoring: " + path + " into OS/8."
      return
    else:
      pt = path

    # TODO: Sacrificial extra character code goes here.

    # Paper tape created, so attach it read-only and copy it in.  We're
    # relying on txt2ptp to insert the Ctrl-Z EOF marker at the end of
    # the file.
    self.back_to_cmd ('\\.')
    self.send_cmd ('attach -r ptr ' + pt)
    self.os8_restart ()
    self.os8_send_cmd ('\\.', 'R PIP')
    # Was the start of PIP successful, or did we get a Monitor error?
    reply = self._child.expect (self._cd_replies)
    if reply != 0:
      self.pip_error_handler ("os8_pip_into", reply)
      return

    # Has the read-in been successful?
    self.os8_send_line (dest + '<PTR:' + option)
    reply = self._child.expect (self._pip_into_replies)
    if reply !=0:
      self.pip_error_handler("os8_pip_into", reply)
      if did_conversion:
        os.remove(pt)
      return

    self.os8_send_ctrl ('[')      # finish transfer
    self._child.expect ('\\*')
    self.os8_send_ctrl ('[')      # exit PIP
    # We could detach ptr and restart OS/8 here, but we don't need to.
    # Do remove the temp file if we created one.
    if did_conversion:
      os.remove (pt)


  #### os8_pip_from ###################################################
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

  def os8_pip_from (self, os8name, path, option = None):
    if option == None: option = ""
    # If path is not a file, use the name portion of os8name.
    if os.path.isdir(path):
      colon = os8name.find(':')
      if colon == -1:                # No dev, just a name.
        path = path + "/" + os8name
      else:
        path = path + "/" + os8name[colon+1:]

    if option != "" and option not in self._valid_pip_options:
      print "Invalid PIP option: " + option + ". Ignoring os8_pip_from on: " + path
      return

    self.back_to_cmd ('\\.')
    self.send_cmd ('attach ptp ' + path)
    self.os8_restart ()
    
    self.os8_send_cmd ('\\.', 'R PIP')
    # Was the start of PIP successful or did we get a Monitor error?
    reply = self._child.expect (self._cd_replies)
    if reply != 0:
      self.pip_error_handler ("os8_pip_from", reply)
      return
    
    self.os8_send_line ('PTP:<' + os8name + option)

    reply = self._child.expect (self._cd_replies)
    if reply !=0:
      self.pip_error_handler ("os8_pip_from", reply)
      # There is an empty PTP file we need to remove.
      os.remove(path)
      return

    self.os8_send_ctrl ('[')      # exit PIP
    self.back_to_cmd ('\\.')
    self.send_cmd ('detach ptp')  # Clean flush of buffers.
    self.os8_restart ()

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


  #### os8_resume #######################################################
  # Resume OS/8.
  #
  # It would be nice if we could just send  the "cont" command
  # and have python expect and OS/8 synch right up.
  # But so far we have not figured out how to do that.
  # To resume OS/8 from SIMH we need to provoke a prompt.
  # Typing a rubout or ^U at a SIMH terminal session does this.
  # But not when SIMH is run under python expect.
  # We don't know why.
  #
  # boot works
  # go 7600 works
  # ^C <pause> \n\r works.
  #
  # The resume command uses the ^C method as the least disruptive
  # to system state.

  def os8_resume (self):
    if self._context == "os8": return   # Already running.
    
    self.send_cmd("cont")   # sets os8 context for us.

    # Now provoke a keyboard monitor prompt.
    self.os8_send_ctrl('c')
    self.os8_kbd_delay()
    self.os8_send_str('\r\n')
  

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
  # Wait for a SIMH command prompt and then send the given command.
  # If we are not in the simh context send ^e and set context "simh".
  # If we are not in simh context, send ^e set context "simh"
  #    and hope for the best.
  # If we issue a command that enters os8 context, set context "os8".
  # Note exiting out of OS/8 into the SIMH context is a bit of a
  # trap door. Resynchronizing with python expect requires provoking
  # a prompt, and prompts are context specific.
  # Perhaps we should require separate and explicit commands to
  # escape to SIMH. But for now, just be careful to use os8_resume
  # after calling send_cmd.
  
  def send_cmd (self, cmd):
    if self._context == "os8":
      self._child.expect ("\n\\.$")
      self._child.sendcontrol ('e')
      self._context = "simh"
    elif self._context != "simh":
      self._child.sendcontrol ('e')
      self._context = "simh"
      
    self._child.expect ("sim> $")
    self._child.sendline (cmd)
    m = re.match (self._simh_comm_re, cmd)
    if m != None and m.group(1)[:1].upper() in self._enters_os8_context:
      self._context = "os8"


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
  

  #### describe_dev_config #############################################
  # We provide an interface to alter SIMH device configurations for
  # specific parameters and specific devices
  #
  # dev configs supported:  rx, tti, tape
  #
  # rx:     RX8E, RX28   RX8E is the simh name for RX01 support.
  #                      RX28 is the simh name for RX02 support.
  # tti:    KSR, 7b      7b is full keyboard support.
  #                      KSR forces upcase of lower case keys on input.
  # tape:   td, dt       td is the TD8E DECtape device
  #                      dt is the TC08 DECtape device

  def describe_dev_config (self, name):
    if name == "tape":
      lines = self.do_simh_show("dt")
      dev_status = self.parse_show_tape_dev(lines)

      if dev_status == "dt": return "dt"
      else:
        lines = self.do_simh_show("td")
        return self.parse_show_tape_dev(lines)
    elif name == "rx":
      lines = self.do_simh_show("rx")
      return self.parse_show_rx_dev(lines)
    elif name == "tti":
      lines = self.do_simh_show("tti")
      return self.parse_show_tti(lines)
    else: return None

      
  #### do_simh_show  ###################################################
  # Calls show on the device name.
  # Returns array of lines from output.

  def do_simh_show (self, name):
    supported_shows = ["dt", "td", "tti", "rx"]
    if name not in supported_shows: return None
    
    ucname = name.upper()
    self.send_cmd("show " + name)
    self._child.expect(ucname + "\s+(.+)\r")
    lines = self._child.after.split ("\r")
    return lines


  #### parse_show_tape_dev  ############################################
  # Returns current state of DECtape support.
  # One of: disabled, td, dt, or None if parse fails.

  def parse_show_tape_dev (self, lines):
    if lines == None: return None
    is_enabled_re = re.compile("^(TD|DT)\s+(disabled|(devno=\S+,\s(\d)\s+units))$")
    m = re.match(is_enabled_re, lines[0])

    if m == None or m.group(1) == None or m.group(2) == None: return None
    if m.group(2) == "disabled": return "disabled"
    elif m.group(1) == "TD": return "td"
    elif m.group(1) == "DT": return "dt"
    else: return None


  #### parse_show_tape_attached  ########################################
  # Returns an ordered list of files attached or None if disabled.
  def parse_show_tape_attached (self, lines):
    if lines == None: return None
    if len(lines) < 2: return None
    attached = {}
    attachment_re = re.compile("^\s+(((DT|TD)(\d)(.+),\s+(not\s+attached|attached\s+to\s+(\S+)),(.+))|12b)$")
    for line in lines[1:]:
      m = re.match(attachment_re, line)
      if m == None or m.group(1) == None or m.group(1) == "12b": continue
      filename = m.group(7)
      if filename == None: filename = ""
      attached[m.group(4)] = filename
    return attached

  #### do_print_lines ###################################################
  # Debugging aid. Prints what we parsed out of _child.after.
  def do_print_lines (self, lines):
    for line in lines:
      print line

  #### simh_configure routines #########################################
  # These routines affect the state of device configuration in SIMH.
  # They are intended as robust ways to toggle between incompatible
  # configurations of SIMH:
  # Choice of TD8E or TC08 DECtape (SIMH td and dt devices).
  # Choice of RX01 or RX02 Floppy emulation.
  # The SIMH rx device sets RX8E for RX01, and RX28 for RX02.  
  # Choice of KSR or 7bit console configuration.
  #
  # When re-configuring dt, dt, and rx devices, any attached
  # images are detached before reconfiguration is attempted.
  # (SIMH errors out if you don't detach them.)
  #
  # The check to see if the change is unnecessary.
  # For now they return None if no change necessary.
  #
  # After re-configuring the device, the SIMH show command is used
  # to confirm the re-configuration was successful.
  #
  # In future, we should add exception handling for no change necessary.
  # For now, return True if the change was successful and False if not.


  def set_tape_config (self, to_tape):
    if to_tape == "dt": from_tape = "td"
    elif to_tape == "td": from_tape = "dt"
    else:
      print "Cannot set_tape_config for " + to_tape
      return False

    print "Disable: " + from_tape + ", and enable: " + to_tape
    
    lines = self.do_simh_show(from_tape)
    from_status = self.parse_show_tape_dev(lines)

    if from_status == None:
      print "do_tape_change: Trouble parsing \'show " + from_tape + "\' output from simh. Giving up on:"
      self.do_print_lines (lines)
      return False

    if from_status == "disabled": print from_tape + " already is disabled."
    else:
      attached_from = self.parse_show_tape_attached(lines)
      if attached_from == None:
        print "do_tape_change: Trouble parsing \'show " + from_tape + "\' output from simh. Giving up on:"
        self.do_print_lines (lines)
        return False
      else:
        for unit in attached_from.keys():
          if attached_from[unit] != "":
            det_comm = "det " + from_tape + unit
            # print det_comm + "(Had: " + attached_from[unit] + ")"
            self.send_cmd(det_comm)
        self.send_cmd("set " + from_tape + " disabled")

    lines = self.do_simh_show(to_tape)
    to_status = self.parse_show_tape_dev(lines)

    if to_status == None:
      print "do_tape_change: Trouble parsing \'show " + to_tape + "\' output from simh. Giving up on:"
      self.do_print_lines (lines)
      return False
    elif to_status != "disabled": print to_tape + " already is enabled."
    else:
      self.send_cmd("set " + to_tape + " enabled")    

    # Test to confirm to_tape is now enabled.

    lines = self.do_simh_show(to_tape)
    to_status = self.parse_show_tape_dev(lines)

    if to_status == None:
      print "Failed enable of " + to_tape + ". Parse fail on \'show " + to_tape + "\'. Got:"
      self.do_print_lines (lines)
      return False
    elif to_status == "disabled":
      print "Failed enable of " + to_tape + ". Device still disabled."
      return False
    else: 
      return True


  #### parse_show_rx_dev ###############################################
  # Show the rx device configuration.

  def parse_show_rx_dev (self, lines):
    if lines == None: return None
    is_enabled_re = re.compile("^\s*(RX)\s+(disabled|((RX8E|RX28),\s+devno=\S+,\s+(\d)\s+units))$")
    m = re.match(is_enabled_re, lines[0])
    if m == None or m.group(2) == None: return None
    if m.group(2) == "disabled": return "disabled"
    return m.group(4)


   #### parse_show_rx_attached ##########################################
  # Returns an ordered list of files attached or None if disabled.

  def parse_show_rx_attached (self, lines):
    if len(lines) < 2: return None
    attached = {}
    attachment_re = re.compile("^\s+(((RX)(\d)(.+),\s+(not\s+attached|attached\s+to\s+(\S+)),(.+))|autosize)$")
    for line in lines[1:]:
      m = re.match(attachment_re, line)
      if m == None or m.group(1) == None or m.group(1) == "autosize": continue
      filename = m.group(7)
      if filename == None: filename = ""
      attached[m.group(4)] = filename
    return attached


  #### set_rx_config ####################################################
  
  def set_rx_config (self, to_rx):
    to_rx = to_rx.lower()
    if to_rx == "rx8e": from_rx = "rx28"
    elif to_rx == "rx01":
      to_rx = "rx8e"
      from_rx = "rx28"
    elif to_rx == "rx28": from_rx = "rx8e"
    elif to_rx == "rx02":
      to_rx = "rx28"
      from_rx = "rx8e"
    else:
      print "Cannot set_rx_config for " + to_rx
      return False
      
    print "Switch rx driver: " + from_rx + ", to: " + to_rx
    lines = self.do_simh_show("rx")

    rx_type = self.parse_show_rx_dev (lines)
    if rx_type == None:
      print "do_rx_change: Trouble parsing \'show rx\' output from simh. Giving up on:"
      self.do_print_lines(lines)
      return False
    elif rx_type == "disabled":
      print "rx is disabled. Enabling..."
      self.send_cmd("set rx enabled")
      # Retry getting rx info
      lines = self.do_simh_show("rx")
      rx_type = self.parse_show_rx_dev (lines)
      if rx_type == None:
        print "do_rx_change after re-enable: Trouble parsing \`show rx\` output from simh. Giving up on:"
        self.do_print_lines(lines)
        return False
      elif rx_type == "disabled":
        print "do_rx_change re-enable of rx failed. Giving up."
        return False

    if rx_type.lower() == to_rx:
      print "rx device is already set to " + to_rx
      return None
      
    attached_rx= self.parse_show_rx_attached(lines)
    if attached_rx == None:
      print "do_rx_change: Trouble parsing /'show rx\' from simh to find rx attachments. Got:"
      self.do_print_lines(lines)
    else:
      for unit in attached_rx.keys():
        if attached_rx[unit] != "":
          det_comm = "det rx" + unit
          # print det_comm + "(Had: " + attached_rx[unit] + ")"
          self.send_cmd(det_comm)

    self.send_cmd("set rx " + to_rx)

    # Test to confirm new setting of RX
    lines = self.do_simh_show("rx")
    rx_type = self.parse_show_rx_dev (lines)

    if rx_type == None:
      print "Failed change of rx to " + to_rx + ". Parse fail on \'show rx\'."
      return False
    elif rx_type.lower() != to_rx: 
      print "Failed change of rx to " + to_rx + ". Instead got: " + rx_type
      return False
    return True


  #### get_tti ##################################################
  # Returns an ordered list of files attached or None if disabled.
  def parse_show_tti (self, lines):
    if lines == None: return None
    is_enabled_re = re.compile("^(KSR|7b)$")
    if len(lines) < 2: return None

    # That second line of output contains embedded newlines.
    m = re.match(is_enabled_re, lines[1].strip())
    if m == None or m.group(1) == None: return None
    return m.group(1)



  #### do_tti_change ###################################################

  def set_tti_config (self, to_tti):
    if to_tti == "KSR": from_tti = "7b"
    elif to_tti == "7b": from_tti = "KSR"
    else:
      print "Cannot set_tti_config to " + to_tti
      return
    
    print "Switch tti driver from: " + from_tti + ", to: " + to_tti

    lines = self.do_simh_show("tti")
    tti_type = self.parse_show_tti (lines)
    if tti_type == None:
      print "do_tti_change: Trouble parsing \'show tti\' output from simh. Giving up on:"
      self.do_print_lines(lines)
      return False
    elif tti_type == to_tti:
      print "tti device is already set to " + to_tti
      return None

    self.send_cmd("set tti " + to_tti)

    # Test to confirm new setting of tti
    lines = self.do_simh_show("tti")
    tti_type = self.parse_show_tti (lines)
    
    if tti_type == None:
      print "Failed change of tti to " + to_tti + ". Parse fail on \'show tti\'."
      return False
    elif tti_type != to_tti: 
      print "Failed change of tti to " + to_tti + ". Instead got: " + tti_type
      return False
    else:
      return True


