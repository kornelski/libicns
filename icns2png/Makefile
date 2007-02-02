# Makefile for Icns2PNG, MacOS X / Linux
#
# Copyright (C) 2002 Mathew Eis <mathew@bearca.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public
# License along with this library; if not, write to the
# Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.
#

# version number
version=0.5

# where make install puts icns2png
prefix=/usr/local

CFLAGS+=-Wall -O
LDFLAGS+=-lpng -lz -lm

OBJS = icns2png.o iconvert.o pngwriter.o byteswap.o

all: icns2png

test: icns2png
	@cd test; rm -f Gnu_I.png Gnu_II.png Gnu_III.png; \
	if ../icns2png Gnu_I.icns Gnu_II.rsrc Gnu_III.bin; then \
		echo 'Icns2PNG test successful!'; \
	else \
		echo 'Icns2PNG test failed internal error'; \
	fi; cd ..;

install: icns2png
	install -m 755 icon2png $(prefix)/bin

release: icns2png test
	@mkdir -p icon2png-$(version); \
	cp icns2png icon2png-$(version); \
	cp README icon2png-$(version); \
	rm -f icon2png-$(version).tar; \
	tar -cf icon2png-$(version).tar icon2png-$(version); \
	rm -rf icon2png-$(version); \
	echo 'Icns2PNG $(version) release package created.'; \
	rm -f *.o;

icns2png: $(OBJS)
	$(CC) -o icns2png $(CFLAGS) $(OBJS) $(LDFLAGS)

clean:
	rm -f *.o icns2png
