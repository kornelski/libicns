ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src icnsutils

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

dist-hook:
	if test ! -e $(distdir)/ChangeLog -o \( $$(wc -l < $(distdir)/ChangeLog) -eq 1 -a -w $(distdir)/ChangeLog \) ; then \
		if test -d .git ; then \
			LC_ALL=C git log --pretty --numstat --summary $(VERSION) | git2cl > $(distdir)/ChangeLog ; \
		elif test -d .svn ; then \
			svn2cl -o $(distdir)/ChangeLog ; \
		fi \
	fi
