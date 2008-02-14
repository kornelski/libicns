# Makefile for Icns2PNG, MacOS X / Linux
#
# Copyright (C) 2008 Mathew Eis <mathew@eisbox.net>
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
version=0.8

# where make install puts icns2png
prefix=/usr/local

CFLAGS+=-Wall -g
LDFLAGS+=-lpng12 -lz -lm -lopenjpeg

OBJS = icns2png.o pngwriter.o jp2dec.o icns.o

CC=gcc

all: icns2png icontainer2icns

test: icns2png
	+@cd test; rm -f Gnu_I.png Gnu_II.png Gnu_III.png; \
	if ../icns2png Gnu_I.icns Gnu_II.rsrc Gnu_III.bin; then \
		echo 'Icns2PNG test successful!'; \
	else \
		echo 'Icns2PNG test failed internal error'; \
	fi; cd ..;

install: icns2png
	install -m 755 icns2png $(prefix)/bin
	install -m 755 icontainer2icns $(prefix)/bin
	
release: icns2png test
	@mkdir -p icns2png-$(version); \
	cp icns2png icns2png-$(version); \
	cp README icns2png-$(version); \
	rm -f icns2png-$(version).tar; \
	tar -jcf icns2png-$(version).tar.bz2 icns2png-$(version); \
	rm -rf icns2png-$(version); \
	echo 'Icns2PNG $(version) binary release package created.'; \
	rm -f #.o;

icns2png: $(OBJS)
	$(CC) -o icns2png $(CFLAGS) $(OBJS) $(LDFLAGS)

icontainer2icns:
	$(CC) -o icontainer2icns icontainer2icns.c 

clean:
	rm -f *.o icns2png icontainer2icns test/*.png

