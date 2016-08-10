# B: Common C++ library
# Copyright (C) 2002-2007, 2016 Damon Revoe
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

if DOXYGEN
.PHONY: doc doc_en

doc: timestamp_en
	@if test -f only_en; then \
		rm -f only_en timestamp; \
	fi

doc_en: timestamp_en
	test -f only_en || rm -f timestamp && touch only_en

if DOXYCONFIG
timestamp_en: $(DOXYCONFIG) $(top_srcdir)/include/B/*.h \
	$(top_srcdir)/doc/*.doc.en $(srcdir)/Doxyfile.en
	rm -rf en
	'$(DOXYCONFIG)' '$(srcdir)/Doxyfile.en' $(PACKAGE) en | \
		VERSION='$(VERSION)' \
		top_srcdir='$(top_srcdir)' \
		HAVE_DOT='$(HAVE_DOT)' \
		$(DOXYGEN) -
	touch timestamp_en
else
timestamp_en: $(top_srcdir)/include/B/*.h \
	$(top_srcdir)/doc/*.doc.en $(srcdir)/Doxyfile.en
	rm -rf en
	VERSION='$(VERSION)' \
	top_srcdir='$(top_srcdir)' \
	HAVE_DOT='$(HAVE_DOT)' \
	$(DOXYGEN) '$(srcdir)/Doxyfile.en'
	touch timestamp_en
endif

install-data-local: install-timestamp

timestamp:
	if test -f only_en; then \
		$(MAKE) $(AM_MAKEFLAGS) timestamp_en; \
	else \
		$(MAKE) $(AM_MAKEFLAGS) timestamp_en; \
	fi
	touch timestamp

install-timestamp: timestamp
	if test -f only_en; then \
		if test -d en/html; then \
			$(mkinstalldirs) $(DESTDIR)$(pkgdocdir)/html; \
			$(INSTALL_DATA) en/html/* $(DESTDIR)$(pkgdocdir)/html; \
		fi; \
	else \
		for l in en; do \
			if test -d $$l/html; then \
				$(mkinstalldirs) \
					$(DESTDIR)$(pkgdocdir)/html/$$l; \
				$(INSTALL_DATA) $$l/html/* \
					$(DESTDIR)$(pkgdocdir)/html/$$l; \
			fi; \
		done; \
	fi
	touch install-timestamp

uninstall-local:
	rm -rf $(DESTDIR)$(pkgdocdir)/html install-timestamp
	rmdir $(DESTDIR)$(pkgdocdir) || true

clean-local:
	rm -rf en timestamp{,_en} install-timestamp
endif

EXTRA_DIST = \
	Doxyfile.en \
	html_footer.en \
	main.css \
	CustomException.doc.en \
	Exception.doc.en \
	Memory.doc.en \
	Memory-Exception.doc.en \
	Object.doc.en \
	RefCount.doc.en \
	RuntimeException.doc.en \
	SystemException.doc.en

MAINTAINERCLEANFILES = Makefile.in
