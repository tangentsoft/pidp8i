Hacking on the PiDP-8/I Software
====

If you are going to make any changes to the PiDP-8/I software, here are
some rules and hints to keep in mind while you work.


Getting Started with Fossil
----

The PiDP-8/I software project is hosted using the [Fossil][fossil]
[distributed version control system][dvcs].  Fossil provides most of the
features of GitHub under a simpler operating model than Subversion
without tying you to a proprietary web service.

This guide will introduce you to some of the basics, but you should also
at least read the [Fossil Quick Start Guide][fqsg]. For a more thorough
introduction, I recommend [the Schimpf book][fbook]. If you have
questions, it is best to ask them on [its low-volume mailing list][fml],
though you may also ask me, either on [the PiDP-8/I mailing list][ggml]
or via private email.

You must use Fossil version 2.1 or higher with our repository, or you
will get an error.

If you started with one of our PiDP-8/I binary OS images made in or
after April 2017, Fossil 2.x is already installed.

If you're starting from some other OS, you either won't have Fossil
installed at all, or you'll most likley be using an older version, since
the Debian project is still shipping version 1.37 and likely will
continue to do so until 2020 or so.  You'll have to build Fossil from
source:

    $ sudo apt install libssl-dev
    $ wget -O fossil-release.tar.gz \
      https://fossil-scm.org/index.html/tarball/fossil-release?uuid=release
    $ tar xvf fossil-release.tar.gz
    $ cd fossil-release
    $ ./configure
    $ make
    $ sudo make install

Fossil is also available for all common desktop platforms.  One of [the
official binaries][fbin] may work on your system.  If you're getting
binaries from a third party, be sure it is Fossil 2.1 or higher.


[fbin]:   http://fossil-scm.org/index.html/uv/download.html
[dvcs]:   http://en.wikipedia.org/wiki/Distributed_revision_control
[fbook]:  http://www.fossil-scm.org/schimpf-book/home
[fml]:    http://mailinglists.sqlite.org/cgi-bin/mailman/listinfo/fossil-users
[fossil]: http://fossil-scm.org/
[fqsg]:   http://fossil-scm.org/index.html/doc/trunk/www/quickstart.wiki
[ggml]:   https://groups.google.com/forum/#!forum/pidp-8


Fossil Anonymous Access
----

To clone the code repository anonymously, say:

    $ mkdir -p ~/museum ~/src/pidp8i/trunk
    $ sudo apt install fossil
    $ fossil clone https://tangentsoft.com/pidp8i ~/museum/pidp8i.fossil
    $ cd ~/src/pidp8i/trunk
    $ fossil open ~/museum/pidp8i.fossil

The `clone` command gets you a file called `pidp8i.fossil` containing
the full history of PiDP-8/I from the upstream 2015.12.15 release
onward.  You can call that clone file anything you like and put it in
any directory you like.  Even the `.fossil` extension is largely a
convention, not a requirement.


Working With Existing Tags and Branches
----

The directory structure shown in the commands above is more complicated
than strictly necessary, but it has a number of nice properties.

First, it collects other software projects under a common top-level
directory, which I'm calling `~/src`, but you are free to use any scheme
you like.

Second, the top-level project directory stores multiple separate
checkouts, one for each branch or tag I'm actively working with at the
moment.  So, to add a few other checkouts, you could say:

    $ cd ~/src/pidp8i
    $ mkdir -p release          # another branch
    $ mkdir -p v20151215        # a tag this time, not a branch
      ...etc...
    $ cd release
    $ fossil open ~/museum/pidp8i.fossil release
    $ cd ../v20151215
    $ fossil open ~/museum/pidp8i.fossil v20151215
      ...etc...

This gives you multiple independent checkouts.  The branch checkouts
remain pinned to the tip of that branch, so that if someone else checks
changes in on that branch and you say `fossil update`, those changes
appear in your checkout of that branch.  The tag checkouts simply give
you the latest checkin with that tag; saying `fossil update` in a
checkout made from a tag will fast-forward you to the tip of the branch
that tag was made on.

(In Fossil, tags and branches are related, but the details are beyond
our scope here.  See the [Fossil Quick Start Guide][fqsg] and the
documents it links to for more details.)

This directory scheme shows an important difference between Fossil and
Git: with Git, the checkout and the clone are intermingled in the same
directory tree, but in Fossil, they are strictly separate.  Git can
emulate Fossil's normal working style through its [worktree][gitwt]
feature, but it's a kind of lash-up using symlinks and such, whereas
with Fossil, there is no confusion: the repository clone is a single
SQLite database file — here, `pidp8i.fossil` — and the checkouts are
made from the contents of that database.

