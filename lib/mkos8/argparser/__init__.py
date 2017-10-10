#!/usr/bin/python
# -*- coding: utf-8 -*-
########################################################################
# argparse.py - Extend ArgumentParser to add mkos8 bits.
#
# Copyright © 2017 by Warren Young
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

import argparse

class ArgParser (argparse.ArgumentParser):
  def __init__ (self, allowed_acts):
    argparse.ArgumentParser.__init__ (self,
        description = 'Build OS/8 RK05 disk images')

    self.add_bool ('-v', '--verbose',
            help = 'verbose SIMH output instead of progress messages')
    self.add_bool ('--enable-music',
            help = 'add *.MU files to binary disk')
    self.add_bool ('--disable-ba',
            help = 'leave BASIC games and demos off binary disk')
    self.add_bool ('--disable-cc8',
            help = 'leave CC8 off binary disk')
    self.add_bool ('--disable-dcp',
            help = 'leave DCP disassembler off binary disk')
    self.add_bool ('--disable-focal',
            help = 'leave FOCAL 69 and U/W FOCAL off binary disk')
    self.add_bool ('--enable-focal69',
            help = 'install FOCAL 69 on the binary disk')
    self.add_bool ('--disable-uwfocal',
            help = 'leave U/W FOCAL (only) off binary disk')
    self.add_bool ('--disable-fortran-iv',
            help = 'leave FORTRAN IV compiler off binary disk')
    self.add_bool ('--disable-init',
            help = 'suppress display of the INIT message on OS/8 boot')
    self.add_bool ('--disable-k12',
            help = 'leave 12-bit Kermit off binary disk')
    self.add_bool ('--disable-macrel',
            help = 'leave MACREL assembler off binary disk')
    self.add_bool ('--enable-vtedit',
            help = 'install and enable TECO VTEDIT mode')
    self.add_bool ('--disable-crt',
            help = 'console is a printing terminal and does not use ' +
                   'character overwrite on rubout')
    self.add_bool ('--disable-lcmod',
            help = 'disable the OS/8 command upcasing patch; best set ' +
                   'when SIMH is set to tti ksr mode')
    self.add_bool ('--disable-advent',
            help = 'leave game of Adventure off binary disk')
    self.add_bool ('--disable-chess',
            help = 'leave CHEKMO-II off binary disk')

    self.add_argument (
        'what',
        choices = allowed_acts,
        help    = 'select which RK05 media gets built; default is "all"',
        nargs   = argparse.REMAINDER)

    self.args = self.parse_args()

    if len (self.args.what) == 0: self.args.what = [ 'all' ]


  def add_bool (self, *args, **kwargs):
    kwargs['action']  = 'store_true'
    kwargs['default'] = False
    self.add_argument (*args, **kwargs)
