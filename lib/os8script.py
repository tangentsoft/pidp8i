#!/usr/bin/python
# -*- coding: utf-8 -*-
########################################################################
# simh-os8-script.py Library for scripting OS/8 under SIMH
# Contains validators and callers for os8 and simh commands to make
# it easier to create scripts.
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

# Bring in just the basics so we can bring in our local modules
import os
import sys
sys.path.insert (0, os.path.dirname (__file__) + '/../lib')
sys.path.insert (0, os.getcwd () + '/lib')

# Python core modules we use
import re
from string import Template
import shutil
import subprocess

# Our local modules
from pidp8i import *
from simh import *

# Error Class Definitions ##############################################
# Enables us to use exceptions from within this module.

class Error(Exception):
  """Base Class for exceptions in this module."""
  pass

class InputError(Error):
  """Exception raised for errors in the input.

  Attributes:
  expr -- input expression in which the error occurred
  msg  -- explanation of the error
  """

  def __init__(self, msg):
    self.msg = msg

  def __str__(self):
    return self.msg

      
# Private globals ######################################################
# Visible within this file, but not to the outside.

# Identify a begin enabled/not_disabled command. group(1) contains either the enabled or
# disabled flag. Put the rest of the line in group(2)
_begin_en_dis_comm_re = re.compile ("^begin\s+(enabled|notdisabled|not_disabled|not-disabled)\s+(.*)$")
  
# Identify an end enabled/not_disabled command. group(1) contains either the enabled or
# disabled flag. Put the rest of the line in group(2)
_end_en_dis_comm_re = re.compile ("^end\s+(enabled|notdisabled|not_disabled|not-disabled)\s+(.*)$")
  
# Identify an end comm and put the rest of the line in group(1)
_end_comm_re = re.compile ("^end\s+(.*)?$")
  
# Identify an end option command and put the rest of the line in group(1)
_end_option_comm_re = re.compile ("^end\s+option\s+(.*)$")
  
# Name of the DECtape image file we create
_new_sys_tape_prefix = "system"

# Parser regexps used in patcher
_com_os8_parse_str = "^\.([a-zA-Z]+)\s?(.*)$"
_com_os8_parse = re.compile(_com_os8_parse_str)
_com_split_str = "^([a-zA-Z]+)\s?(.*)$"
_com_split_parse = re.compile(_com_split_str)
_odt_parse_str = "^([0-7]+)\s?/\s?(\S+)\s+([0-7;]+)"
_odt_parse = re.compile(_odt_parse_str)

# Put command keyword in group(1) and the rest is in group(3)
_comm_re_str = "^(\S+)(\s+(.*))?$"
_comm_re = re.compile(_comm_re_str)

# Identify an end comm and put the rest of the line in group(1)
_end_comm_re = re.compile ("^end\s+(.*)?$")

# Identify an end option command and put the rest of the line in group(1)
_end_option_comm_re = re.compile ("^end\s+option\s+(.*)$")

# Identify a begin command and put the rest of the line in group(1)
_begin_option_comm_re = re.compile ("^begin\s+option\s+(.*)$")

# Parse an argument string into a sys device
_mount_regex_str = "^(rk|td|dt|rx)(\d?)\s+(.*)$"
_mount_re = re.compile(_mount_regex_str)

# Map of SIMH device names to OS/8 device name prefixes.
_os8_from_simh_dev = {"rk" : "RK", "td" : "DTA", "dt" : "DTA", "rx" : "RX"}

_os8_partitions = {"RK": ["A", "B"]}

# OS/8 file name matching regex
_os8_file_re = re.compile("(\S+):(\S+)?")

# Regular expression for syntax checking inside FOTP
# Destination is in group(1), Source is in group(3)
_fotp_re = re.compile ("^((\S+:)?\S+)<((\S+:)?\S+)$")

# Regular expression for detecting the 2 arg and 3 arg forms
# of the "pal8" script command.

# OS/8 name regex template:
# Optional device spec, i.e. DTA0:
# File spec with a specific extension or no extension.

_os8_fspec = Template ("((\S+:)?([A-Z0-9]{1,6}|[A-Z0-9]{1,6}\.$ext))")
_os8_BN_fspec = _os8_fspec.substitute(ext="BN")
_os8_PA_fspec = _os8_fspec.substitute(ext="PA")
_os8_LS_fspec = _os8_fspec.substitute(ext="LS")

# For the two arg form:
# The full destination spec is in group(1), The full source spec is in group(4).
# The device components, if any, are in group(2) for destination, and
# group(5) for source.
# The file components are in group(3) for destination, and group (6) for source.
# The destination file must either end in ".BN" or have no extension.
# The source must file either end in ".PA" or have no extension.
_two_arg_pal_re = re.compile ("^" + _os8_BN_fspec + "<" + _os8_PA_fspec + "$")

# For the 3 arg form:
# The full destination spec is in group(1), The full source spec is in group(7).
# The full listing spec is in group(4)
# The device components, if any, are in group(2) for destination, group(5)
# for listing, and group(8) for source.
# The file components are in group(3) for destination, and group(9) for source,
# and group(6) for listing.
# The destination file must either end in ".BN" or have no extension.
# The source must file either end in ".PA" or have no extension.
# The listing must either end in "LS" or have no extension.

_three_arg_pal_re = re.compile ("^" + _os8_BN_fspec + "," + _os8_LS_fspec + "<" + _os8_PA_fspec + "$")

