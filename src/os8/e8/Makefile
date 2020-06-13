# Makefile: Build E8 and its collateral files.
# 
# This file is © 2020 by Warren Young and is offered under the SIMH
# license.  See COPYING.md for details.

all: e8.pa e8all.pa

clean:
	rm -f e?.pa e8.zip e8all.pa doc/manual.pdf

pdf: doc/manual.pdf

zip: all
	zip -9j e8-dist.zip e*.pa *.md
	fossil uv add e8-dist.zip
	fossil uv sync

doc/manual.pdf: doc/manual.md
	tools/mkmanpdf

e8.pa: e8all.pa
	@tools/splitpal $<

e8all.pa: e8base.pa e8file.pa e8srch.pa
	@cat $^ > $@
