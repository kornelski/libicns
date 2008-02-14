# Makefile for Icns2PNG, MacOS X / Linux
#
# Copyright (C) 2008 Mathew Eis <mathew@eisbox.net>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# VERSION 2 of the License, or (at your option) any later VERSION.
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

# Version information
LIBNAME = libicns
LIBVERMAJ = 0
LIBVERMIN = 0.8

VERSION = $(LIBVERMAJ).$(LIBVERMIN)
LIBSTATIC = $(LIBNAME).a
LIBSO = $(LIBNAME).so
LIBSOMAJ = $(LIBSO).$(LIBVERMAJ)
LIBSOVER = $(LIBSO).$(VERSION)

LIBHDR = icns.h

# Utilities
CC=gcc
AR_RC=ar rc
RANLIB=ranlib
MAKE=make

# Where make install puts icns2png
PREFIX=/usr/local
LIBPATH=$(PREFIX)/lib
BINPATH=$(PREFIX)/bin
INCPATH=$(PREFIX)/include

# Compiler flags
CFLAGS+=-Wall -g

# Linker flags
LDFLAGS+=-L. -L$(LIBPATH) -lz -lm -lpng12 -lopenjpeg
LDFLAGS_A+=$(LDFLAGS) $(LIBSTATIC)
LDFLAGS_SO+=$(LDFLAGS) -Wl,-rpath,. -licns

# Library object files
LIBOBJS = icns.o 
LIBOBJSDLL = $(LIBOBJS:.o=.pic.o)

# Utility programs included in library
LIBUTILS = icns2png icontainer2icns

# icns2png object files
UTILOBJS = icns2png.o jp2dec.o pngwriter.o

# Subdirectories to make
SUBDIRS = $(TESTDIR)

# Macros for makefile
.SUFFIXES:	.c .o .pic.o .icns .rsrc .bin .png

.c.pic.o:
	$(CC) -c $(CFLAGS) -fPIC -o $@ $*.c

# test files
TESTFILES = test1.png test2.png test3.png
TESTUTIL = ./icns2png

# Macros for testing
.SECONDEXPANSION:
.icns.png:
	$(TESTUTIL) $*.icns	
.rsrc.png:
	$(TESTUTIL) $*.rsrc
.bin.png:
	$(TESTUTIL) $*.bin

all: $(LIBSTATIC) $(LIBSO) $(LIBUTILS)

$(LIBSTATIC): $(LIBOBJS)
	$(AR_RC) $@ $(LIBOBJS)
	$(RANLIB) $@

$(LIBSO): $(LIBSOMAJ)
	ln -sf $(LIBSOMAJ) $(LIBSO)

$(LIBSOMAJ): $(LIBSOVER)
	ln -sf $(LIBSOVER) $(LIBSOMAJ)

$(LIBSOVER): $(LIBOBJSDLL)
	$(CC) -shared -W1,-coname,$(LIBSOMAJ) -o $(LIBSOVER) $(LIBOBJSDLL)

icns2png: $(UTILOBJS) $(LIBSO)
	$(CC) -o icns2png $(CFLAGS) $(UTILOBJS) $(LDFLAGS_SO)

icontainer2icns:
	$(CC) -o icontainer2icns icontainer2icns.c

test: $(LIBUTILS) $$(TESTFILES)

install-headers: $(LIBHDR)
	cp $(LIBHDR) $(INCPATH)/
	chmod 644 $(INCPATH)/$(LIBHDR)

install-static: install-headers $(LIBSTATIC) 
	-@if [ ! -d $(LIBPATH) ]; then mkdir -p $(LIBPATH); fi
	cp $(LIBSTATIC) $(LIBPATH)/
	chmod 644 $(LIBPATH)/$(LIBSTATIC)

install-shared: install-headers $(LIBSO)
	-@if [ ! -d $(LIBPATH) ]; then mkdir -p $(LIBPATH); fi	
	cp $(LIBSOVER) $(LIBPATH)/
	chmod 755 $(LIBPATH)/$(LIBSOVER)
	(cd $(LIBPATH); \
	ln -sf $(LIBSOVER) $(LIBSOMAJ); \
	ln -sf $(LIBSOMAJ) $(LIBSO))

install-utils:
	-@if [ ! -d $(BINPATH) ]; then mkdir -p $(BINPATH); fi
	install -m 755 icns2png $(BINPATH)
	install -m 755 icontainer2icns $(BINPATH)

install: install-static install-shared install-utils
	
clean:
	rm -f $(LIBSTATIC) $(LIBSO) $(LIBSOMAJ) $(LIBSOVER) \
		*.o $(LIBUTILS) $(TESTFILES)


