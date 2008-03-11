# Makefile for libicns
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
CFLAGS+=-Wall -I$(INCPATH)
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
LDFLAGS+=-L. -L$(LIBPATH)
ifeq "$(JASPER)" "true"
 LDFLAGS+=-lm -ljpeg -ljasper
endif
ifeq "$(OPENJPEG)" "true"
 LDFLAGS+=-lopenjpeg
endif

LDFLAGS_A+=$(LDFLAGS) $(LIBSTATIC)
ifeq "$(UNAME)" "Darwin"
 LDFLAGS_SO+=-licns $(LDFLAGS)
else
 LDFLAGS_SO+=-Wl,-rpath,. -licns $(LDFLAGS)
endif

# Library object files
LIBOBJS = icns_debug.o icns_element.o icns_family.o icns_image.o icns_io.o icns_jp2.o icns_rle24.o icns_utils.o 
LIBOBJSDLL = $(LIBOBJS:.o=.pic.o)

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

$(LIBSOMAJ): $(LIBSOMIN)
	ln -sf $(LIBSOMIN) $(LIBSOMAJ)

$(LIBSOMIN): $(LIBSOVER)
	ln -sf $(LIBSOVER) $(LIBSOMIN)

$(LIBSOVER): $(LIBOBJSDLL)
ifeq "$(UNAME)" "Darwin"
	$(CC) -dynamiclib -install_name $(LIBSOMAJ) -current_version $(VERSION) \
	 -compatibility_version $(COMPATVER) -o $(LIBSOVER) $(LIBOBJSDLL)
else
	$(CC) -shared -W1,-soname,$(LIBSOMAJ) -o $(LIBSOVER) $(LIBOBJSDLL)
endif


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

install: install-static install-shared
	
clean:
	rm -f $(LIBSTATIC) $(LIBSO) $(LIBSOMAJ) $(LIBSOMIN) \
		$(LIBSOVER) $(LIBOBJS) $(LIBOBJSDLL)


