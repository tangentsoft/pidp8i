Hacking on the PiDP-8/I Software
====

If you are going to make any changes to the PiDP-8/I software, here are
some rules and hints to keep in mind while you work.


<a id="gs-fossil"></a>
Getting Started with Fossil
----

The PiDP-8/I software project is hosted using the [Fossil][fossil]
[distributed version control system][dvcs].  Fossil provides most of the
features of GitHub under a simpler operating model than Subversion
without tying you to a proprietary web service.

This guide will introduce you to some of the basics, but you should also
at least read the [Fossil Quick Start Guide][fqsg]. For a more thorough
introduction, I recommend [the Schimpf book][fbook]. If you have
questions about Fossil, you can ask on [the Fossil forum][ffor], where
I, your humble project maintainer, am active.

If you started with one of our PiDP-8/I binary OS images made in or
after April 2017, Fossil 2.x is already installed.

If you're starting from some other OS, you either won't have Fossil
installed at all, or you'll most likely be using an older version, since
the Debian project is still shipping version 1.37 and likely will
continue to do so until 2020 or so in accordance with their software
stability policy.  You could build Fossil from source, or you could just
go grab a pre-built binary we keep on the project site:

    $ wget https://tangentsoft.com/pidp8i/uv/fossil-2.7-raspbian-9.6-stretch
    $ sudo install -m 755 fossil-* /usr/local/bin/fossil

Fossil is also available for all common desktop platforms.  One of [the
official binaries][fbin] may work on your system.  If you're getting
binaries from a third party, be sure it is Fossil 2.1 or higher.


[fbin]:   https://fossil-scm.org/index.html/uv/download.html
[dvcs]:   https://en.wikipedia.org/wiki/Distributed_revision_control
[fbook]:  https://www.fossil-scm.org/schimpf-book/home
[ffor]:   https://fossil-scm.org/forum/
[fossil]: https://fossil-scm.org/
[fqsg]:   https://fossil-scm.org/index.html/doc/trunk/www/quickstart.wiki


<a id="fossil-anon"></a>
Fossil Anonymous Access
----

To clone the code repository anonymously, say:

    $ mkdir -p ~/museum ~/src/pidp8i/trunk
    $ fossil clone https://tangentsoft.com/pidp8i ~/museum/pidp8i.fossil
    $ cd ~/src/pidp8i/trunk
    $ fossil open ~/museum/pidp8i.fossil

The `clone` command gets you a file called `pidp8i.fossil` containing
the full history of the PiDP-8/I software project from the upstream
2015.12.15 release onward.  You can call that clone file anything you
like and put it in any directory you like.  Even the `.fossil` extension
is largely a convention, not a requirement.


<a id="tags" name="branches"></a>
Working with Existing Tags and Branches
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
Git: with Git, the checkout and the clone are normally intermingled in
the same directory tree, but in Fossil, the clone and the checkout are
always strictly separate.  Git has a weak emulation of Fossil's normal
working style via its [worktree][gitwt] feature, but most Git users
aren't even aware of the feature, and those that are aware of it tend to
discourage its use because of the problems it can cause.  Fossil was
designed to work this way from the start, so it doesn't have the
problems associated with Git worktrees.

