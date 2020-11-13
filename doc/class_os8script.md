

How to implement the state machine to drive a program under OS/8

Call self.check_and_run to:

 * make sure we're booted.
 * make sure we're in the OS/8 context.
 * start the initial command.

It returns -1 if we can't run, and the reply status of the
OS/8 run command.

Tricky bit:  If the program uses its own command prompt, we need to
go beyond the default replies that would come back from the
os8 set. Otherwise the state machine will hang looking for a prompt
it does not recognize.

TODO: add a hook for an additional command prompt.

You have a mini-state machine in your command:

 * Decide which results indicate "success", "fail" and/or "die".  You
 * You need to leave your command with OS/8 back at command level after
   having recognized that the monitor prompt has been printed.  If you don't
   the state machine gets confused and hangs.
 * You can gather intermediate results from `self.simh.child_before().strip()`
 * You can gather final results from self.simh.child_after().strip()

The implementation of the OCOMP gives a good example:

The "success" criterion is the "NOTHING OUTPUT" message indicating that
both input files are identical.  When that happens the program returns to
the OS/8 monitor, and so we exit the OCOMP state machine as soon as we
see the monitor prompt followint the "NOTHING OUTPUT" results.

The `USER ERROR` and `File not found` outputs are considedred non
fatal erorrs.  self.simh.child_after().strip() provides detail.

It is expected that the monitor prompt will be forthcoming and
the next cycle through the while loop will see it.

However, if we get a command decoder prompt, it means we need to send
`^C` to exit the program and return to the monitor.

The `while` loop does just that.

For debugging purposes we use expect_loop to show how many times we've
cycled through the output looking for stuff.

The conditional on `transcript` is how we pass program output out to
the command shell that ran `os8-run`.  It must appear after the call
to `self.simh._child.expect(self.replies_rex["ocomp"])` so that the
output between when we feed the command line to the command decoder
prompt, and the sight of the monitor prompt is harvested from
`self.simh.child_before().strip()` and displayed.

  def ocomp_command (self, line, script_file):
    if self.verbose: print("Running OCOMP on: " + line)
    reply = self.check_and_run("ocomp", "R OCOMP", script_file)
    if reply == -1: return "die"

    if self.simh.os8_test_result (reply, "Command Decoder Prompt", "call_ocomp") == False:
      print("OCOMP failed to start at line " + str(self.line_ct_stack[0]))
      return "fail"

    if self.verbose: print("Line: " + \
       str(self.line_ct_stack[0]) + ": ocomp_command: " + line)

    # Send our command and harvest results.
    reply = self.simh.os8_cmd (line, self.replies_rex["ocomp"])
    if "transcript" in self.options_enabled:
      print (self.simh._child.before.decode().strip())

    ret_val = "fail"
    expect_loop = 1

    mon = self.simh.test_result(reply, "Monitor Prompt", self.replies["ocomp"], "")
    # We get return status and then clean up the state machine.
    while not mon:
      if self.debug:
        print ("ocomp gave reply: [" + str(expect_loop) +"] " + str(reply) + " -> " + \
             self.replies["ocomp"][reply][0])
      ok = self.simh.test_result(reply, "NOTHING OUTPUT", self.replies["ocomp"], "")
      cd  = self.simh.test_result(reply, "Command Decoder Prompt", self.replies["ocomp"], "")
      nf = self.simh.test_result(reply, "File not found", self.replies["ocomp"], "")
      ue = self.simh.test_result(reply, "USER ERROR", self.replies["ocomp"], "")

      if ok:
        if self.debug: print ("Success")
        ret_val = "success"
      elif nf or ue:
        print ("OCOMP:" + self.simh.child_after().strip() + "\n")
      elif cd: # Got Command Decoder. Exit with failure.
        if self.debug: print ("call_ocomp: Non-fatal error: Sending ^C")
        # Exit OCOMP. We'll confirm we got back to OS/8 monitor below.
        self.simh.os8_send_ctrl ('c')      

      reply = self.simh._child.expect(self.replies_rex["ocomp"])

      if "transcript" in self.options_enabled:
        print (self.simh._child.before.decode().strip())

      expect_loop += 1

      mon = self.simh.test_result(reply, "Monitor Prompt", self.replies["ocomp"], "")

    return ret_val
