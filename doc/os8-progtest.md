# os8-progtest: Perform tests on a program under OS/8

This program uses Python expect to work through tests of a program
under OS/8.  The test cases and expected output are expressed in YAML format
utilizing the [pyyaml library][pyyaml].

It is found in the `tools` directory of the source tree.

## usage

> `os8-progtest` [`options`]

|                      | **Positional Arguments**
| -------------------- | ----------------------------------------------
|  _prog_spec_         | program to test, and optional subset of tests
|                      | **Optional Arguments**
|  -h, --help          | show this help message and exit
|  -v, --verbose       | increase output verbosity
|  -d DEBUG            | set debug level
|  --destdir DESTDIR   | Destination directory for output files
|  --srcdir SRCDIR     | Source directory for test .yml files
|  --target TARGET     | target image file
|  --dry-run, -n       | Dry Run. Run the tests pretending they all succeed.

The tests are run.  If the expected output fails to be found, the program
will time out and fail the test.

More than one _prog_spec_ can appear on the command line, to test more than
one program at a time.

The _prog_spec_ argument consists either of a program name, for example:

    os8-progtest cc8

or the program name followed by a colon and then a comma separated list of tests.

    os8-progtest cc8:ps,fib

## The test definition file

The `.yml` file defines series of tests, each one of which consists of
a state machine.  The state machine consistes of a state name, the
test text string to send, and an array of possible responses and the
state to go to if the response is received.

In the abstract, each test name begins starts at the beginning of a line,
and all the associated states are indented one tab stop.
Each state consits of a name followed by an array specification,
where the first element is the test text string to send, and the
second element is an array of response, newstate pairs.

Every state machine must have a start state.

Every state machine should have at least one state that names `success` or
`failure` as termination states.

The `-n`, `--dry-run` option should be used at development time to
confirm that the test state machine will terminate with success if all
tests come back successful.

The easiest way to understand how to define the state machine is to
study an example. This file defines two tests for the `cc8` package,
`ps` and `fib`.  For each test, the start-up is to run the `CCR`
`BATCH` script, and when prompted, supply the name of a C program to
be processed by the `CCR` script.

    'ps':
        'start': ["EXE CCR\r", [["PROGRAMME\\s+>", 'progname']]]
        'progname': ["ps.c\r", [[".*924.*COMPLETED\r\n\r\n#END BATCH\r\n\r\n.$", 'success']]]
    'fib':
        'start': ["EXE CCR\r", [["PROGRAMME\\s+>", 'progname']]]
        'progname': ["fib.c\r", [["OVERFLOW AT #18 = 2584\r\n\r\n#END BATCH\r\n\r\n.$", 'success']]]
    
Both the `ps` and `fib` tests follow a very similar structure:

 * Both have state machines that begin with a `start` state, and
   contain a `success` terminating condition.
 * Both run the OS/8 command `EXE CCR` which runs the script `CCR.BI` found
   the default storage device `DSK:`.
 * The prompt received is the same in both cases, and send to `progname` state
   which feeds in the name of the C program to compile and run.
 * Both then detect the `#END BATCH` output and known good last line output of
   the execution of the respective C programs.

An example of two correct answers can be found in this state machine used for
a basid test of the `ADVENT` game under the `FRTS` FORTRAN IV run-time system:

    'startup': 
        'start': ["R FRTS\r",[["\n\\*", 'advent']]]
        'advent': ["ADVENT\e", [[ "LOCATION OF TEXT DATABASE\\s+\\(\\S+\\).*", 'database'],
                           ["WELCOME TO ADVENTURE!!", 'instructions']]]
        'database': ["\r", [["LOCATION OF TEXT INDEX\\s+\\(\\S+\\).*", 'index']]]
        'index': ["\r", [["LOCATION OF SAVED GAMES\\s+\\(\\S+\\).*", 'saved']]]
        'saved': ["\r", [["LOCATION OF TEXT INPUT\\s+\\(\\S+\\).*", 'input']]]
        'input': ["\r", [["WOULD YOU LIKE INSTRUCTIONS\\?.*", 'instructions']]]
        'instructions': ["n\r", [["\r\n> $", quit]]]
        'quit': ["quit\r", [["DO YOU REALLY WANT TO QUIT NOW\\?\r\n\r\n> ", 'yes']]]
        'yes': ["yes\r", [["\n\\.$", 'success']]]

Notice the use of `\e` to start `ADVENT`, terminating the OS/8 Command Decoder
input with an escape.

Either the user is prompted for locations of run-time files, or if said
files are already configured, it goes directly to the `instructions` question.

### Additional syntax information

A line beginning with a `#` is ignored as a comment.


## Crafting the state machines

1. The YAML Quoting conventions are carefully chosen!

    1. Surround state names with single quotes.  Otherwise state names like `yes`
get evaluated and turned into something else. (`yes` becomes `True`.)

    2. Surround send and reply strings with double quotes. The YAML evaluation of
quoted strings is the simplest to understand as it gets translated into
Python pexpect regular expressions.

2. Command lines end with a carriage return denoted by '\r' or escape denoted
by '\e'.

3. To get out of a program with `<CTRL>C` you send the YAML hex code `\x03`
which gets properly translated and sent.

4. `pexpect` translates all TTY output you see from running the simulator
to upper case.  So take anything you see, and translate it to upper case
in the reply string.

5. It is important to escape characters that normally have regex
meaining: `.` `+` `*` `$` `(` `)` and `\`.  To do this in YAML,
preface each occurrance with two backslashes.  So, for example, to pass
in a literal question mark, rather than the question mark regex
special you would replace `?` with `\\?`.

6. To pass in regex escapes, that have a backslash, for example `\s` for
whitespace, double the backslash. (So `\s` becomes `\\s`.

6. Explicitly calling out end of reply string with `$` can often improve
reliability.

7. Sometimes guessing the exact whitespace is difficult.  The `\\s+` construct
to match on one or more whitespace characters is often helpful.

8. Helpful match strings:

string     |  Meaning
---------- | -----------------
"\n\\.$"   | OS/8 Monitor prompt.  Always look for this at the end.
"\n\\*$"   | OS/8 Command decoder prompt.  Often the first step in running programs.


[pyyaml]: https://pyyaml.org/wiki/PyYAMLDocumentation

### <a id="license"></a>License

Copyright © 2020 by Bill Cattey. Licensed under the terms of [the SIMH license][sl].

[sl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
