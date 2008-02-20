# Makefile for libicns and icns2png
#
# Copyright (C) 2001-2008 Mathew Eis <mathew@eisbox.net>
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

# Grad operating system name
UNAME := $(shell uname)

# Version information
# Note, versions are in the format of the following
# 0.0.0a
# See DEVNOTES for more information on versioning
LIBNAME = libicns
LIBVERMAJ = 0
LIBVERMIN = 1.1a

VERSION = $(LIBVERMAJ).$(LIBVERMIN)
LIBSTATIC = $(LIBNAME).a
ifeq "$(UNAME)" "Darwin"
 LIBSO = $(LIBNAME).dylib
 LIBSOMAJ = $(LIBNAME).$(LIBVERMAJ).dylib
 LIBSOVER = $(LIBNAME).$(VERSION).dylib
else
 LIBSO = $(LIBNAME).so
 LIBSOMAJ = $(LIBSO).$(LIBVERMAJ)
 LIBSOVER = $(LIBSO).$(VERSION)
endif

LIBHDR = icns.h

# Utilities
CC=gcc
AR_RC=ar rc
RANLIB=ranlib
MAKE=make

# Where make install puts libicns and icns2png
PREFIX ?= /usr/local
LIBPATH=$(PREFIX)/lib
BINPATH=$(PREFIX)/bin
INCPATH=$(PREFIX)/include

# Compiler flags
CFLAGS+=-Wall -g -I$(INCPATH)

# Linker flags
LDFLAGS+=-L. -L$(LIBPATH) -lz -lm -lpng12 -lopenjpeg
LDFLAGS_A+=$(LDFLAGS) $(LIBSTATIC)
ifeq "$(UNAME)" "Darwin"
LDFLAGS_SO+=-licns $(LDFLAGS)
else
LDFLAGS_SO+=-Wl,-rpath,. -licns $(LDFLAGS)
endif

# Library object files
LIBOBJS = icns.o 
LIBOBJSDLL = $(LIBOBJS:.o=.pic.o)

# Utility programs included in library
LIBUTILS = icns2png icontainer2icns

# icns2png object files
UTILOBJS = icns2png.o

# Subdirectories to make
SUBDIRS = $(TESTDIR)

# Macros for makefile
.SUFFIXES:	.c .o .pic.o

.c.pic.o:
	$(CC) -c $(CFLAGS) -fPIC -o $@ $*.c

all: $(LIBSTATIC) $(LIBSO) $(LIBUTILS)

$(LIBSTATIC): $(LIBOBJS)
	$(AR_RC) $@ $(LIBOBJS)
	$(RANLIB) $@

$(LIBSO): $(LIBSOMAJ)
	ln -sf $(LIBSOMAJ) $(LIBSO)

$(LIBSOMAJ): $(LIBSOVER)
	ln -sf $(LIBSOVER) $(LIBSOMAJ)

$(LIBSOVER): $(LIBOBJSDLL)
ifeq "$(UNAME)" "Darwin"
	$(CC) -dynamiclib -install_name $(LIBSOMAJ) -o $(LIBSOVER) $(LIBOBJSDLL)
else
	$(CC) -shared -W1,-soname,$(LIBSOMAJ) -o $(LIBSOVER) $(LIBOBJSDLL)
endif

icns2png: $(UTILOBJS) $(LIBSO)
	$(CC) -o icns2png $(CFLAGS) $(UTILOBJS) $(LDFLAGS_SO)

icontainer2icns:
	$(CC) -o icontainer2icns icontainer2icns.c

test: $(LIBUTILS)
	+@echo 'Test: Converting icon in native icns format...'; \
	./icns2png test1.icns; \
	echo 'Test: Converting icon in mac resource...'; \
	./icns2png test2.rsrc; \
	echo 'Test: Converting icon in macbinary resource...'; \
	./icns2png test3.bin

install-headers: $(LIBHDR)
	cp $(LIBHDR) $(INCPATH)/
	chmod 644 $(INCPATH)/$(LIBHDR)

install-static: install-headers $(LIBSTATIC) 
	-@if [ ! -d $(LIBPATH) ]; then mkdir -p $(LIBPATH); fi
	cp $(LIBSTATIC) $(LIBPATH)/
	chmod 644 $(LIBPATH)/$(LIBSTATIC)

install-shared: install-headers $(LIBSO)
	-@if [ ! -d $(LIBPATH) ]; then mkdir -p $(LIBPATH); fi	
	if [ "$(UNAME)" = "Darwin" ]; then install_name_tool -id $(LIBPATH)/$(LIBSOMAJ) $(LIBSOVER); fi
	cp $(LIBSOVER) $(LIBPATH)/
	chmod 755 $(LIBPATH)/$(LIBSOVER)
	(cd $(LIBPATH); \
	ln -sf $(LIBSOVER) $(LIBSOMAJ); \
	ln -sf $(LIBSOMAJ) $(LIBSO))

install-utils: $(LIBUTILS)
	-@if [ ! -d $(BINPATH) ]; then mkdir -p $(BINPATH); fi
	install_name_tool -change $(LIBSOMAJ) $(LIBPATH)/$(LIBSOMAJ) icns2png
	install -m 755 icns2png $(BINPATH)
	install -m 755 icontainer2icns $(BINPATH)

install: install-static install-shared install-utils
	
clean:
	rm -f $(LIBSTATIC) $(LIBSO) $(LIBSOMAJ) $(LIBSOVER) \
		*.o $(LIBUTILS) *.png


