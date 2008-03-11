# Makefile.inc for libicns and icns2png
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
# Update this section

# Version information
# Note, versions are in the format of the following
# 0.0.0a
# See DEVNOTES for more information on versioning
LIBNAME = libicns
LIBVERMAJ = 0
LIBVERMIN = 5
LIBVERREL = 2

# Compile with debug flags and icns debug messages
DEBUG = false

# Compile with support for icons >= 256x256
# Use either Jasper or OpenJPEG, but not both
# As of 1.900.1, the Jasper library is still buggy when
# decompressing RGBA icons, so OpenJPEG is preferable
#JASPER = true
OPENJPEG = true

# Where make install puts libicns and icns2png
PREFIX ?= /usr
LIBPATH=$(PREFIX)/lib
BINPATH=$(PREFIX)/bin
INCPATH=$(PREFIX)/include

# Utilities
CC=gcc
AR_RC=ar rc
RANLIB=ranlib
LT=libtool
MAKE=make

#######################################################
# This section should not need updating

# Library directory
LIBDIR=libicns

# Utility program directory
UTILSDIR=icnsutils

# Operating system name
UNAME := $(shell uname)

# Version information
VERSION = $(LIBVERMAJ).$(LIBVERMIN).$(LIBVERREL)
COMPATVER = $(LIBVERMAJ).$(LIBVERMIN).0
LIBSTATIC = $(LIBNAME).a
ifeq "$(UNAME)" "Darwin"
 LIBSO = $(LIBNAME).dylib
 LIBSOMAJ = $(LIBNAME).$(LIBVERMAJ).dylib
 LIBSOMIN = $(LIBNAME).$(LIBVERMAJ).$(LIBVERMIN).dylib
 LIBSOVER = $(LIBNAME).$(VERSION).dylib
else
 LIBSO = $(LIBNAME).so
 LIBSOMAJ = $(LIBSO).$(LIBVERMAJ)
 LIBSOMIN = $(LIBSO).$(LIBVERMAJ).$(LIBVERMIN)
 LIBSOVER = $(LIBSO).$(VERSION)
endif

