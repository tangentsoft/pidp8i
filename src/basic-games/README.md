BASIC Games and Demos

Many of them were ported to OS/8 BASIC by DEC employee Kay R. Fisher.  Most were published in the DEC publication, *101 BASIC Computer Games*. That book describes itself as follows:

>It is the first collection of games all in BASIC. It is also the only collection that contains both a complete listing and a sample run of each game along with a descriptive write-up.

The first printing was in 1973.  A quite readable preserved version of the 1975 edition can be [found online][book]. If a demo is from this book, it is designated `-101-` instead of `-----` below.  If the demo is from the book, there will be a page or two of useful lore, and a sample run is provided.

[book]: http://bitsavers.trailing-edge.com/pdf/dec/_Books/101_BASIC_Computer_Games_Mar75.pdf

| File name | Src | Description
| ---------- | --- | ------------
| `BINGO .BA` | `-101-` | Computer generates bingo card for you and itself and randomly calls out numbers at random.
| `BLKJAC.BA` | `-101-` | Play the card game of 21 against the computer.
| `BLKJAK.BA` | `-101-` | Version of Blackjack written for PDP-8 Edusystem 30 BASIC by Tom Kloos, Oregon MuseUm of Science and Industry.
| `BUNNY .BA` | `-101-` | Prints the Playboy rabbit as typewriter art.
| `CALNDR.BA` | `-101-` | Perpetual calendar program, by Geoffrey Chase, OSB, Portsmouth Abbey School.  You must modify line 160 to name the day of the week for January 1 of your year.  (0 for Sunday, -1 for Monday, etc.).  You also need to modify lines 360 and 620 for leap years: Change the number 365 on line 360 to 366, and change the third element of the array in line 620 from 28 to 29.
| `CHECKR.BA` | `-101-` | Written by Alan J. Segal. Play checkers against the computer.
| `CRAPS .BA` |  `-----`  | The dice game of craps.  Surprisingly it's not the version from *101 BASIC Computer Games*.
| `DICE  .BA` | `-101-` | Simulates rolling of dice, and prints the distribution of values returned.
| `FOOTBL.BA` | `-101-` | The first of two football simulations found in *101 BASIC Computer Games*.
| `FOTBAL.BA` | `-101-` | The second of two football simulations found in *101 BASIC Computer Games*. Written by Raymond W. Miseyka, Butler Sr. High School, Butler, PA.
| `GOLF  .BA` | `-----` | A not so great golf simulation.  The one in *101 BASIC Computer Games* looks a lot better.
| `HELLO .BA` | `-101-` | Simple conversation program where Petey P. Eight gives advice.
| `HOCKEY.BA` | `-101-` | Simulation of regulation hockey game.  Written by Charles Buttrey, Eaglebrook School, Deerfield, MA .
| `KING  .BA` | `-101-` | Land management simulation by James A. Storer, Lexington High School, Lexington, MA. Missing a bit of the text output in the game published in the 1975 edition of *101 BASIC Computer Games*, but functionally equivalent.
| `LIFE  .BA` | `-101-` | Conway's game of life, written in BASIC by Clark Baker, Project DELTA, Delaware School Auxiliary Association, Newport, Delaware.
| `LIFE2 .BA` | `-101-` | Two players place pieces on the board and the rules of Conway's game of life determines who survives. Written by Brian Wyvill, Bradford University, Bradford, Yorkshire, England.
| `MONPLY.BA` | `-101-` | Simulation of the board game, Monopoly, by David Barker, Southeastern State College, Durant, OK. Ported to OS/8 BASIC by Kay R. Fisher who eliminated the original RSTS-E virtual files.
| `POKER .BA` | `-101-` | Play poker against the computer. Original author: A. Christopher Hall, Trinity College, Hartford, CT. Re-ported to OS/8 BASIC from [CPM Users Group POKER.BAS](https://amaus.org/static/S100/MESSAGE%20BOARDS/CPM%20Users%20Group/cpmug020/POKER.BAS) source by Bill Cattey.  CPM User Group posting had missing bits, and a couple constructs OS/8 BASIC didn't like.
| `RESEQ.BA` | `-----` | Re-sequence line numbers of a BASIC program. Not from the DEC BASIC book.
| `ROCKET.BA` | `-101-` | Lunar lander simulation. Written by Jim Storer, Lexington High School.  Ported from Focal by David Ahl, Digital. Lost for a while due to bad disk bits.  Using a partial copy from [Dave Gesswein's archive](http://www.pdp8online.com/pdp8cgi/os8_html/ROCKET.BA?act=file;fn=images/os8/diag-games-kermit.rk05;to=ascii;blk=561,9,1;plain=1), a RSTS-11 version was found at [pdp-11.trailing-edge.com](http://pdp-11.trailing-edge.com/rsts11/rsts-11-013/ROCKET.BAS) and re-ported to OS/8 by Bill Cattey, converting RSTS BASIC constructs to OS/8 constructs, and eliminating some the ON ERROR GOTO stuff that was added to the RSTS version but does not exist in OS/8. This code diverged significantly from the version appearing in *101 BASIC Computer Games*, but is functionally equivalent.
| `ROCKT1.BA` | `-101-` | Another Lunar Lander Simulator. Written by Eric Peters, Digital. Thought lost.  Recovered from [Dave Gesswein's Archive](http://www.pdp8online.com/pdp8cgi/os8_html/ROCKT1.BA?act=file;fn=images/os8/diag-games-kermit.rk05;to=ascii;blk=570,8,1;plain=1).
| `ROULET.BA` | `-----` | European Roulette Wheel game.  Written by David Joslin.  Converted to BASIC-PLUS by David Ahl, Digital.  Ported to OS/8 BASIC By Kay R. Fisher, DEC.  Thought lost. Recoverdd from [Dave Gesswein's Archive](http://www.pdp8online.com/pdp8cgi/os8_html/ROULET.BA?act=file;fn=images/os8/diag-games-kermit.rk05;to=ascii;blk=578,17,1;plain=1)
| `SIGNS .BA` | `-----` | Program to print posters by Daniel R. Vernon, Butler High School, Butler PA.  When I tested the program under OS/8 BASIC I kept getting `SU  AT LINE 00261` which is a Subscript out of Bounds error.
| `SNOOPY.BA` | `-----` | The old "Curse You Red Barron" Snoopy poster.
| `SPACWR.BA` | `-101-` | Space war game based on classic Star Trek.  Game written by Mike Mayfield, Centerline Engineering.
| `TICTAC.BA` | `-----` | Play tic-tac-toe with the computer.  Simple version.  Thought lost.  Recovered from [Dave Gesswein's Archive](http://www.pdp8online.com/pdp8cgi/os8_html/TICTAC.BA?act=file;fn=images/os8/diag-games-kermit.rk05;to=ascii;blk=595,7,1;plain=1).
| `WAR   .BA` | `-101-` | Play the game of war against the computer.  Was thought lost.  Recovered from [Dave Gesswein's Archive](). However, that port to OS/8 BASIC contained a bug and could never run.  Line 230 defines the array for the cards. In OS/8 BASIC the maximum string size needed to be specified, as well as the number of strings.  230 DIM A$(52,3),L(54) will not accomodate the 10-value card.  The 3 needed to be a 4.  Program fixed and run-tested.
| `WAR2  .BA` | `-101-` | Written by Bob Dores, Milton, MA. A battle with 72,000 soldiers you deploy to Army, Navy, and Air Force against the computer.
| `WEKDAY.BA` | `-101-` | Input a birth date and learn fun facts like how many years have been spent sleeping in the elapsed time.  Written by Tom Kloos, Oregon Museum of Science and 'Industry.
| `WUMPUS.BA` | `-----` | Hunt the wumpus.
| `YAHTZE.BA` | `-101-` | Dice game of Yahtze.  Author unknown.  Quite an elaborate and comprehensive implementation.  


## License

Copyright © 2017 by Bill Cattey. This document is licensed under the terms of [the SIMH license][sl].

[sl]:   https://tangentsoft.com/pidp8i/doc/trunk/SIMH-LICENSE.md
