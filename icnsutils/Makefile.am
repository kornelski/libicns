bin_PROGRAMS = icns2png icontainer2icns png2icns icnsutil

icns2png_SOURCES = \
  icns2png.c

icontainer2icns_SOURCES = \
  icontainer2icns.c

png2icns_SOURCES = \
  png2icns.c

icnsutil_SOURCES = \
  icnsutil.c

icns2png_LDADD = \
  @PNG_LIBS@ \
  ../src/libicns.la

png2icns_LDADD = \
  @PNG_LIBS@ \
  ../src/libicns.la

icnsutil_LDADD = \
  @PNG_LIBS@ \
  ../src/libicns.la

man_MANS = \
  icns2png.1 \
  icontainer2icns.1 \
  png2icns.1 \ 
  icnsutil.1

EXTRA_DIST = \
  $(man_MANS)

AM_CPPFLAGS = \
  -I$(top_srcdir)/src/

AM_CFLAGS = -Wall

MAINTAINERCLEANFILES = \
  Makefile.in