Another important difference relative to Git is that with Fossil, local
checkins attempt to automatically sync checked-in changes back to the
repository you cloned from.  (This only works if you have a login on the
remote repository, the subject of the next section.)  This solves a
number of problems with Git, all stemming from the fact that Git almost
actively tries to make sure every clone differs from every other in some
important way.

While Fossil does allow offline operation and local independent clones,
its default mode of operation is to try and keep the clones in sync as
much as possible.  Git works the way it does because it was designed to
meet the needs of the Linux kernel development project, which is
inherently federated, so Git tries to operate in a federated model as
well.  Fossil is better for smaller, more coherent teams, where there is
a single, clear goal for the project and a single source for its
official code.  Fossil helps remote developers cooperate, whereas Git
helps remote developers go off on their own tangents for extended
periods of time and optionally sync back up with each other
occasionally.

Fossil is a better match for the way the PiDP-8/I software project
works: we want you to cooperate closely with us, not go off on wild
tangents.

[gitwt]:  https://git-scm.com/docs/git-worktree


Fossil Developer Access
----

If you have a developer account on tangentsoft.com's Fossil instance, just
add your username to the URL like so:

    $ fossil clone http://username@tangentsoft.com/pidp8i pidp8i.fossil

Fossil will ask you for the password for `username` on the remote Fossil
instance, and it will offer to remember it for you.  If you let it
remember the password, operation from then on is scarcely different from
working with an anonymous clone, except that on checkin, your changes
will be sync'd back to the repository on tangentsoft.com if you're
online at the time.

If you're working offline, Fossil will still do the checkin, but you'll
be able to sync with the central repoisitory once you get back online.
It is best to work on a branch when unable to use Fossil's autosync
feature, as you are less likely to have a sync conflict when attempting
to send a new branch to the central server than in attempting to merge
your changes to the tip of trunk into the current upstream trunk, which
may well have changed since you went offline.

You can purposely work offline by disabling autosync mode:

    $ fossil set autosync 0

Until you re-enable it (`autosync 1`) Fossil will stop trying to sync
your local changes back to the central repo.  In this mode, Fossil works
more like Git's default mode, buying you many of the same problems that
go along with that working style.  I recommend disabling autosync mode
only when you are truly going to be offline, and don't want Fossil
attempting to sync when you know it will fail.


Getting Developer Access
----

The administrator of this repository is Warren Young, whose email you
can find on the [official PiDP-8/I project mailing list][ggml].  I
generally give developer access to anyone who makes a reasonable
request.


Creating Branches
----

Creating a branch in Fossil is scary-simple, to the point that those
coming from other version control systems may ask, "Is that really all
there is to it?"  Yes, really, this is it:

    $ fossil ci --branch new-branch-name

That is to say, you make your changes as you normally would; then when
you go to check them in, you give the `--branch` option to the
`ci/checkin` command to put the changes on a new branch, rather than add
them to the same branch the changes were made against.

While developers with login rights to the PiDP-8/I Fossil instance are
allowed to check in on the trunk at any time, we recommend using
branches whenever you're working on something experimental, or where you
can't make the necessary changes in a single coherent checkin.
Basically, `trunk` should always build without error, and it should
always function correctly.  Branches are for isolating work until it is
ready to merge into the trunk.

Here again we have a difference with Git: because Fossil normally syncs
your work back to the central repository, this means we get to see the
branches you are still working on.  This is a *good thing*.  Do not fear
committing broken or otherwise bad code to a branch.  [You are not your
code.][daff]  We are software developers, too: we understand that
software development is an iterative process, and that not all ideas
spring forth perfect and production-ready from the fingers of its
developers.  These public branches let your collaborators see what
you're up to, and maybe lend advice or a hand in the work, but mostly
public branches let your collaborators see what you're up to, so they're
not surprised when the change finally lands in trunk.

This is part of what I mean about Fossil fostering close cooperation
rather than fostering wild tangents.

Jim McCarthy (author of [Dynamics of Software Development][dosd]) has a
presentation on YouTube that touches on this topic at a couple of
points:

