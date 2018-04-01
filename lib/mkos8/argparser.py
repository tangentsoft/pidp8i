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
import opts

class ArgParser (argparse.ArgumentParser):
  def __init__ (self, allowed_acts):
    # Call parent class ctor to initialize the arg parser
    argparse.ArgumentParser.__init__ (self,
        description = 'Build OS/8 RK05 disk images')

    # Add general-purpose args
    self.add_bool ('-d', '--debug',
            help = 'add extra debugging output, normally suppressed')
    self.add_bool ('-v', '--verbose',
            help = 'verbose SIMH output instead of progress messages')

    # Add arguments corresponding to --*-os8-* configure script options
    for obn, vals in opts.opts.iteritems():
      od = 'dis' if vals[0] else 'en'
      self.add_bool ('--' + od + 'able-' + obn, help = vals[1])

    # Add options that do not exactly mirror configuration options
    self.add_bool ('--disable-lcmod',
            help = 'disable the OS/8 command upcasing patch; best set ' +
                   'when SIMH is set to tti ksr mode')

    # Add the ability to feed in options.
    self.add_argument("--enable", nargs=1, action='append' )
    self.add_argument("--disable", nargs=1, action='append' )

    # Add trailing "what do do" argument
    self.add_argument (
        'what',
        choices = allowed_acts,
        help    = 'select which RK05 media gets built; default is "all"',
        nargs   = argparse.REMAINDER)

    # Finish initializing
    self.args = self.parse_args()
    if len (self.args.what) == 0: self.args.what = [ 'all' ]


  def add_bool (self, *args, **kwargs):
    kwargs['action']  = 'store_true'
    kwargs['default'] = False
    self.add_argument (*args, **kwargs)
