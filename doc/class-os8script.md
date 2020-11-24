# class os8script: A higher level interface to OS/8 under SIMH

## Introduction

This class is a higher level abstraction above the class simh.
An understanding of that class as documented in [doc/class-simh.md][class-simh-doc]
is a prerequisite for working with this class.

Development of this class was driven by the desire to create a scripting
language to engage in complex dialogs with OS/8 commands. The first use
cases were embodied in the `os8-run` scripting system:

 * use `BUILD` to perform elaborate system configuration in a repeatable way.
 * drive commands calling the OS/8 Command decoder to operate on specified
 files under OS/8.
 * apply patches, first using `ODT` and then using `FUTIL`.
 * assemble programs using `PAL8`.
 * reconfigure the SIMH devices, for example switching between having TC08
 or TD8E DECtape devices. (Only one is allowed at a time, and the OS/8 system
 areas are incompatible.)

The latest use case, embodied in the `os8-progtest` utility, is to allow
specifying arbitrary state machines that engage in complex program dialogs
so as to permit programmed, run-time testing of functionality under OS/8.

This document describes the class os8script API with an eye to assisting
the development of stand-alone programs that use the API.

## Housekeeping

Before we get into calls to create the environment and calls to run
commands, it is important to learn the rules of housekeeping in the
`class os8script` environment.

### Important caveat about parallelism:

The pidp8i software package does a lot of complex building both under POSIX
and in a scripted way under OS/8.  The `tools/mmake` POSIX command
run multiple independent instances of `make` in parallel.

OS/8 comes from a single threaded, single computer design aesthetic, and the
boot device assumes NOTHING else is touching its contents. This means if there
is more than one instance of SIMH booting OS/8 from the same image file
(for example in two terminal windows on the same POSIX host)
the result is **completely unpredictable**.

This was the primary driver for the creation of the `scratch` option to
`mount` and the development of the `copy` command.  Care **must** be exercised
to do work in a `scratch` boot environment, and manage dependencies and concurrencies.

### Gracefully unmount virtual files

POSIX buffers output.  If you `mount` an image file, modify it, and quit
the program, the buffered modifications may be lost.  In order to guarantee
all buffers are properly flushed, you **must** call `umount` on image files
you've modified.

### Quit SIMH.

This is not a requirement, but is good practice.

### Remove scratch images.

The management of scratch images is rudimentary.  The `mount` command
creates them, and appends them to a global list `scratch_list`. There isn't
an association created with that scratch file to know that a particular
image was the original. The purpose instead, is for cleanup.

Any program that makes use of class `os8script` that would create a
scratch image needs to go through `scratch_list` and delete the scratch files.

Ok, now we can learn how to set up the environment.

## Setup:

The following will include the libraries you need:

    import os
    import sys
    sys.path.insert (0, os.path.dirname (__file__) + '/../lib')
    sys.path.insert (0, os.getcwd () + '/lib')

    from pidp8i import *
    from simh   import *


1. Recommend using argparse to create an args structure containing
the parsed command line arguments.

2. Create the simh object that will do the work.

3. Create the os8script object that calls to the simh object.

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

4. Find the system image you want to boot to do the work.

In the example below we default to using the `os8mo` element from the
dirs library that is the default media output directory where the
build process installs the bootable images. The bootable image
can be specified by a `--target` option and it defaults to the `v3d.rk05`
image.

5. Mount and boot the image.  Using `scratch` is **highly** recommended.

`main` would look something like this:


    def main ():    
      parser = argparse.ArgumentParser(
        description = """
        Run my program under PDP-8 OS/8.""",
        usage = "%(prog)s [options]")
      parser.add_argument ("--target", help="target image file", default="v3d.rk05")
      image_path = os.path.join(dirs.os8mo, args.target)
    
      try:
        s = simh (dirs.build, True)
      except (RuntimeError) as e:
        print("Could not start simulator: " + e.message + '!')
        sys.exit (1)
    
      if VERBOSE:
        s.verbose = True
        s.set_logfile (os.fdopen (sys.stdout.fileno (), 'wb', 0))
    
      os8 = os8script (s, [], [], verbose=False, debug=False)
    
      os8.mount_command ("rk0 " + targ_path + " required scratch", None)
      os8.boot_command ("rk0", None)


Further details and examples can be found by reading the `main` function
of either `bin/os8-run` or `tools/os8-progtest` with further explanation in the
early section of [doc/class-simh.md][class-simh-doc]

## Doing the Work:

There are two script-based calls if you have a file on the POSIX host,
or can assemble a string into a file handle, and express your work
as an `os8-run` style script:

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

Otherwise you do direct calls into the API

### Environment check and command startup

First call `check_and_run` to run your desired OS/8 command from the Keyboard Monitor.  It will:

 * make sure we're booted.
 * make sure we're in the OS/8 context.
 * run the command.
 * returns the reply status of the initial command or -1 if any of the previous steps fail.

### `os8-run` Commands

The methods that implement the [os8-run][os8-run-doc] commands can be called
directly.  They all take two arguments:

`line`: The rest of the line in the script file after the command was parsed.
This makes the script file look like a series of command lines.  The parser
sees the command keyword and passes the rest of the line as `line`.

`script_file`: A handle on the script file.  This gets passed around from
command to command to deal with multi-line files. We rely on this handle
keeping track of where we are in the file over time.  If you call a command
that doesn't deal with multiple lines, you can pass `None` as the `script_file`
handle.  If the command needs more lines, and it sees `None` python will kill
the program and give you a backtrace.

