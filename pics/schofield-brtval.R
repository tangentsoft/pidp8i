# schofield-brtval.R: Generate SVG graph from data output by the C
#    program of the same name, written to a CSV file of the same name.
#
# The schofield-brtval.svg file checked into Fossil is modified from the
# version output by this program:
#
# 1. The colors are modified to match the scheme on tangentsoft.com/pidp8i
#
# 2. The data line thickness was increased
#
# 3. The data lines were smoothed by Inkscape's "simplify" function
#
# Copyright © 2017 Warren Young
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS LISTED ABOVE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#
# Except as contained in this notice, the names of the authors above shall
# not be used in advertising or otherwise to promote the sale, use or other
# dealings in this Software without prior written authorization from those
# authors.
#

library(readr)
rawData <- read_csv("schofield-brtval.csv")
dts <- ts(rawData)
svg("schofield-brtval.svg", width=5, height=3.6)
plot.ts(dts, plot.type='single', ylab='Brightness', yaxp=c(0, 32, 16))
dev.off()