# Regular expression for syntax checking inside ABSLDR
# One or more OS/8 binary files and optional args beginning with a slash.

_absldr_re = re.compile ("^" + _os8_BN_fspec + "(," + _os8_BN_fspec + ")*(/\S)*$")

# Regular expressions for syntax checking for copy_into and copy_from.
# May be <source> where destination and default option /A is implied.
# Or <source> <option> where destination is implied and option is set.
# Or <source> <destination> where option /A is implied.
# Or <source> <destination> <option> are explicit.
# Valid options are "/A", "/I", and "/B"
# Use two regex's in order:
# <source> in group 1, <dest> in group 2.
# Option is one of /I /B /A in group 4.

# source in group 1, option in group 3.
_from_into_re_1 = re.compile ("^(\S+)(\s+(/[AIB]))?$")
# source in group 1, destination in group 2, option in group 4.
_from_into_re_2 = re.compile ("^(\S+)\s+(\S+)(\s+(/[AIB]))?$")

# Array of regular expressions for syntax checking inside BUILD
_build_comm_regs = {"LOAD"  : re.compile("^(\S+:)?\S+(.BN)?$"),
                    "UNLOAD": re.compile("^\S+(,\S+)?$"),
                    "INSERT": re.compile("^\S+,\S+(,\S+)?$"),
                    "DELETE": re.compile("^\S+(,\S+)?$"),
                    "SYSTEM": re.compile("^\S+$"),
                    "DSK"   : re.compile("^(\S+:)?\S+$"),
                    "BUILD" : re.compile("^(\S+(.BN)?)\s+(\S+(.BN)?)$"),
                    "PRINT" : None,
                    "BOOT"  : None,
                    "end"   : None}

_build_replies = ["\\$", "WRITE ZERO DIRECT\\?", "\\?BAD ARG",
                  "\\?BAD INPUT", "\\?BAD LOAD",
                  "\\?BAD ORIGIN", "\\?CORE", "\\?DSK", "\\?HANDLERS",
                  "I/O ERR", "\\?NAME", "NO ROOM", "SYS NOT FOUND",
                  "\\?PLAT", "\\?SYNTAX", "\\?SYS", "SYS ERR",
                  "\S+ NOT FOUND"]

# Parse two whitspace separated arguments into group(1) and group(2)
_two_args_re = re.compile("^(\S+)\s+(\S+)$")

_rx_settings = ["rx01", "rx02", "RX8E", "RX28"]
_tape_settings = ["td", "dt"]
_tti_settings = ["KSR", "7b"]
_configurables = {"rx": _rx_settings, "tape": _tape_settings,
                  "tti": _tti_settings}

# Options enabled/not_disabled for conditional execution in scripts.
#
# Earlier code allowed --enable and --disable. We interface to it.
# We maintain two arrays: options_enable and options_disabled for those
# two argument constructs.
#
# Argument parsing of repeated enable and disable arguments is as follows:
# --enable looks on options_disabled and if present removes it, then adds
# to options_enabled.
# --disable looks on options_enabled and if present removes it, then adds
# to options_enabled.
#
# Last seen enable/disable command line or executed command for a
# particular option wins.
#
# Scripts have enable/disable commands that are run-time
# changers of the contents of options_enabled and options_disabled.
#
# When we run a script we have begin/end blocks for enabled/not_disabled options:
# "begin enabled <option name>" ... "end enabled <option name>
# "begin not_disabled <option name>" ... "end not_disabled <option name>
#
# The enabled block looks for an explicit enablement on the options_enabled
# list. If none is found we default to ignoring the contents of the block.
#
# The not_disabled block looks for an explicit disablement on the
# options_disabled lis.  If found, the block is ignored. Otherwise
# the block defaults to being executed.
#
# begin/end blocks can be nested.  We track the nesting with options_stack.
# Testing for options happens when the begin command is evaluated.
# So changing an enable/disable option inside a begin/end block
# takes effect at the next begin statement. 
# You can write a script as follows:
# enable foo
# begin enabled foo
# # Commands to executed
# disable foo
# # Commands still being executed.
# begin enabled foo
# # Commands to ignore
# end enabled foo
# end enabled foo


