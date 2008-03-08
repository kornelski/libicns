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

include Makefile.inc

#######################################################

# Subdirectories to make in
DIRS = $(LIBDIR) $(UTILSDIR)

all:
	-for d in $(DIRS); do (cd $$d; $(MAKE) all ); done

utils: $(LIBNAME)
	-cd $(UTILSDIR); $(MAKE) all

$(LIBNAME):
	-cd $(LIBDIR); $(MAKE) $(LIBNAME)

clean:
	-for d in $(DIRS); do (cd $$d; $(MAKE) clean ); done


