bin_PROGRAMS = icns2png icontainer2icns

icns2png_SOURCES = \
  icns2png.c

icontainer2icns_SOURCES = \
  icontainer2icns.c

icns2png_LDADD = \
  @PNG_LIBS@ \
  ../libicns/libicns.la

man_MANS = \
  icns2png.1

EXTRA_DIST = \
  $(man_MANS)

AM_CPPFLAGS = \
  -I$(top_srcdir)/libicns/

AM_CFLAGS = -Wall