All of these commands return a string, "success" or "fail".

The `begin` command calls out to one of two sub-commands, `build` and `cdprog`.
These embody complex state machines to step through command dialogs and detect
error conditions.

The `patch` command contains a pretty complex state machine. It knows how
interprest patch description files as commands in either `ODT` or `FUTIL`
to modify files under OS/8 and then save them.

The `os8`, `pal8`, and `ocomp` commands contain state machines as well, albeit
less complex ones.  There is not any command dialog, just error detection.

The single most important idea to learn in producing a reliable program
using class os8script is the notion of **The Current Context and State of the
os8script Environment**. The `os8script` class is
careful to validate that OS/8 is booted and active before submitting strings
that it expects will be interpreted by the OS/8 Keyboard Monitor.  It is careful
to escape out to SIMH when sending strings it expects will be interpreted
as SIMH commands.  The os8script is set of layered state machines. SIMH starts off
at its command level, then OS/8 is booted.  To issue more SIMH commands after that
you have to escape out of OS/8, but then have to return to OS/8 to continue.
When a program is started under OS/8 it creates a new layered state machine:
The dialog with the program, until the program finishes and returns to the
OS/8 keyboard monitor.

When you run a complex command under os8script, you will be writing a state
machine that will need to return to OS/8 when it is finished.

The `os8-progtest` tool allows creation of state machine descriptions
that submit text, and listen for replies to step through the states of
a complex dialog with the program, and then return to the OS/8 keyboard
monitor.


command   | API method             | Notes
--------- | ---------------------- | --------
mount     | mount_command          | Does complex parsiing of `line` to get all the parameters needed.
boot      | boot_command           | You need to issue this command before running any commands.
os8       | os8_command            | Allows no dialog. Just run the command and return to the Keyboard Monitor.
ocomp     | ocomp_command          | Simple state machine to return `success` if two files are identical.
pal8      | pal8_command           | Contains a state machine to gather up error output nicely.
include   | include_command        | Allows running a script within a script to arbitrary depths.
begin     | begin_command          | Complex but constrained state machine for the OS/8 `BUILD` command or commands that use the OS/8 Command Decoder.
end       | end_command            | Exits begin.
print     | print_command          | Lets scripts send messages.
umount    | umount_command         | Cleans out a mount command, except for scratch files. See [cleanups][#cleanups].
simh      | simh_command           | Lets you send arbitrary commands to simh.
configure | configure_command      | An interface to a constrained subset of high level PDP-8 specific configurations.
enable    | enable_option_command  | Connects up the enable interface to scripts.
disable   | disable_option_command | Connects the disable interface to scripts.
cpto      | cpto_command           | The way you get files into OS/8 from the POSIX host. Relies on the OS/8 `PIP` command.  Contains a state machine for working through dialogs. Cleans up any temporary files for you.
cpfrom    | cpfrom_command         | The way you get files out to the POSIX host from the OS/8 environment. Relies on the OS/8 `PIP` command.  Contains a state machine for working through dialogs. Cleans up any temporary files for you.
copy      | copy_command           | Allows scripts to say, "Make me a copy of this POSIX file," which is generally an image file that serves as the basis of a modified file.
resume    | resume_command         |
restart   | restart_command        |
patch     | patch_command          | Complex. 


Tables of expect matches

The class `simh` library contains a table of all the normal and error
replies that the OS/8 Keyboard Monitor and Command Decoder are known to emit
in `_os8_replies` and pre-compiled regular expressions for each one in
`_os8_replies_rex`.  Class `os8script` has a method `intern_replies`
that allows management of additional tables by name, allowing, for example
the `build_command` state machine to create a table with replies
from the `BUILD` command **in addition to** all the OS/8 replies.

`intern_replies` takes 3 arguments:

`name`: The name of the new reply table.  If a table of that name already exists
return `False`.

`replies`: An array of 3 element arrays.  The three elemments are
1. The common name of the match string.  How the common name is used is described below.
2. The regular expression python expect will use to try and match the string.
3. True if receiving this is a fatal error that returns to the OS/8 Keyboard Monitor.
Knowing this state change is helpful in establishing correct expectation about
the state of the enviromnent.

The common name is used in match tests:

The `simh` object instantiated within the `os8script` object has a `test_result`
method that takes four arguments:

`reply`: integer index into the array of replies.
`name`: the common name of the result we are expecting to match.
`replies`: the array of replies that we are testing against.
`caller`: if not empty, an error string is printed that uses the value of
`caller` as a preface, if the common name we expect (passed in via
`name`0 does not match what is found in the `replies` array at the index `reply`.

For example if we wanted to test a start up of `MYPROG` into the command decoder
we could do this:

    reply = self.simh.os8_cmd ("R MYPROG", debug=self.debug)
    self.simh.os8_test_result (reply, "Command Decoder Prompt", "start_myprog")

If we didn't get the Command Decoder prompt, because MYPROG wasn't found we'd
get something like this:

   start_myprog: failure
   Expected "Command Decoder Prompt". Instead got "File not found".


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

[class-simh-doc]: https://tangentsoft.com/pidp8i/doc/trunk/doc/class-simh.md
[os8-run-doc]: https://tangentsoft.com/pidp8i/doc/trunk/doc/os8-run.md

## <a id="license" name="credits"></a>Credits and License

Written by and copyright © 2017-2020 by Warren Young and William Cattey.
Licensed under the terms of [the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