class os8script:
  # Contains a simh object, other global state and methods
  # for running OS/8 scripts under simh.
  #### globals and constants ###########################################
  
  
  def __init__ (self, simh, enabled_options, disabled_options, verbose=False, debug=True):
    self.verbose = verbose
    self.debug = debug
    self.simh = simh
    self.options_enabled = enabled_options
    self.options_disabled = disabled_options
    # Do we need separate stacks for enabled/disabled options?
    self.options_stack = []
    # List of scratch files to delete when we are done with our script.
    self.scratch_list = []


  #### basic_line_parse ################################################
  # Returns stripped line and any other cleanup we want.
  # Returns None if we should just 'continue' on to the next line.
  # Filters out comments.
  # Processes the option begin/end blocks.
  
  def basic_line_parse (self, line, script_file):
    if line[0] == "#": return None
    retval = line.strip()
    if retval == "": return None
    # First test if we are in a begin option block
    m = re.match (_begin_en_dis_comm_re, retval)
    if m != None:
      en_dis = m.group(1)
      rest = m.group(2)
      if self.verbose: print "doing_begin_option: " + en_dis + " " + rest
      if self.debug: print "options_enabled: " + str (self.options_enabled)
      if self.debug: print "options_disabled: " + str (self.options_disabled)

      if en_dis == "enabled":
        if rest in self.options_enabled:
          # Block is active. We push it onto the stack
          if self.debug: print "Pushing enabled block " + rest + " onto options_stack"
          self.options_stack.insert(0, rest)
        else:
          # Option is inactive.  Ignore all subseqent lines
          # until we get to an end command that matches our option.
          self.ignore_to_subcomm_end (line, script_file, en_dis + " " + rest)

        return None
      # only other choice is disabled because of our regex.
      else:
        if rest not in self.options_disabled:
          # Block defaults to active. We push it onto the stack
          if self.debug: print "Pushing not_disabled block " + rest + " onto options_stack"
          self.options_stack.insert(0, rest)
        else:
          # Block is inactive.  Ignore all subseqent lines
          # until we get to an end command that matches our option.
          self.ignore_to_subcomm_end (line, script_file, en_dis + " " + rest)
        return None
  
    m = re.match(_end_en_dis_comm_re, line)
    if m != None:
      rest = m.group(2)
      if self.verbose: print "Inside end: rest = " + rest
      if (rest == None or rest == ""):
        print "Warning! option end statement encountered with no argument."
        return None
      if len(self.options_stack) == 0:
        print "Warning! option end statement found with no matching begin for option: " + rest
        return None
      if rest != self.options_stack[0]:
        print "Warning! Mismatched option begin/end group. Currently inside option: " + \
          self.options_stack[0] + " not " + rest
        return None
      else:
        if self.debug: print "Popping " + self.options_stack[0]
        self.options_stack.pop()
        return None
  
    return retval
  
  
  #### ignore_to_subcomm_end ###########################################
  
  def ignore_to_subcomm_end (self, old_line, script_file, end_str):
    if self.debug: print "ignore to: " + end_str
    for line in script_file:
      line = line.strip()
      if self.verbose: print "Ignore: " + line
      
      m = re.match(_end_comm_re, line)
      if m == None: continue
  
      rest = m.group(1)
      if rest == None: rest = ""
      
      if rest == end_str: return
  
  
  #### include_command #################################################
  # Call run_system_script recursively on the file path provided.
  
  def include_command (self, line, script_file):
    if not os.path.isfile(line):
      print "Could not find include file: " + line
      return "fail"
    return self.run_script_file (line)
      
  
  #### enable_option_command ###########################################
  # Deletes an option from the list of active options.
  
  def enable_option_command (self, line, script_file):
    m = re.match(_comm_re, line)
    if m == None:
      print "Could not parse enable command."
      return "fail"
    option = m.group(1)
    if option == None:
      print "Empty option to enable."
      return "fail"
    # Remove it from other set if present
    if option in self.options_disabled:
      self.options_disabled.remove(option)
    # Add it if not already present.
    if option not in self.options_enabled:
      self.options_enabled.append(option)
    return "success"


  #### disable_option_command ###########################################
  # Deletes an option from the list of active options.
  
  def disable_option_command (self, line, script_file):
    m = re.match(_comm_re, line)
    if m == None:
      print "Could not parse disable option command."
      return "fail"
    option = m.group(1)
    if option == None:
      print "Empty option to disable command."
      return "fail"
    # Remove it from other set if present
    if option in self.options_enabled:
      self.options_enabled.remove(option)
    # Add it if not already present.
    if option not in self.options_disabled:
      self.options_disabled.append(option)
    return "success"


  #### configure_command ###############################################
  # First arg is the item to configure.
  # Second arg is the setting.
  # This enables adding option setting inside a script file.
  
  def configure_command (self, line, script_file):
    m = re.match(_two_args_re, line)
    if m == None or m.group(1) == None or m.group(2) == None: 
      print "Could not parse configure command: " + line
      return "fail"
    item = m.group(1)
    setting = m.group(2)
    if item not in _configurables:
      print "Ignoring invalid configuration item: " + item
      return "fail"
    if setting not in _configurables[item]:
      print "Cannot set " + item + " to " + setting
      return "fail"
    if item == "tape":
      self.simh.set_tape_config(setting)
    elif item == "rx":
      self.simh.set_rx_config (setting)
    elif item == "tti":
      self.simh.set_tti_config (setting)
    return "success"


  #### copy_into_command ###########################################
  # Calls os8_pip_into with the command line arguments.
  
  def copy_into_command (self, line, script_file):
    m = re.match(_from_into_re_1, line)
    if m != None:
      self.simh.os8_pip_into (m.group(1), "DSK:", m.group(2))
    else:
      m = re.match(_from_into_re_2, line)
      if m == None:
        print "Could not parse copy_into command."
        return "fail"
      self.simh.os8_pip_into (m.group(1), m.group(2), m.group(4))
    return "success"


  #### copy_into_command ###########################################
  # Calls os8_pip_from with the command line arguments.
  
  def copy_from_command (self, line, script_file):
    m = re.match(_from_into_re_2, line)
    if m == None:
      print "Could not parse copy_from command."
    return "fail"
    self.simh.os8_pip_from (m.group(1), m.group(2), m.group(4))
    return "success"


  #### copy_command ###############################################
  # Simple script interface to create a copy of a file.

  def copy_command (self, line, script_file):
    m = re.match(_two_args_re, line)
    if m == None or m.group(1) == None or m.group(2) == None: 
      print "Could not copy command: " + line
      return "fail"
    from_path = m.group(1)
    to_path = m.group(2)

    print "copy command: from: " + from_path + ", to: " + to_path
    
    if (not os.path.isfile(from_path)):
        print "Required copy input file: " + from_path + " not found."
        return "fail"

    if os.path.isfile(to_path):
      save_path = to_path + ".save"
      print "Pre-existing " + to_path + " found.  Saving as " + to_path + ".save"
      if os.path.isfile(save_path):
        print "Overwriting old " + to_path + ".save"
        os.remove(save_path)
      os.rename(to_path, save_path)

    try:
      shutil.copyfile(from_path, to_path)
    except shutil.Error as e:
      print "copy command failed with error: " + str(e)
      return "fail"
    except IOError as e:
      print "copy command failed with IOError: " + str(e)
      return "fail"
    return "success"


  #### resume_command #############################################
  # Call the os8_resume in simh to resume OS/8.

  def resume_command (self, line, script_file):
    self.simh.os8_resume()
    return "success"


  #### patch_command ##############################################
  # Read the named patch file and perform its actions.

  def patch_command (self, line, script_file):
    if not os.path.isfile(line):
      print "Could not find patch file: " + line
      return "fail"
    self.run_patch_file (line)
    return "success"


  #### _command ###########################################
  # 

  def _command (self, line, script_file):
    return "success"


  #### run_script_file ############################################
  # Run os8 command script file
  # Call parsers as needed for supported sub commands.
  #
  # Commands:
  # mount <simh-dev> <image-file> [<option> ...]
  #       option: must-exist | no-overwrite | read-only | copy_scratch
  # umount <>
  # boot <simh-dev>
  # os8 <command-line>
  #       the rest of the line is passed verbatim to OS/8
  # pal8 <pal-command line>
  # include <script-file>
  # configure <device> <parameter>
  #       device: tti | tape | rx
  #       tt parameter: KSR | 7b
  #       tape parameter: td | dt
  #       rx parameter: rx8e | rx28 | rx01 | rx02
  # enable <parameter>
  # disable <parameter>
  # copy_into <posix-file> [<os8-file>] [<format>]
  # copy_from <os8-file> <posix-file> [<format>]
  #       format: /A | /I | /B
  # copy <from-file> <to-file>
  # patch <patch-file>
  # resume
  # done
  # begin <sub-command> <os8-path>
  # end <sub-command>
  #
  # Sub-commands:
  # build, fotp, absldr
  #
  # Commands return, "success", "fail", or "die".
  
  def run_script_file (self, script_path):
  # Strings, regexps, and command arrays used by run_system
    commands = {"mount": self.mount_command,
                "boot": self.boot_command,
                "os8": self.os8_command,
                "done": self.done_command,
                "pal8": self.pal8_command,
                "include": self.include_command,
                "begin": self.begin_command,
                "end": self.end_command,
                "umount": self.umount_command,
                "simh": self.simh_command,
                "configure": self.configure_command,
                "enable": self.enable_option_command,
                "disable": self.disable_option_command,
                "copy_into": self.copy_into_command,
                "copy_from": self.copy_from_command,
                "copy": self.copy_command,
                "resume": self.resume_command,
                "patch": self.patch_command}
  
    try:
      script_file = open(script_path, "r")
    except IOError:
      print script_path + " not found."
      return "fail"
  
    for line in script_file:
      line = self.basic_line_parse (line, script_file)
      if line == None: continue
      
      m = re.match(_comm_re, line)
      if m == None:
        print "Ignoring command line: " + line
        continue
  
      if m.group(1) not in commands:
        print "Unrecognized script command: " + m.group(1)
        continue
  
      # print "Calling: " + m.group(1)
      retval = commands[m.group(1)](m.group(3), script_file)
      if retval == "die":
        print "\nFatal error encountered in " + script_path + " with command line: "
        print "\t" + line
        sys.exit(-1)
    return "success"


  #### end_command #####################################################
  
  def end_command (self, line, script_file):
    print "Unexpectedly encountered end command: " + line
    return "fail"

  
  #### parse_odt #######################################################
  
  def parse_odt (self, com, line):
    if self.debug: print line
    
    if line == "\\c": return "break"
    match = _odt_parse.match(line)
    if match == None:
      print "Aborting because of bad ODT line: " + line
      self.simh.os8_send_ctrl('C')
      return "err"
    loc = match.group(1)
    old_val = match.group(2)
    new_val = match.group(3)
    expect_val_str = "\s?[0-7]{4} "
    
    if self.debug: print "Loc: " + loc + ", old_val: " + old_val + ", new_val: " + new_val
    self.simh.os8_send_str (loc + "/")
    self.simh._child.expect(expect_val_str)
  
    if old_val.isdigit():          # We need to check old value
      found_val = self.simh._child.after.strip()
      if found_val != old_val:
        print "\tOld value: " + found_val + " does not match " + old_val + ". Aborting patch."
        # Abort out of ODT back to the OS/8 Monitor
        self.simh.os8_send_ctrl('C')
        return "err"
  
    self.simh.os8_send_line (new_val)
    return "cont"


  #### futil_exit ########################################################
  
  def futil_exit (self, com, line):
    self.simh.os8_send_line(line)
    return "break"


  #### futil_file ########################################################
  
  def futil_file (self, com, line):
    # Redundant re-parse of line but nobody else wants args right now.
    match = _com_split_parse.match(line)
    if match == None:
      print "Aborting because of mal-formed FUTIL FILE command: " + line
      self.simh.os8_send_ctrl('C')
      return "err"
    fname = match.group(2)
    expect_futil_file_str = "\n" + fname + "\s+(.*)$"
    self.simh.os8_send_line (line)
    self.simh._child.expect(expect_futil_file_str)
    result = self.simh._child.after.strip()
    match = _com_split_parse.match(result)
    if match == None:
      print "Aborting because unexpected return status: " + result + " from: " + line
      self.simh.os8_send_ctrl('C')
      return "err"
    if match.group(2).strip() == "LOOKUP FAILED":
      print "Aborting because of FUTIL lookup failure on: " + fname
      self.simh.os8_send_ctrl('C')
      return "err"
    else:
      return "cont"
  
  
  #### parse_futil #####################################################
  #
  # Very simple minded:
  # If first char on line is an alpha, run the command.
  # If the first char on line is number, do the substitute command.
  #
  # Substitute command acts like ODT.
  # Future version should support the IF construct.
  #
  # When we encounter the EXIT command, we return success.
  
  def parse_futil (self, com, line):
    futil_specials = {
      "EXIT": self.futil_exit,
      "FILE": self.futil_file
    }
  
    if line[0].isdigit():
      # Treat the line as ODT
      return self.parse_odt(com, line)
    else:
      match = _com_split_parse.match(line)
      if match == None:
        print "Ignoring failed FUTIL command parse of: " + line
        return "cont"
      fcom = match.group(1)
      rest = match.group(2)
  
      if fcom not in futil_specials:
        # Blind faith and no error checking.
        self.simh.os8_send_line(line)
        return "cont"
      else:
        return futil_specials[fcom](fcom, line)


  #### run_patch_file ##################################################
  
  def run_patch_file (self, pathname):
    print "Running patch file: " + pathname

    try:
      patch_file = open(pathname, "r")
    except IOError:
      print pathname + " not found. Skipping."
      return "fail"
  
    special_commands = {
      "ODT": self.parse_odt,
      "R": None,               # Get next parser.
      "FUTIL": self.parse_futil
    }
  
    inside_a_command = False
    the_command = ""
    the_command_parser = None
    
    for line in patch_file:
      line = line.rstrip()
      if line == "": continue
      elif line[0] == '#': continue     # Ignore Comments
      elif inside_a_command:
        retval = the_command_parser (the_command, line)
        if retval == "break":
          inside_a_command = False
          self.simh.os8_send_ctrl('C')
        elif retval == "err":
          patch_file.close()
          return "fail"
      elif line[0] == '.':        # New OS/8 Command
        match = _com_os8_parse.match(line)
        if match == None:
          print "Aborting patch on failed OS/8 command parse of: " + line
          return "fail"
        com = match.group(1)
        rest = match.group(2)
  
        if com in special_commands:
          if com == "R":
            # Run command is special.  Take arg as the command and run it.
            com = rest
          inside_a_command = True
          the_command = com
          the_command_parser = special_commands[com]
  
        # We carefully separate com and args
        # But don't make much use of that yet.
        if self.verbose and self.debug: print line
        self.simh.os8_send_cmd ("\\.", line[1:])  # Skip Prompt.
  
    patch_file.close()
  
    print "\tSuccess."
    return "success"


  #### skip_patch ######################################################
  # Returns true if the given filename matches one of the regex string
  # keys of the given skips dict and the flag value for that key is set.
  # See skips definition in make_patch, which calls this.
  
  def skip_patch (fn, skips):
      for p in skips:
          if re.search (p, fn) and skips[p]: return True
      return False
  

  #### call_pal8 #######################################################
  # Generic call out to PAL8 with error recovery.
  # We rely on the caller to have good specifications for source,
  # binary and optional listing files.
  
  def call_pal8 (self, source, binary):
    pal8_replies = ["ERRORS DETECTED: ", "BE\s+\S+", "CF\s+\S+", "DE\s+\S+", "DF", "IC\s+\S+", "ID\s+\S+",
                    "IE\s+\S+", "II\s+\S+", "IP\s+\S+", "IZ\s+\S+", "LD\s+\S+", "LG\s+\S+", "PE\s+\S+",
                    "PH\s+\S+", "RD\s+\S+", "SE\s+\S+", "UO\s+\S+", "US\s+\S+", "ZE\s+\S+", "\S+ NOT FOUND"]
  
    if self.verbose: print "Assembling " + source
    com_line = binary + "<" + source
    self.simh.os8_send_cmd ("\\.", "R PAL8")
    self.simh.os8_send_cmd ("\\*", com_line)
    err_count = 0
    reply = self.simh._child.expect (pal8_replies)
    executed_line = self.simh._child.before.strip()
    reply_str = self.simh._child.after.strip()
    if reply == 0:
      self.simh._child.expect("\d+")
      err_count = int(self.simh._child.after.strip())
      reply_str += " " + self.simh._child.after.strip()
    if reply > 0 or err_count > 0:
      print "PAL8 Error: "
      print "\t*" + executed_line
      print "\t" + reply_str
      self.simh.os8_send_ctrl ('c')      # exit PAL8 Just in case.
      # We could do something better than just dying, I expect.
      return "fail"
    # self.simh.os8_send_ctrl ('[')      # exit PAL8
    return "success"


  #### simh_command ####################################################
  # I tried to avoid including this command but sometimes you just
  # have to reconfigure subtle bits of device drivers.
  # We assume we can call a simh command at any time, but
  # doing so puts us in the simh context that persists until we
  # issue a boot or go command.
  
  def simh_command (self, line, script_file):
    if self.verbose: print line
    self.simh.send_cmd(line)
    return "success"


  #### umount_command ##################################################
  def umount_command (self, line, script_file):
    detach_comm = "det " + line
    if self.verbose: print detach_comm
    self.simh.send_cmd(detach_comm)
    return "success"


  #### mount_command ###################################################
  # mount <simh-device> <image-file> [option ...]
  #
  # Interface to SIMH attach command with options that do a bit more.
  #
  # Options:
  # must-exist: <image-file> must exist, otherwise abort the attach.
  # no-overwrite if <image-file> already exists, create a copy with a
  #    version number suffix. This is useful when you want to prevent
  #    overwrites of a good image file with changes that might not work.
  #    os8-run steps through version seen until it can create a new
  #    version that doesn't overwrite any of the previous ones.
  # read-only:  Passes the `-r` option to SIMH attach to mount the
  #    device in read only mode.
  # copy_scratch: Create a writeable scratch version of the named image
  #    file and mount it.  This is helpful when you are booting a
  #    distribution DECtape.  Booted DECtape images must be writeable.
  #    To protect a distribution DECtape, use this option.
  #    When the script is done the scratch version is deleted.
  #
  # If the mount command fails for any reason, we presume
  # it is a fatal error and abort the script.
  
  def mount_command (self, line, script_file):
    m = re.match(_mount_re, line)
    if m == None:
      print "Could not parse mount.  Ignoring: " + line
      return "die"
    simh_dev = m.group(1)
    unit = m.group(2)
    rest = m.group(3)
    parts = rest.split()
    if len(parts) == 0:
      print "No image name specified in: " + line
      return "die"
    ro_arg = ""
    imagename = parts[0]
    dot = imagename.rindex(".")
    base_imagename = imagename[:dot]
    extension = imagename[dot:]
    copy_imagename = ""
    # Case of additional arguments.
    if len (parts) > 1:
      # Perform must_exist before copy_scratch
      if "must-exist" in parts[1:]:
          if not os.path.exists(imagename):
            print imagename + " must exist but was not found. Not mounting."
            return "die"
      if "copy_scratch" in parts[1:]:
        copy_imagename = base_imagename + "_copy" + extension
        try:
          shutil.copyfile(imagename, copy_imagename)
        except shutil.Error as e:
          print "copy_scratch failed with error: " + str(e)
          return "die"
        except IOError as e:
          print "copy_scratch failed with IOError: " + str(e)
          return "die"
        self.scratch_list.append(copy_imagename)
        imagename = copy_imagename
        
      if "read-only" in parts:
        if copy_imagename != "":
          print "You don't really need to set read only on a scratch copy."
        ro_arg = "-r "
      if "no-overwrite" in parts:
        if copy_imagename != "":
          print "Ignoring no-overwrite option because copy_scratch is present."
        else:
          next_tape = 0
          while os.path.isfile(imagename):
            print "Found: " + imagename
            next_tape += 1
            imagename =  base_imagename + "_" + str(next_tape) + extension
    if unit == None or unit == "":
      print "Need unit number for: " + line
      return "die"
  
    if simh_dev not in _os8_from_simh_dev:
      print "Unrecognized simh dev: " + simh_dev
      return "die"
    os8dev = _os8_from_simh_dev[simh_dev]
  
    attach_comm = "att " + ro_arg + simh_dev + unit + " " + imagename
  
    if self.verbose: print "mount: " + attach_comm
    self.simh.send_cmd(attach_comm)
    return "success"


  #### boot_command ####################################################
  
  def boot_command (self, line, script_file):
    boot_comm = "boot " + line
    if self.verbose: print boot_comm
    self.simh.send_cmd(boot_comm)
    return "success"


  #### os8_command #####################################################
  
  def os8_command (self, line, script_file):
    os8_comm = line
    if self.verbose: print "os8_command: " + os8_comm
    self.simh.os8_send_cmd ("\\.", os8_comm)
    return "success"


  #### pal8_command ####################################################
  # The "pal8" script command comes in two forms:
  # The two argument form where the PAL8 status is printed on the fly
  # and the 3 argument form where all status goes into the listing file.
  # We do the 3 argument form with a simple "os8" script command.
  
  def pal8_command (self, line, script_file):
    m_2form = re.match (_two_arg_pal_re, line)
    if m_2form != None:
      # Call the 2arg pal8 code that works hard at error analysis.
      return call_pal8 (self, source=m_2form.group(4), binary=m_2form.group(1))
    else:
      m_3form = re.match (_three_arg_pal_re, line)
      if m_3form != None:
        # Just run the OS/8 command.
        os8_comm = line
        if self.verbose: print "Calling 3-arg pal8 command: " + os8_comm
        self.simh.os8_send_cmd ("\\.", os8_comm)
      else:
        print "Unrecognized pal8 form: " + line
        return "fail"
    return "success"


  #### done_command ####################################################
  # Return to SIMH from OS/8
  # Detach all devices to make sure buffers all get written out.
  
  def done_command (self, line, script_file):
    for filename in self.scratch_list:
      if self.verbose: print "Deleting scratch_copy: " + filename
      os.remove(filename)
    self.simh.send_cmd ("detach all")
    return "success"


  #### begin_command ###################################################
  
  def begin_command (self, line, script_file):
    sub_commands = {"fotp": self.fotp_subcomm, "build": self.build_subcomm,
                    "absldr": self.absldr_subcomm}
  
    m = re.match(_comm_re, line)
    if m == None:
      print "Could not parse sub-command: " + line
    if m.group(1) not in sub_commands:
      print "Ignoring unrecognized sub-command: " + m.group(1)
      print "Ignoring everything to next end."
      self.ignore_to_subcomm_end(line, script_file, "")
      return "fail"
    else:
      return sub_commands[m.group(1)](m.group(3), script_file)
  
  
  #### run_build_build #################################################
  # ***CAUTION***
  # When you do this you are instructing BUILD to
  # OVERWRITE the system area.  If you do this to your
  # running RK05 pack by mistake, you WILL make a mess
  # and need to re-run mkos8 to re-make it.
  
  def run_build_build (self, os8_spec, cd_spec):
    self.simh.os8_send_cmd ("\$", "BUILD")
    self.simh.os8_send_cmd ("LOAD OS/8: ", os8_spec)
    self.simh.os8_send_cmd ("LOAD CD: ", cd_spec)
    return "success"


  #### build_subcomm ###################################################
  
  def build_subcomm (self, old_line, script_file):
    os8_comm = "RU " + old_line
    if self.verbose: print os8_comm
    self.simh.os8_send_cmd ("\\.", os8_comm)
    self.simh._child.expect("\n\\$$")
    
    for line in script_file:
      line = self.basic_line_parse(line, script_file)
      if line == None: continue
  
      m = re.match(_comm_re, line)
      if m == None:
        print "Ignoring mal-formed build sub-command: " + line
        continue
  
      build_sub = m.group(1)
      rest = m.group(3)
      if rest == None: rest = ""
      
      if build_sub not in _build_comm_regs:
        print "Unrecognized BUILD command: " + build_sub
        continue
  
      if build_sub == "end":
        if rest == "":
          print "Warning! end statement encountered inside build with no argument. Exiting build."
          return "fail"
        elif rest != "build": 
          print "Warning! Mismatched begin/end blocks. Encountered end: " + rest + "Exiting build."
          return "fail"
        # Return to monitor level
        self.simh.os8_send_ctrl ('c')
        return "success"
        
      build_re = _build_comm_regs[build_sub]
  
      if build_re != None:
        m2 = re.match(build_re, rest)
        if m2 == None:
          print "Ignoring mal-formed BUILD " + build_sub + " command: " + rest
          continue
      
        if build_sub == "BUILD":
          if m2.group(1) == None:
            print "Missing source of OS8.BN. Ignoring BUILD command."
            continue
          if m2.group(3) == None:
            print "Missing sorce of CD.BN. Ignoring BUILD command."
            continue
          if self.verbose: print "calling run_build_build"
          self.run_build_build (m2.group(1), m2.group(3))
          continue

      comm = build_sub + " " + rest
      if self.debug: print "BUILD-> " + comm

      self.simh.os8_send_line (comm)
      reply = self.simh._child.expect(_build_replies)
      if self.debug: print "reply: " + str(reply)
      # print "before: " + self.simh._child.before.strip()
      # print "after: " + self.simh._child.after.strip()
      if reply > 2:
        print "BUILD error with command " + self.simh._child.before.strip()
        print "\t" + self.simh._child.after.strip()
        self.simh.os8_send_ctrl ('c')
      # Special case "BOOT" sub-command: May ask, "WRITE ZERO DIRECT?"
      if build_sub == "BOOT" and reply == 1:
        self.simh.os8_send_line("Y")
        reply = self.simh._child.expect("SYS BUILT")
    print "Warning end of file encountered with no end of BUILD command block."
    return "fail"
  
  #### fotp_subcomm ####################################################
  
  def fotp_subcomm (self, old_line, script_file):
    os8_comm = "RU " + old_line
    if self.verbose: print os8_comm
    self.simh.os8_send_cmd ("\\.", os8_comm)
    
    for line in script_file:
      line = self.basic_line_parse(line, script_file)
      if line == None: continue
  
      # Test for special case, "end" and act on it if present.
      m = re.match(_comm_re, line)
      if m != None and m.group(1) != None and m.group(1) != "" and m.group(1)  == "end":
        rest = m.group(3)
        if rest == None or rest == "":
          print "Warning! end statement encountered inside fotp with no argument."
          return "fail"
        elif rest != "fotp":
          print "Warning! Mismatched begin/end blocks in fotp. Encountered end: " + rest
          return "fail"

        if self.verbose: print "End FOTP"
        self.simh.os8_send_ctrl ('[')
        return "success"
  
      m = re.match(_fotp_re, line)
      if m == None:
        print "Ignoring mal-formed fotp file spec: " + line
        continue
  
      comm = line
      if self.verbose: print "* " + line
      self.simh.os8_send_cmd ("\\*", line)
    print "Warning end of file encountered with no end of FOTP command block."
    return "fail"


  
  #### absldr_subcomm ##################################################
  # A clone of fotp_subcom.  Can we find a way to merge the common code?
  
  def absldr_subcomm (self, old_line, script_file):
    os8_comm = "RU " + old_line
    if self.verbose: print os8_comm
    self.simh.os8_send_cmd ("\\.", os8_comm)
    
    for line in script_file:
      line = self.basic_line_parse(line, script_file)
      if line == None: continue
  
      # Test for special case, "end" and act on it if present.
      m = re.match(_comm_re, line)
      if m != None and m.group(1) != None and m.group(1) != "" and m.group(1)  == "end":
        rest = m.group(3)
        if rest == None or rest == "":
          print "Warning! end statement encountered inside absldr with no argument."
          return "fail"
        elif rest != "absldr":
          print "Warning! Mismatched begin/end blocks in absldr. Encountered end: " + rest
          return "fail"
          
        if self.verbose: print "End ABSLDR"
        self.simh.os8_send_ctrl ('[')
        return "success"
  
      m = re.match(_absldr_re, line)
      if m == None:
        print "Ignoring mal-formed absldr file spec: " + line
        continue
  
      comm = line
      if self.verbose: print "* " + line
      self.simh.os8_send_cmd ("\\*", line)
    print "Warning end of file encountered with no end of ABSLDR command block."
    return "fail"


  #### check_exists ####################################################
  # Check existence of all files needed
  
  def check_exists (s, image_copyins):
    for copyin in image_copyins:
      image = copyin[1]
      image_path = dirs.os8mi + image
      if (not os.path.isfile(image_path)):
          print "Required file: " + image_path + " not found."
          mkos8_abort(s)
      # else: print "Found " + image_path
  
     
  #### Data Structures ################################################
  #
  # The make procedures use helper procedures
  # to confirm that the relevant input image file exists and
  # to perform the file copies.
  #
  # A data structure called "image copyin"
  #    describes the image file pathname relative to an implied root,
  #    provides a message string when the action is run,
  #    names a default destination device for whole image content copies,
  #    offers an optional array of specific file copy actions.
  #
  # FUTURE: Parse source path for ".tu56" vs. ".rk05" for more general use.
  # Currently all code assumes a copyin comes from a DECtape image.
  #
  # Example: We Install all files for ADVENT, the Adventure game:
  #
  # advent_copyin = ['RKB0:', 'subsys/advent.tu56',  "Installing ADVENT...", None]
  #
  # A DECtape device is chosen for attachment in SIMH and
  # a 'COPY *.*' command is filled in with the Destination device, and the chosen DECtape.
  #
  # A data structer called "file copyin"
  #     provides override destination to allow renames or varied destinations.
  #     names individual files within a copyin to use
  #
  # Example:  To copy the C compiler we want all .SV files on SYS
  #           but everything else to RKB0:
  #           (Note the useful /V option to invert the match.)
  #
  # cc8_sv_file_copyin   = ['SYS:', '*.SV']
  # cc8_rest_file_copyin = ['RKB0:', '*.SV/V']
  #
  # A 'COPY' command is filled in with the override destination and
  # The file spec is used with the chosen dectape instead of "*.*"
  #
  
  #### copyin_pair #####################################################
  # Copy two images into two destinations with two messages
  #
  # Assumes our context is "in simh".
  # Assumes dt0 and dt1 are free.
  # Assumes rk0 is the boot device
  # Detaches dt0 and dt1 after using them.
  # copyin0 mounts on dt0.  copyin1 mounts on dt1.
  # Either copyin or both can be None
  
  def copyin_pair (s, copyin0, copyin1, debug):
    if debug:
      if copyin0:
        print "Copying: " + copyin0[1] + " to: " + copyin0[0] + "from dt0"
      else: print "copyin0 is empty."
      if copyin1:
        print "Copying: " + copyin1[1] + " to: " + copyin1[0] + "from dt1"
      else: print "copyin1 is empty."
      
    if not copyin0 and not copyin1: return   # Nothing to do.
  
    # The order of events here is a bit funky because we want
    # to use both DECtape drives but also
    # switch between SIMH and OS/8 as infrequently as possible.
  
    if copyin0: self.simh.send_cmd ("attach -r dt0 " + dirs.os8mi + copyin0[1])
    if copyin1: self.simh.send_cmd ("attach -r dt1 " + dirs.os8mi + copyin1[1])
  
    self.simh.os8_restart()
  
    if copyin0:
      if self.verbose: print copyin0[2]
      if copyin0[3]:                    # We have specific files to do.
        for file_copyin in copyin0[3]:
          self.simh.os8_send_cmd ("\\.", "COPY " + file_copyin[0] + "<DTA0:" + file_copyin[1])
      else:
        self.simh.os8_send_cmd ("\\.", "COPY " + copyin0[0] + "<DTA0:*.*")
  
    if copyin1:
      if self.verbose: print copyin1[2]
      if copyin1[3]:                    # We have specific files to do.
        for file_copyin in copyin1[3]:
          self.simh.os8_send_cmd ("\\.", "COPY " + file_copyin[0] + "<DTA1:" + file_copyin[1])
      else:
        self.simh.os8_send_cmd ("\\.", "COPY " + copyin1[0] + "<DTA1:*.*")
  
    self.simh.back_to_cmd("\\.")
  
    if copyin0: self.simh.send_cmd ("detach dt0")
    if copyin1: self.simh.send_cmd ("detach dt1")
  
  
  #### do_all_copyins ##################################################
  
  def do_all_copyins (s, copyins, debug):
    pair_idx = 0
    pair_ct = int(len(copyins) / 2)
    while pair_idx < pair_ct:
      copyin_pair(s, copyins[pair_idx * 2], copyins[pair_idx * 2 + 1], debug)
      pair_idx += 1
    if pair_ct * 2 < len(copyins):
      copyin_pair(s, copyins[len(copyins) - 1], None, debug)
  
  
  