Another important difference relative to Git is that with Fossil, local
checkins attempt to automatically sync checked-in changes back to the
repository you cloned from.  (This only works if you have a login on the
remote repository, the subject of the [next section](#login).)  This
solves a number of problems with Git, all stemming from the fact that
Git almost actively tries to make sure every clone differs from every
other in some important way.

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


<a id="login"></a>
Fossil Developer Access
----

If you have a developer account on the `tangentsoft.com/pidp8i` Fossil
instance, just add your username to the URL like so:

    $ fossil clone https://username@tangentsoft.com/pidp8i pidp8i.fossil

If you've already cloned anonymously, you don't have to clone again to
inform Fossil about your developer account.  Just do a manual sync from
within a PiDP-8/I checkout directory, changing the URL to include the
user name:

    $ fossil sync https://username@tangentsoft.com/pidp8i

Either way, Fossil will ask you for the password for `username` on the
remote Fossil instance, and it will offer to remember it for you.  If
you let it remember the password, operation from then on is scarcely
different from working with an anonymous clone, except that on checkin,
your changes will be sync’d back to the repository on tangentsoft.com if
you're online at the time, and you'll get credit under your developer
account name for the checkin.

If you're working offline, Fossil will still do the checkin locally, and
it will sync up with the central repository after you get back online.
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
only when you are truly going to be offline and don't want Fossil
attempting to sync when you know it will fail.


<a id="gda"></a>
Getting Developer Access
----

We are pretty open about giving developer access to someone who’s
provided at least one good, substantial [patch](#patches) to the
software. If we’ve accepted one of your patches, just ask for a
developer account [on the forum][pfor].


<a id="forum"></a>
Developer Discussion Forum
----

The "[Forum][pfor]" link at the top of the Fossil web interface is for
discussing the development of the PiDP-8/I software only. All other
traffic should go to [the mailing list][ggml] instead.  We're not trying
to split the community by providing a second discussion forum; we just
think many development-related discussions are too low-level to be of
any interest to most of the people on the mailing list.

We used to relegate such discussions to private email, but that was not
out of any wish to hide what we're doing. We just didn't have a good
place to do this work in public until recently.

You can sign up for the forums without having a developer login, and you
can even post anonymously. If you have a login, you can [sign up for
email alerts][alert] if you like.

Keep in mind that posts to the Fossil forum are treated much the same
way as ticket submissions and wiki articles. They are permanently
archived with the project. The "edit" feature of Fossil forums just
creates a replacement record for a post, but the old post is still
available in the repository. Don't post anything you wouldn't want made
part of the permanent record of the project!

[ggml]:  https://groups.google.com/forum/#!forum/pidp-8
[pfor]:  https://tangentsoft.com/pidp8i/forum
[alert]: https://tangentsoft.com/pidp8i/alerts


<a id="branching"></a>
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

One of this project's core principles is that `trunk` should always
build without error, and it should always function correctly.  That's an
ideal we have not always achieved, but we do always *try* to achieve it.

Contrast branches, which PiDP-8/I developers may use to isolate work
until it is ready to merge into the trunk.  It is okay to check work in
on a branch that doesn't work, or doesn't even *build*, so long as the
goal is to get it to a point that it does build and work properly before
merging it into trunk.

Here again we have a difference with Git: because Fossil normally syncs
your work back to the central repository, this means we get to see the
branches you are still working on.  This is a *good thing*.  Do not fear
committing broken or otherwise bad code to a branch.  [You are not your
code.][daff]  We are software developers, too: we understand that
software development is an iterative process, and that not all ideas
spring forth perfect and production-ready from the fingers of its
developers.  These public branches let your collaborators see what
you're up to, and maybe lend advice or a hand in the work; mostly,
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

If you have checkin rights on the repository, it is generally fine to
check things in on someone else's feature branch, as long as you do so
in a way that cooperates with the purpose of that branch.  The same is
true of `trunk`: you should not check something in directly on the trunk
that changes the nature of the software in a major way without
discussing the idea first.  This is yet another use for branches: to
make a possibly-controversial change so that it can be discussed before
being merged into the trunk.

[brlist]: https://tangentsoft.com/pidp8i/brlist
[daff]:   http://www.hanselman.com/blog/YouAreNotYourCode.aspx
[dosd]:   http://amzn.to/2iEVoBL


<a id="special"></a>
Special Branches
----

Most of the branches in the PiDP-8/I project are feature branches of the
sort described in the previous section: an isolated line of development
by one or more of the project's developers to work towards some new
feature, with the goal of merging that feature into the `trunk` branch.

There are a few branches in the project that are special, which are
subject to different rules than other branches:

*   **<code>release</code>** - One of the steps in the
    [release process][relpr] is to merge the stabilized `trunk` into the
    `release` branch, from which the release tarballs and binary OS
    images are created.  Only the project's release manager — currently
    Warren Young — should make changes to this branch.

*   **<code>bogus/BOGUS</code>** — Because a branch is basically just a
    label for a specific checkin, Fossil allows the tip of one branch to
    be "moved" to another branch by applying a branch label to that
    checkin.  We use this label when someone makes a checkin on the tip
    of a branch that should be "forgotten."  Fossil makes destroying
    project history very difficult, on purpose, so things moved to the
    "bogus" branch are not actually destroyed; instead, they are merely
    moved out of the way so that they do not interfere with that
    branch's normal purpose.

    If you find yourself needing to prune the tip of a branch this way,
    the simplest way is to do it via the web UI, using the checkin
    description page's "edit" link.  You can instead do it from the
    command line with the `fossil amend` command.

[relpr]:  https://tangentsoft.com/pidp8i/doc/trunk/doc/RELEASE-PROCESS.md


<a id="debug"></a>
Debug Builds
----

By default, the build system creates a release build, but you can force
it to produce a binary without as much optimization and with debug
symbols included:

     $ ./configure --debug-mode


<a id="build-system"></a>
Manipulating the Build System Source Files
----

The [autosetup build system][asbs] is composed of these files and
directories:

     auto.def
     autosetup/*
     configure
     Makefile.in

Unlike with GNU Autoconf, which you may be familiar with, the
`configure` script is not output from some other tool.  It is just a
driver for the Tcl and C code under the `autosetup` directory.

If you have to modify any of the files in `autosetup/` to get some
needed effect, you should try to get that change into the upstream
[Autosetup][asbs] project, then merge that change down into the local
copy when it lands upstream.

The bulk of the customization to the build system is in `auto.def`,
which is a Tcl script run by `autosetup` via the `configure` script.
Some knowledge of [Tcl syntax][tcldoc] will therefore be helpful in
modifying it.

If you do not have Tcl installed on your system, `configure` builds a
minimal Tcl interpreter called `jimsh0`, based on the [Jim Tcl][jim]
project.  Developers working on the build system are encouraged to use
this stripped-down version of Tcl rather than "real" Tcl because Jim Tcl
is more or less a strict subset of Tcl, so any changes you make that
work with the `jimsh0` interpreter should also work with "real" Tcl, but
not vice versa.  If you have Tcl installed and don't really need it,
consider uninstalling it to force Autosetup to build and use `jimsh0`.

The `Makefile.in` file is largely a standard [GNU `make`][gmake] file
excepting only that it has variables substituted into it by Autosetup
using its `@VARIABLE@` syntax.  At this time, we do not attempt to
achieve compatibility with other `make` programs, though in the future
we may need it to work with [BSD `make`][bmake] as well, so if you are
adding features, you might want to stick to the common subset of
features implemented by both the GNU and BSD flavors of `make`.  We do
not anticipate any need to support any other `make` flavors.

This, by the way, is why we're not using some heavy-weight build system
such as the GNU Autotools, CMake, etc.  The primary advantage of GNU
Autotools is that you can generate source packages that will configure
and build on weird and ancient flavors of Unix; we don't need that.
Cross-platform build systems such as CMake ease building the same
software on multiple disparate platforms straightforward, but the
PiDP-8/I software is built primarily on and for a single operating
system, Raspbian Linux.  It also happens to build and run on [several
other OSes][oscomp], for which we also do not need the full power of
something like CMake.  Autosetup and GNU `make` suffice for our purposes
here.

[asbs]:   http://msteveb.github.io/autosetup/
[bmake]:  https://www.freebsd.org/doc/en/books/developers-handbook/tools-make.html
[gmake]:  https://www.gnu.org/software/make/
[jim]:    http://jim.tcl.tk/
[oscomp]: https://tangentsoft.com/pidp8i/wiki?name=OS+Compatibility
[tcldoc]: http://wiki.tcl.tk/11485


<a id="dirs"></a>
Directory Structure
----

The directory structure of the PiDP-8/I project is as follows:

*   <b>`.`</b> - Top level, occupied only by the few files the end user
    of the source code needs immediately at hand on first unpacking the
    project: the top level build system files, key documentation, and
    licensing information. If a given file *can* be buried deeper, it
    *should* be buried to reduce clutter at this most precious level of
    the hierarchy.

*   <b>`.fossil-settings`</b> - Versioned settings for the Fossil build
    system which Fossil applies as defaults everywhere you check out a
    Fossil version.  Settings made here are intended to be correct for
    all users of the system; think of these not as expressing defaults
    but as expressing *policy*.  It is possible to override these
    settings, but we do not make settings here if we expect that some
    users may quibble with our choices here.

    Any setting whose value may vary between users of the Fossil
    repository should be done locally with a `fossil set` command.

    Say `fossil help set` at the command line for more on this.

*   <b>`autosetup`</b> - The bulk of the [Autosetup build system][asbs].
    These are generic files, not modified by the project itself. We
    occasionally run `tools/autosetup-update` to merge in upstream
    changes.

*   <b>`bin`</b> - Programs installed to `$prefix/bin`, which may also
    be run during development, if only to test changes to those
    programs.  Some scripts stored here are written in place by the
    project's developers, while other files in this directory are
    outputs of the build system.

    The content of this directory is copied to `$prefix/bin` at
    installation time, which is added to the user's `PATH` by the
    `make install` script.

*   <b>`boot`</b> - SIMH initialization scripts.  The `*.script.in`
    files are written by the project developers but have local
    configuration values substituted in by the `configure` script to
    produce a `*.script` version.  Scripts which need no config-time
    values substituted in are checked in directly as `*.script`.  The
    `*.script` files in this directory which do not fall into either of
    those categories are outputs of `tools/mkbootscript`, which produces
    them from `palbart` assembly listings.

    All of these `*.script` files are copied to `$prefix/share/boot` by
    `make mediainstall` which runs automatically from `make install`
    when we detect that the binary media and SIMH boot scripts have
    never been installed at this site before.  On subsequent installs,
    the user chooses whether to run `make mediainstall` by hand to
    overwrite all of this.

*   <b>`doc`</b> - Documentation files sufficiently unimportant to a new
    user of the software that they need not be at the top level of the
    project tree.  Such files can wait for new users to discover them.

    Fossil's [embedded documentation][edoc] feature allows us to present
    the contents of `doc` to web site users all but indistinguishably
    from a wiki page.  Why are there two different ways to achieve the
    same end, and how do we decide which mechanism to use?

    The rule is simple: if a given document's change history is tied to
    the history of the PiDP-8/I project itself, it goes in `doc`, else
    it goes in the wiki.  When checking out older versions of the
    PiDP-8/I software, you expect to roll back to contemporaneous
    versions of the project documentation, which is what happens to all
    files stored in the repository, including those in `doc`, but this
    does not happen to the wiki documents.  The wiki always presents the
    most current version, no matter what version you have locally
    checked out.

    (Fossil's wiki feature behaves much like Wikipedia: it keeps change
    history for wiki documents, but it always presents the most recent
    version unless you manually go poking around in the history to pull
    up old versions.  If you check out a historical version of the
    software and then say `fossil ui` within that checkout directory,
    the resulting web view still shows the most recent locally-available
    version of each wiki document, not versions of the wiki documents
    contemporaneous with the historical version of the Fossil tree you
    have checked out.)

    The `doc/graphics` subdirectory holds JPEGs and SVGs displayed
    inline within wiki articles.

*   <b>`etc`</b> - Files which get copied to `/etc` or one of its
    subdirectories at installation time.

    There is an exception: `pidp8i.service.in` does not get installed to
    `/etc` at install time, but only because systemd's [unit file load
    path scheme][uflp] is screwy: *some* unit files go in `/etc`, while
    others do not.  The systemd docs claim we can put user units in
    `/etc/systemd/user` but this does not appear to work on a Raspberry
    Pi running Raspbian Stretch at least.  We've fallen back to another
    directory that *does* work, which feels more right to us anyway, but
    which happens not to be in `/etc`.  If systemd were designed sanely,
    we'd install such files to `$HOME/etc/systemd` but noooo....

    Since none of the above actually argues for creating another
    top-level repository directory to hold this one file, we've chosen
    to store it in `etc`.

*   <b>`examples`</b> - Example programs for the end user's edification.
    Many of these are referenced by documentation files and therefore
    should not be renamed or moved, since there may be public web links
    referring to these examples.

*   <b>`hardware`</b> - Schematics and such for the PiDP-8/I board or
    associated hardware.

*   <b>`labels`</b> - Graphics intended to be printed out and used as
    labels for removable media.

*   <b>`lib`</b> - Library routines used by other programs, installed to
    `$prefix/lib`.

*   <b>`libexec`</b> - A logical extension of `lib`, these are
    standalone programs that nevertheless are intended to be run
    primarily by other programs.  Whereas a file in `lib` might have its
    interface described by a programmer's reference manual, the
    interface of a program in `libexec` is described by its usage
    message.  Example:

    *   <b>`scanswitch`</b> - Run by `etc/pidp8i`.
    
        <p>It is run by hand only by developers modifying its behavior.</p>

    Programs in `libexec` are installed to `$prefix/libexec`, which is
    *not* put into the user's `PATH`, on purpose.  If a program should
    end up in the user's `PATH`, it belongs in `bin`.  Alternately, a
    wrapper may be put in `bin` which calls a `libexec` program as a
    helper.

*   <b>`media`</b> - Binary media images used either by SIMH directly or
    by tools like `os8-run` to produce media used by SIMH.

    The contents of this tree are installed to `$prefix/share/media`.

*   <b>`obj`</b> - Intermediate output directory used by the build
    system.  It is safe to remove this directory at any time, as its
    contents may be recreated by `make`.  No file checked into Fossil
    should be placed here.

    (Contrast `bin` which does have some files checked into Fossil; all
    of the *other* files that end up in `bin` can be recreated by
    `make`, but not these few hand-written programs.)

*   <b>`src`</b> - Source code for the project's programs, especially
    those that cannot be used until they are built.  The great majority
    of these programs are written in C.  The build system's output
    directories are `bin`, `boot`, `libexec`, and `obj`.

    Programs that can be used without being "built," example programs,
    and single-file scripts are placed elsewhere: `bin`, `examples`,
    `libexec`, `tools`, etc.  Basically, we place such files where the
    build system *would* place them if they were built from something
    under `src`.

    There are no program sources in the top level of `src`.  The file
    `src/config.h` may appear to be an exception to that restriction,
    but it is *generated output* of the `configure` script, not "source
    code" *per se*.

    Multi-module programs each have their own subdirectory of `src`,
    each named after the program contained within.

    Single module programs live in `src/misc` or `src/asm`, depending on
    whether they are host-side C programs or PAL8 assembly programs.

*   <b>`test`</b> - Output directory used by `tools/test-*`.

*   <b>`tools`</b> - Programs run only during development and not
    installed.

    If a program is initially created here but we later decide that it
    should be installed for use by end users of the PiDP-8/I system, we
    move it to either `bin` or `libexec`, depending on whether it is run
    directly at the command line or run from some other program that is
    also installed, respectively.

[edoc]: https://www.fossil-scm.org/index.html/doc/trunk/www/embeddeddoc.wiki
[uflp]: https://www.freedesktop.org/software/systemd/man/systemd.unit.html#id-1.9


<a id="patches"></a>
Submitting Patches
----

If you do not have a developer login on the PiDP-8/I software
repository, you can still send changes to the project.

The simplest way is to say this after developing your change against the
trunk of PiDP-8/I:

    $ fossil diff > my-changes.patch

Then either upload that file somewhere (e.g. Pastebin) and point to it
from a [forum post][pfor] or attach the patch to a new [PiDP-8/I mailing
list][ggml] message. Either way, include a declaration of the license
you wish to contribute your changes under.  We suggest using the [SIMH
license][simhl], but any [non-viral][viral] [OSI-approved license][osil]
should suffice. We’re willing to tolerate viral licenses for standalone
products; for example, CC8 is under the GPL, but it’s fine because it
isn’t hard-linked into any other part of the PiDP-8/I software system.

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
`fossil ci` commands will check your changes in on that branch without
needing a `--branch` option until you explicitly switch that checkout
directory to some other branch.  This lets you build up a larger change
on a private branch until you're ready to submit the whole thing as a
bundle.

Because you are working on a branch on your private copy of the
PiDP-8/I Fossil repository, you are free to make as many checkins as
you like on the new branch before giving the `bundle export` command.

Once you are done with the bundle, send it to the developers the same
way you should a patch file.

If you provide a quality patch, we are likely to offer you a developer
login on [the repository][repo] so you don't have to continue with the
patch or bundle methods.

Please make your patches or experimental branch bundles against the tip
of the current trunk.  PiDP-8/I often drifts enough during development
that a patch against a stable release may not apply to the trunk cleanly
otherwise.

[osil]:  https://opensource.org/licenses
[repo]:  https://tangentsoft.com/pidp8i/
[simhl]: https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
[viral]: https://en.wikipedia.org/wiki/Viral_license


<a id="code-style"></a>
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
doubt, ask on the [project forum][pfor].

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


<a id="tickets"></a>
Ticket Processes
----
Normal end users of the Fossil ticket system are not expected to
understand it properly or to fill out tickets properly. Without certain
permissions, it is in fact not possible to completely fill out a ticket
properly.

Therefore, the first thing that should happen to a ticket is that
someone with sufficient privilege should triage it and fix up any
incorrect settings.

The Status of a ticket initially starts out as Open. If the person
triaging a ticket takes the time to check that the problem actually
occurs as the ticket filer claims or that the requested feature is in
fact missing, the Status should change to Verified.

If a developer implements a fix or feature in response to a ticket, he
has two choices: change the ticket’s Status to “Review” if he wants
someone to check out the change before closing it, or go straight to
Closed status. Closing a ticket hides it from the Wishes and Bugs ticket
reports.

Larger teams often require all ticket changes to go through Review
status before getting to Closed, but the PiDP-8/I project is too small
to require such ceremony: if we’ve given you a developer account on the
repository, you’re expected to resolve and close tickets in the same
step, most of the time. If you cannot confidently close a ticket when
resolving it, you should probably not be assigning a resolution yet
anyway. Do whatever you have to with tests and such to *become* certain
before you resolve a ticket.

There is a process interlock between the Resolution and Status settings
for a ticket: while Status is not Closed, Resolution should be Open.
When Resolution changes from Open, Status should change to either Review
or, preferentially, Closed.  A resolution is an end state, not an
expression of changeable intent: no more ceremony than setting a
ticket’s Resolution state *away* from Open and its Status *to* Closed is
required.

If you do not intend to close a ticket but wish to advocate for a
particular resolution, just add a comment to the ticket and let someone
else choose whether to close the ticket or not. Don’t change the
Resolution value until the issue has been *resolved* for good.

For example, the resolution "Works as Designed" does not merely mean,
“Yes, we know it works that way,” it also implies “...and we have no
intention to change that behavior, ever.”  If there is a chance that the
behavior described in the ticket could change, you should not assign any
resolution. Just leave it open until someone decides to do something
final with the ticket.

This is not to say that a ticket can never be re-opened once it’s had a
resolution assigned and been closed, but that this is a rare occurrence.
When a developer makes a decision about a ticket, it should be difficult
to re-open the issue. A rejected ticket probably shouldn’t be re-opened
with anything short of a working patch, for example:

> User A: I want feature X.

> Dev B: No, we’re not going to do that. Ticket closed and rejected.

> User C: Here’s a patch to implement feature X.

> Dev B: Well, that’s different, then. Thanks for the patch! Ticket
> marked Implemented, but still Closed.
