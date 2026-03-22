ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I $(srcdir)

SUBDIRS = lib src tests db

CONFIG_CLEAN_FILES = Makefile.in aclocal.m4 config.h.in configure

clean-local: lcov-clean splint-clean doxygen-clean man-clean
	$(RM) -rf autom4te.cache/

distclean-local:
	$(RM) -rf build-aux/ m4/


################################################################################
# code coverage testing
.PHONY: lcov lcov-perf lcov-gen lcov-clean
# use recursive makes in order to ignore errors during check/perf
lcov:
	$(MAKE) $(AM_MAKEFLAGS) check
	$(MAKE) $(AM_MAKEFLAGS) lcov-gen
lcov-perf:
	$(MAKE) $(AM_MAKEFLAGS) perf
	$(MAKE) $(AM_MAKEFLAGS) lcov-gen

# we have to massage the lcov.info file slightly to hide the effect of libtool
# placing the objects files in the .libs/ directory separate from the *.c
lcov-gen:
	$(LCOV) -c -d  $(top_builddir) -o lcov.info
	$(SED) -i -e 's#.libs/##' lcov.info
	LANG=C $(GENHTML) -s -f --legend -o lcov -t "librds" -p $(top_builddir) lcov.info
	@echo "Output: lcov/"

lcov-clean:
	$(LCOV) -z -d $(top_builddir)
	$(RM) -rf lcov.info lcov/
	$(FIND) tests/ lib/ src/ -name '*.gcda' -print | xargs $(RM) -f
	$(FIND) tests/ lib/ src/ -name '*.gcno' -print | xargs $(RM) -f
	$(FIND) tests/ -name '*.lcov.info' -print | xargs $(RM) -f
	$(FIND) tests/ -name '*.log' -print | xargs $(RM) -f

lcov-detailed:
	$(RM) -rf lcov.info lcov/
	INFOS=""; \
	for TEST in tests/*_test; do \
		TEST=`basename $$TEST`; \
		INFOS="$$INFOS -a tests/$$TEST.lcov.info"; \
		$(LCOV) -z -d $(top_builddir); \
		pushd tests; ./$$TEST; popd; \
		$(LCOV) -c -d $(top_builddir) -t $$TEST -o tests/$$TEST.lcov.info; \
	done; \
	$(LCOV) $$INFOS -o lcov.info
	LANG=C $(GENHTML) -s -f --legend -o lcov -t "librds" -p $(top_builddir) lcov.info
	@echo "Output: lcov/"


################################################################################
# static code analysis
.PHONY: splint splint-clean
splint:
	splint +posixlib \
		-I$(srcdir)/lib $(librds_la_CFLAGS) $(librds_la_SOURCES) -DHAVE_CONFIG_H \
		-DDATADIR=\"$(datadir)\" -DSHAREDSTATEDIR=\"$(sharedstatedir)\" \
		$(srcdir)/lib/*.c > splint.txt
	@echo "Output: splint.txt"

splint-clean:
	$(RM) -f splint.txt


################################################################################
# static code analysis
.PHONY: cppcheck cppcheck-clean
cppcheck:
	cppcheck --enable=all $(srcdir)/lib 2> cppcheck.txt
	@echo "Output: cppcheck.txt"

cppcheck-clean:
	$(RM) -f cppcheck.txt


################################################################################
# code documentation
.PHONY: doxygen doxygen-clean
doxygen:
	$(DOXYGEN) Doxyfile
	@echo "Output: html/"

doxygen-clean:
	$(RM) -rf html/


################################################################################
# manual pages
.PHONY: man man-clean
man:
	$(DOXYGEN) Doxyfile-man3
	@echo "Output: man/"

man-clean:
	$(RM) -rf man/


################################################################################
# source code formatter
.PHONY: astyle
astyle:
	find $(srcdir)/lib -name "*.[ch]" -exec $(ASTYLE) --suffix=none --style=ansi --convert-tabs "{}" \;
