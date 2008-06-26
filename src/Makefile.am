
lib_LTLIBRARIES = libicns.la

libicns_la_LDFLAGS = -version-info 2:4:1

libicns_la_LIBADD = @JP2000_LIBS@

libicns_la_SOURCES = \
  icns_debug.c \
  icns_element.c \
  icns_family.c \
  icns_image.c \
  icns_io.c \
  icns_jp2.c \
  icns_rle24.c \
  icns_utils.c \
  icns_colormaps.h \
  icns_internals.h \
  icns.h 

AM_CFLAGS = -Wall

libicns_includedir=$(includedir)
libicns_include_HEADERS = icns.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libicns.pc
