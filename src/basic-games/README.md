BASIC Games and Demos

These demos in BASIC can be found in the OS/8 RK05 system packs in the `RKB0:` partition.

Many of them were ported to OS/8 BASIC by DEC employee Kay R. Fisher.  Most were published in the DEC publication,
*101 BASIC Computer Games*. That book describes itself as follows:

>It is the first collection of games all in BASIC. It is also the only collection that contains both a complete listing and
a sample run of each game along with a descriptive write-up.

The first printing was in 1973.  A quite readable preserved version of the 1975 edition can be [found online][book].
If a demo is from this book, it is designated `-101-` instead of `-----` below.  If the demo is from the book,
there will be a page or two of useful lore, and a sample run is provided.

[book]: http://bitsavers.trailing-edge.com/pdf/dec/_Books/101_BASIC_Computer_Games_Mar75.pdf

| File name | Src | Description
| ---------- | --- | ------------
| `BINGO .BA` | `-101-` | Computer generates bingo card for you and itself and randomly calls out numbers at random.
| `BLKJAC.BA` | `-101-` | Play the card game of 21 against the computer.
| `BLKJAK.BA` | `-101-` | Blackjack written for PDP-8 Edusystem 30 BASIC by Tom Kloos, Oregon Museum of Science and Industry.
| `BUNNY .BA` | `-101-` | Prints the Playboy rabbit as typewriter art.
| `CALNDR.BA` | `-101-` | Perpetual calendar program, by Geoffrey Chase, OSB, Portsmouth Abbey School.  [Usage](#calndr)
| `CHECKR.BA` | `-101-` | Written by Alan J. Segal. Play checkers against the computer.
| `CRAPS .BA` |  `-----`  | The dice game of craps.  Surprisingly it's not the version from *101 BASIC Computer Games*.
| `DICE  .BA` | `-101-` | Simulates rolling of dice, and prints the distribution of values returned.
| `FOOTBL.BA` | `-101-` | The first of two football simulations from *101 BASIC Computer Games*.
| `FOTBAL.BA` | `-101-` | The other football game from *101 BASIC GAMES* by Raymond W. Miseyka, Butler Sr. High School, Butler, PA.
| `GOLF  .BA` | `-----` | A not so great golf simulation.  The one in *101 BASIC Computer Games* looks a lot better.
| `HELLO .BA` | `-101-` | Simple conversation program where Petey P. Eight gives advice.
| `HOCKEY.BA` | `-101-` | Simulation of regulation hockey game.  Written by Charles Buttrey, Eaglebrook School, Deerfield, MA .
| `KING  .BA` | `-101-` | Land management simulation by James A. Storer, Lexington High School, Lexington, MA. [History](#king)
| `LIFE  .BA` | `-101-` | Conway's game of life by Clark Baker, Project DELTA, Delaware School Auxiliary Assoc., Newport, Delaware.
| `LIFE2 .BA` | `-101-` | Two players put pieces on the board. Rules of Conway's game of life determines survivor. [History](#life2)
| `MONPLY.BA` | `-101-` | Monopoly board game simulation, by David Barker, Southeastern State College, Durant, OK. [History](#mon)
| `POKER .BA` | `-101-` | Play poker against the computer. [History](#poker)
| `RESEQ.BA` | `-----` | Re-sequence line numbers of a BASIC program. Not from the DEC BASIC book.
| `ROCKET.BA` | `-101-` | Lunar lander simulation. Written by Jim Storer, Lexington High School. [History](#rocket)
| `ROCKT1.BA` | `-101-` | Another Lunar Lander Simulator. Written by Eric Peters, Digital.[History](#rockt1)
| `ROULET.BA` | `-----` | European Roulette Wheel game.  Written by David Joslin. [History](#roulet)
| `SIGNS .BA` | `-----` | Program to print posters by Daniel R. Vernon, Butler High School, Butler PA. [Usage](#signs)
| `SNOOPY.BA` | `-----` | The old "Curse You Red Barron" Snoopy poster.
| `SPACWR.BA` | `-101-` | Space war game based on classic Star Trek.  Game written by Mike Mayfield, Centerline Engineering.
| `TICTAC.BA` | `-----` | Play tic-tac-toe with the computer.  Simple version. [History](#tictac)
| `WAR   .BA` | `-101-` | Play the game of war against the computer. [History](#war)
| `WAR2  .BA` | `-101-` | Deploy 72,000 soldiers to Army, Navy, and Air Force against the computer. [History](#war2)
| `WEKDAY.BA` | `-101-` | Input a birth date and learn fun facts about happenings in the elapsed time. [History](#wekday)
| `WUMPUS.BA` | `-----` | Hunt the wumpus.
| `YAHTZE.BA` | `-101-` | Dice game of Yahtze.  Author unknown.  Quite an elaborate and comprehensive implementation.

## Usage

### <a id="calndr"></a>`CALNDR.BA`
You must modify line 160 to name the day of the week for January 1 of your year.  (0 for Sunday, -1 for Monday, etc.).
You also need to modify lines 360 and 620 for leap years:
Change the number 365 on line 360 to 366, and change the third element of the array in line 620 from 28 to 29.

### <a id="signs"></a>`SIGNS.BA`

When tested the under OS/8 BASIC we get `SU  AT LINE 00261` which is a Subscript out of Bounds error.

## History

### <a id="king"></a>`KING.BA`

Missing a bit of the text output in the game published in the 1975 edition of *101 BASIC Computer Games*,
but functionally equivalent.

### <a id="life2"></a>`LIFE2.BA`

Written by Brian Wyvill, Bradford University, Bradford, Yorkshire, England.

### <a id="mon"></a>`MONPLY.BA`

Ported to OS/8 BASIC by Kay R. Fisher who eliminated the original RSTS-E virtual files.

### <a id="poker"></a>`POKER.BA`

Original author: A. Christopher Hall, Trinity College, Hartford, CT. Re-ported to OS/8 BASIC from
[CPM Users Group POKER.BAS][cpm-src] source by Bill Cattey fixing constructs OS/8 BASIC didn't like.

### <a id="rocket"></a>`ROCKET.BA`

Ported from Focal by David Ahl, Digital. Lost for a while due to bad disk bits.
Using a partial copy from [Dave Gesswein's archive][dgw-rocket], a RSTS-11 version
was found at [pdp-11.trailing-edge.com][trailing-edge] and re-ported to OS/8 by Bill Cattey, 
converting RSTS BASIC constructs to OS/8 constructs, and eliminating ON ERROR GOTO that does
not exist in OS/8. This code diverged significantly from the version appearing in *101 BASIC
Computer Games*, but is functionally equivalent.

### <a id="rockt1"></a>`ROCKT1.BA`

Thought lost.  Recovered from [Dave Gesswein's Archive][dgw-rockt1].

### <a id="roulet"></a>`ROULET.BA`

Converted to BASIC-PLUS by David Ahl, Digital.  Ported to OS/8 BASIC By Kay R. Fisher, DEC.  Thought lost.
Recovered from [Dave Gesswein's Archive][dgw-roulet].

### <a id="tictac"></a>`TICTAC.BA`

Thought lost.  Recovered from [Dave Gesswein's Archive][dgw-tictac].

### <a id="war"></a>`WAR.BA`

Was thought lost.  Recovered from [Dave Gesswein's Archive](). However, that port to OS/8
BASIC contained a bug and could never run.  Line 230 defines the array for the cards.
In OS/8 BASIC the maximum string size needed to be specified, as well as the number of strings.
`230 DIM A$(52,3),L(54)` will not accomodate the 10-value card.  The 3 needed to be a 4. 
Program fixed and run-tested.

### <a id="war2"></a>`WAR2.BA`

Written by Bob Dores, Milton, MA. 

### <a id="wekday"></a>`WEKDAY.BA`

Written by Tom Kloos, Oregon Museum of Science and 'Industry.

## License

Copyright © 2017 by Bill Cattey. This document is licensed under the terms of [the SIMH license][sl].
[cpm-src]: https://amaus.org/static/S100/MESSAGE%20BOARDS/CPM%20Users%20Group/cpmug020/POKER.BAS
[dgw-rocket]: http://www.pdp8online.com/pdp8cgi/os8_html/ROCKET.BA?act=file;fn=images/os8/diag-games-kermit.rk05;blk=561,9,1;to=ascii
[dgw-rockt1]: http://www.pdp8online.com/pdp8cgi/os8_html/ROCKT1.BA?act=file;fn=images/os8/diag-games-kermit.rk05;blk=570,8,1;to=auto
[dgw-roulet]: http://www.pdp8online.com/pdp8cgi/os8_html/ROULET.BA?act=file;fn=images/os8/diag-games-kermit.rk05;blk=578,17,1;to=auto
[dgw-tictac]: http://www.pdp8online.com/pdp8cgi/os8_html/TICTAC.BA?act=file;fn=images/os8/diag-games-kermit.rk05;blk=595,7,1;to=auto
[trailing-edge]: http://pdp-11.trailing-edge.com/rsts11/rsts-11-013/ROCKET.BAS
[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md

