#         -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT(libicns, 0.5.9, mathew@eisbox.net)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/icns.h])
AM_MAINTAINER_MODE

# To enable debugging mode
CFLAGS="${CFLAGS=}"
AC_MSG_CHECKING(whether to enable debugging)
AC_ARG_ENABLE(debug, [  --enable-debug=[yes/no] turn on debugging [default=no]],, enable_debug=no)
if test "x$enable_debug" = "xyes"; then
         CFLAGS="$CFLAGS -g -DDEBUG -DICNS_DEBUG"
AC_MSG_RESULT(yes)
else
AC_MSG_RESULT(no)
fi

# Stuff for the config.h.in file
AC_CONFIG_HEADERS(config.h)
AC_GNU_SOURCE

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(stdint.h)
AC_CHECK_HEADERS(getopt.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_CONST
AC_C_BIGENDIAN
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_MODE_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_LIB(getopt,getopt_long)

# Check for libpng
AC_CHECK_LIB(png, png_create_read_struct, [
AC_SUBST(PNG_LIBS, "-lpng")
], [
  AC_CHECK_LIB(png10, png_create_read_struct, [
  AC_SUBST(PNG_LIBS, "-lpng10")
  ], [
    AC_CHECK_LIB(png12, png_create_read_struct, [
    AC_SUBST(PNG_LIBS, "-lpng12")
    ], [
      AC_MSG_ERROR([cannot find required library png])
    ], [])
  ], [])
], [])
AC_CHECK_HEADERS([png.h libpng/png.h libpng10/png.h libpng12/png.h])

# Check for libopenjpeg and libjasper
AC_CHECK_LIB(jasper, jas_init, [
AC_SUBST(JP2000_LIBS, "-ljasper")
AC_CHECK_HEADERS([jasper/jasper.h])
AC_DEFINE([ICNS_JASPER],[1],[We have Jasper])
], [
  AC_CHECK_LIB(openjpeg2, opj_setup_decoder, [
  AC_SUBST(JP2000_LIBS, "-lopenjpeg2")
  AC_CHECK_HEADERS([openjpeg.h])
  AC_DEFINE([ICNS_OPENJPEG],[1],[We have OpenJPEG])
  ], [
    AC_CHECK_LIB(openjpeg, opj_setup_decoder, [
    AC_SUBST(JP2000_LIBS, "-lopenjpeg")
    AC_CHECK_HEADERS([openjpeg.h])
    AC_DEFINE([ICNS_OPENJPEG],[1],[We have OpenJPEG])
    ], [
      AC_MSG_WARN([libopenjpeg or libjasper jp2000 codec libraries not found])
      AC_MSG_WARN([libicns will be built without 256x256 and 512x512 support])
    ])
  ])
])

AC_CONFIG_FILES([Makefile libicns.spec icnsutils/Makefile src/Makefile src/libicns.pc])
AC_OUTPUT

