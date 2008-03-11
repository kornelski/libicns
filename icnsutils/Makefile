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
#######################################################
# Please update the Makefile variables in Makefile.inc

include ../Makefile.inc

#######################################################

# Development headers
LIBHDR = icns.h

# Compiler flags
CFLAGS+=-Wall -I$(INCPATH) -I../$(LIBDIR)
ifeq "$(DEBUG)" "true"
CFLAGS+=-g -DICNS_DEBUG
endif
ifeq "$(JASPER)" "true"
CFLAGS+=-DICNS_JASPER
endif
ifeq "$(OPENJPEG)" "true"
CFLAGS+=-DICNS_OPENJPEG
endif

# Linker flags
LDFLAGS+=-L. -L$(LIBPATH) -L../$(LIBDIR)
ifeq "$(JASPER)" "true"
 LDFLAGS+=-lm -ljpeg -ljasper
endif
ifeq "$(OPENJPEG)" "true"
 LDFLAGS+=-lopenjpeg
endif

LDFLAGS_A+=$(LDFLAGS) $(LIBSTATIC)
ifeq "$(UNAME)" "Darwin"
 LDFLAGS_SO+=$(LDFLAGS) -licns
else
 LDFLAGS_SO+=$(LDFLAGS) ../$(LIBDIR)/$(LIBSO)
endif
LDFLAGS_PNG+=-lm -lz -lpng12

# icns2png object files
ICNS2PNGOBJS = icns2png.o

# icontainer2icns object files
ICONTOBJS = icontainer2icns.o

# Utility program object files
UTILOBJS = $(ICNS2PNGOBJS) $(ICONTOBJS)

# Utility program executables
UTILEXES = $(UTILOBJS:.o=)


all: $(UTILEXES)

icns2png: $(ICNS2PNGOBJS)
	$(CC) -o icns2png $(CFLAGS) $(ICNS2PNGOBJS) $(LDFLAGS_PNG) $(LDFLAGS_SO)

icontainer2icns: $(ICONTOBJS)
	$(CC) -o icontainer2icns $(ICONTOBJS)

test: $(UTILEXES)
	+@echo 'Test: Converting icon in native icns format...'; \
	./icns2png ../test/test1.icns; \
	echo 'Test: Converting icon in mac resource...'; \
	./icns2png ../test/test2.rsrc; \
	echo 'Test: Converting icon in macbinary resource...'; \
	./icns2png ../test/test3.bin; \

install: $(UTILEXES)
	-@if [ ! -d $(BINPATH) ]; then mkdir -p $(BINPATH); fi
	install -m 755 icns2png $(BINPATH)
	install -m 755 icontainer2icns $(BINPATH)
	if [ "$(UNAME)" = "Darwin" ]; then install_name_tool -change $(LIBSOMAJ) $(LIBPATH)/$(LIBSOMAJ) $(BINPATH)/icns2png; fi
	
clean:
	rm -f $(UTILOBJS) $(UTILEXES) 


