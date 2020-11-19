Outline:

How-To as taken from os8-progtest/os8-run
The steps needed in Main:

Setup:

1. Recommend using argparse to create an args structure containing
the parsed command line arguments.

2. Create the simh object that will do the work.

3. Create the os8script object that calls to the simh object.

Do the Work:

`run_script_file` was the first use case.  A filename is passed in,
and the library is responsible for opening the file and acting on its
contents.  There are helper routines for enabling the script to
find the image file to boot.

`run_script_handle` is called by `run_script_file` once the
filename has been successfully opened.  This method allows creation
of in-memory file handles using the `io` library. For example:

    import io

    _test_script =  """
    enable transcript
    os8 DIR
    """

    script_file = io.StringIO(_test_script)
    os8.run_script_handle(script_file)

Open code calls to the API

1. 

4. Find the system image you want to boot to do the work.


3. Call self.check_and_run.  It will:

 * make sure we're booted.
 * make sure we're in the OS/8 context.
 * start the initial command.

It returns the reply status of the initial command or
-1 if running is not possible.



API

The `os8script` object is a higher level interface to
SIMH for executing the OS/8 environment.

The creation method, `os8script` takes up to 5 arguments:

`simh`: The `simh`: object that will host SIMH.  See [class-simh.md][simh-class-doc].
`enabled_options`: List of initial options enabled for interpreting script commands.
`disabled_options`: List of initial options disabled for interpreting script commands.
`verbose`: Optional argument enabling verbose output. Default value of `False`.
`debug`: Optional argument enabling debug output. Default value of `True`.

The two options lists were put into the creation call initially because
for the first use of the API, it was easy to pass the arrays returned by
argparse.  Conceptually, an initial set of options is passed in at create
time, and thereafter the add/remove calls are used to change the active options
one at a time.



Legacy text:
How to implement the state machine to drive a program under OS/8

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