* [Don't go dark](https://www.youtube.com/watch?v=9OJ9hplU8XA)
* [Beware of a guy in a room](https://www.youtube.com/watch?v=oY6BCHqEbyc)

Fossil's sync-by-default behavior fights these negative tendencies.

PiDP-8/I project developers are welcome to create branches at will. The
main rule is to follow the branch naming scheme: all lowercase with
hyphens separating words. See the [available branch list][brlist] for
examples to emulate.

[brlist]: https://tangentsoft.com/pidp8i/brlist
[daff]:   http://www.hanselman.com/blog/YouAreNotYourCode.aspx
[dosd]:   http://amzn.to/2iEVoBL


Debug Builds
----

By default, the build system creates a release build, but you can force
it to produce a binary without as much optimization and with debug
symbols included:

     $ ./configure --debug-mode


Manipulating the Build System Source Files
----

The [autosetup build system][asbs] is composed of these files and
directories:

     auto.def
     autosetup/
     configure
     Makefile.in

Unlike with GNU Autoconf, which you may be familiar with, the
`configure` script is not output from some other tool.  It is just a
driver for the Tcl and C code under the `autosetup` directory.  If you
have to modify any of these files to get some needed effect, you should
try to get that change into the upstream project, then merge that change
down into the local copy when it lands upstream.

The bulk of the customization to the build system is in `auto.def`,
which is a Tcl script run by `autosetup` via the `configure` script.
Some knowledge of [Tcl syntax][tcldoc] will therefore be helpful in
modifying it.

If you do not have Tcl installed on your system, `configure` builds a
minimal Tcl interpreter called `jimsh0`, based on the [Jim Tcl][jim]
project.  Developers working on the build system are encoruaged to use
this stripped-down version of Tcl rather than "real" Tcl because Jim Tcl
is more or less a strict subset of Tcl, so any changes you make that
work with the `jimsh0` interpreter should also work with "real" Tcl, but
not vice versa.  If you have Tcl installed and don't really need it,
consider uninstalling it to force `autosetup` to build and use `jimsh0`.

The `Makefile.in` file is largely a standard [GNU `make`][gmake] file
excepting only that it has variables substituted into it by
[`autosetup`][asbs] using its `@VARIABLE@` syntax.  At this time, we do
not attempt to achieve compatibility with other `make` programs, though
in the future we may need it to work with [BSD `make`][bmake] as well,
so if you are adding features, you might want to stick to the common
subset of features implemented by both the GNU and BSD flavors of
`make`.  We do not anticpate any need to support any other `make`
flavors.

(This, by the way, is why we're not using some heavy-weight build system
such as the GNU Autotools, CMake, etc.  The primary advantage of GNU
Autotools is that you can generate source packages that will configure
and build on weird and ancient flavors of Unix; we don't need that.
Cross-platform build systems such as CMake ease building the same
software on multiple disparate platforms straightforward, but the
PiDP-8/I software is built primarily on and for a single operating
system, Rasbpian Linux.  It also happens to build and run on other
modern Unix and Linux systems, for which we also do not need the full
power of something like CMake.  `autosetup` and GNU `make` suffice for
our purposes here.)

[asbs]:   http://msteveb.github.io/autosetup/
[bmake]:  https://www.freebsd.org/doc/en/books/developers-handbook/tools-make.html
[gmake]:  https://www.gnu.org/software/make/
[jim]:    http://jim.tcl.tk/
[tcldoc]: http://wiki.tcl.tk/11485


Submitting Patches
----

If you do not have a developer login on the PiDP-8/I software
repository, you can still send changes to the project.

The simplest way is to say this after developing your change against the
trunk of PiDP-8/I:

    $ fossil diff > my-changes.patch

Then attach that file to a new [PiDP-8/I mailing list][ggml] message
along with a declaration of the license you wish to contribute your
changes under.  We suggest using the [SIMH license][simhl], but any
[non-viral][viral] [OSI-approved license][osil] should suffice.

If your change is more than a small patch, `fossil diff` might not
incorporate all of the changes you have made.  The old unified `diff`
format can't encode branch names, file renamings, file deletions, tags,
checkin comments, and other Fossil-specific information.  For such
changes, it is better to send a Fossil bundle:

    $ fossil set autosync 0                # disable autosync
    $ fossil checkin --branch my-changes
      ...followed by more checkins on that branch...
    $ fossil bundle export --branch my-changes my-changes.bundle

After that first `fossil checkin --branch ...` command, any subsequent
changes will also be made on that branch without needing a `--branch`
option until you explicitly switch to some other branch.  This lets you
build up a larger change on a private branch until you're ready to
submit the whole thing as a bundle.

Because you are working on a branch on your private copy of the
PiDP-8/I Fossil repository, you are free to make as many checkins as
you like on the new branch before giving the `bundle export` command.

Once you are done with the bundle, send it to the mailing list just as
with the patch.

If you provide a quality patch, we are likely to offer you a developer
login on [the repository][repo] so you don't have to continue with the
patch or bundle methods.

Please make your patches or experimental branch bundles against the tip
of the current trunk.  PiDP-8/I often drifts enough during development
that a patch against a stable release may not apply to the trunk cleanly
otherwise.

[osil]:  https://opensource.org/licenses
[repo]:  http://tangentsoft.com/pidp8i/
[simhl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[viral]: https://en.wikipedia.org/wiki/Viral_license


The PiDP-8/I Software Project Code Style Rules
----

Every code base should have a common code style.  Love it or
hate it, here are PiDP-8/I's current code style rules:

**C Source Code**

File types: `c`, `h`, `c.in`

We follow the SIMH project's pre-existing code style when modifying one
of its source files:

*   Spaces for indents, size 4

*   DOS line endings.  (Yes, even though this is a Linux-based project!
    All decent Linux text editors can cope with this.)

*   Function, structure, type, and variable names are all lowercase,
    with underscores separating words

*   Macro names are in `ALL_UPPERCASE_WITH_UNDERSCORES`

*   Whitespace in the SIMH C files is of a style I have never seen
    anywhere else in my decades of software development.  This example
    shows the important features:

        int some_function (char some_parameter)
        {
        int some_variable = 0;

        if (some_parameter != '\0') {
            int nbytes = sizeof (some_parameter);
            char *buffer = malloc (4 * nbytes);

            switch (some_parameter) {
                case 'a':
                    do_something_with_buffer ((char *)buffer); 
                default:
                    do_something_else ();
                }
            }
        else {
            some_other_function (with_a_large, "number of parameters",
                wraps_with_a_single, "indent level");
            printf (stderr, "Failed to allocate buffer.\n");
            }
        }
    
    It is vaguely like K&R C style except that:

    -   The top level of statements in a function are not indented

    -   The closing curly brace is indented to the same level as the
        statement(s) it contains

    -   There is a space before all opening parentheses, not just those
        used in `if`, `while`, and similar flow control statements.

        Nested open parentheses do not have extra spaces, however.  Only
        the outer opening parenthesis has a space separating it from
        what went before.

    -   Multiple variables declared together don't have their types and
        variable names aligned in columns.

I find that this style is mostly sensible, but with two serious problems:
I find the indented closing curly braces confusing, and I find that the
loss of the first indent level for the statements inside a function makes
functions all visually run together in a screenful of code.  Therefore,
when we have the luxury to be working on a file separate from SIMH,
we use a variant of its style with these two changes, which you can
produce with the `tools/restyle` command.  See its internal comments for
details.

That gives the following, when applied to the above example:

        int some_function (char some_parameter)
        {
            int some_variable = 0;

            if (some_parameter != '\0') {
                int nbytes = sizeof (some_parameter);
                char *buffer = malloc (4 * nbytes);

                switch (some_parameter) {
                    case 'a':
                        do_something_with_buffer ((char *)buffer); 
                    default:
                        do_something_else ();
                }
            }
            else {
                some_other_function (with_a_large, "number of parameters",
                    wraps_with_a_single, "indent level");
                printf (stderr, "Failed to allocate buffer.\n");
            }
        }
    
If that looks greatly different, realize that it is just two indenting
level differences: add one indent at function level, except for the
closing braces, which we leave at their previous position.

SIMH occasionally exceeds 100-column lines.  I recommend breaking
long lines at 72 columns.  Call me an 80-column traditionalist.

When in doubt, mimic what you see in the current code.  When still in
doubt, ask on the mailing list.

[indent]: http://linux.die.net/man/1/indent


**Plain Text Files**

File types: `md`, `txt`

*   Spaces for indents, size 4.

*   Unix line endings.  The only common text editor I'm aware of that
    has a problem with this is Notepad, and people looking at these
    files anywhere other than unpacked on a Raspberry Pi box are
    probably looking at them through the Fossil web interface on
    tangentsoft.com.

*   Markdown files must follow the syntax flavor understood by
    [Fossil's Markdown interpreter][fmd].

[fmd]: https://tangentsoft.com/pidp8i/md_rules
