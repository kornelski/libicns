ACLOCAL_AMFLAGS = -I m4

SUBDIRS = libicns icnsutils

.PHONY: rpm

EXTRA_DIST = \
  samples/test1.icns \
  samples/test2.rsrc \
  samples/test3.bin \
  @PACKAGE@.spec.in \
  MANIFEST.sources

rpm: @PACKAGE@.spec
	fakeroot rpm --clean -bb @PACKAGE@.spec

distclean-local:
	-rm -f @PACKAGE@.spec
