#!/usr/bin/env python3

# Demo program for using class-os8script.

import os
import sys
sys.path.insert (0, os.path.dirname (__file__) + '/../lib')
sys.path.insert (0, os.getcwd () + '/lib')

from pidp8i import *
from simh   import *
from os8script import *

_basic_replies = [
  ["READY", "\r\nREADY\r\n", False],
  ["NEW OR OLD", "NEW OR OLD--$", False],
  ["FILENAME", "FILE NAME--$", False],
  ["BAD FILE", "BAD FILE$", False],
  ["ME", "ME", False],
  ["3 READY", "\s+3\s+READY\r\n", False],
  ]

import argparse

def main ():    
  parser = argparse.ArgumentParser(
    description = """
    Run my program under PDP-8 OS/8.""",
    usage = "%(prog)s [options]")
  parser.add_argument ("--target", help="target image file", default="v3d.rk05")
  parser.add_argument("-v", "--verbose", help="increase output verbosity",
             action="store_true")

  args = parser.parse_args()
  
  image_path = os.path.join(dirs.os8mo, args.target)

  try:
    s = simh (dirs.build, True)
  except (RuntimeError) as e:
    print("Could not start simulator: " + e.message + '!')
    sys.exit (1)

  if args.verbose: s.set_logfile (os.fdopen (sys.stdout.fileno (), 'wb', 0))

  os8 = os8script (s, [], [], verbose=args.verbose, debug=False)
  os8.intern_replies ("basic", _basic_replies, True)

  os8.mount_command ("rk0 " + image_path + " required scratch", None)
  os8.boot_command ("rk0", None)

  reply = os8.simh.os8_cmd ("R BASIC", os8.replies_rex["basic"])
  if reply == -1:
    print ("OS/8 isn't running! Fatal error!")
    sys.exit (1)

  # State machine to submit, run and revise a BASIC program.
  mon = os8.simh.test_result(reply, "Monitor Prompt", os8.replies["basic"], "")
  while not mon:
    if args.verbose: print ("Got reply: " + os8.replies["basic"][reply][0])

    if os8.simh.test_result(reply, "NEW OR OLD", os8.replies["basic"], ""):
      reply = os8.simh.os8_cmd ("NEW", os8.replies_rex["basic"])
    elif os8.simh.test_result(reply, "FILENAME", os8.replies["basic"], ""):
      reply = os8.simh.os8_cmd ("MYPROG", os8.replies_rex["basic"])  
    elif os8.simh.test_result(reply, "READY", os8.replies["basic"], ""):
      reply = os8.simh.os8_cmd ("10 PRINT 1 + 2\r20 END\rRUN\r", os8.replies_rex["basic"])  
    elif os8.simh.test_result(reply, "3 READY", os8.replies["basic"], ""):
      print ("Got Expected Result! Sending ^C")
      os8.simh.os8_send_ctrl ('c')
      reply = os8.simh._child.expect(os8.replies_rex["basic"])
    else:
      print ("Unexpected result. Sending ^C")
      os8.simh.os8_send_ctrl ('c')
      reply = os8.simh._child.expect(os8.replies_rex["basic"])
    mon = os8.simh.test_result(reply, "Monitor Prompt", os8.replies["basic"], "")

  os8.exit_command("0", "")

if __name__ == "__main__": main()
