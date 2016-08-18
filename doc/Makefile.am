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

.PHONY: doc

if DOXYGEN
doc: timestamp

if DOXYCONFIG
timestamp: $(DOXYCONFIG) $(top_srcdir)/include/b/*.h \
	$(top_srcdir)/doc/*.doc $(srcdir)/Doxyfile
	rm -rf html
	'$(DOXYCONFIG)' '$(srcdir)/Doxyfile' $(PACKAGE) | \
		VERSION='$(VERSION)' \
		top_srcdir='$(top_srcdir)' \
		HAVE_DOT='$(HAVE_DOT)' \
		$(DOXYGEN) -
	touch timestamp
else
timestamp: $(top_srcdir)/include/b/*.h \
	$(top_srcdir)/doc/*.doc $(srcdir)/Doxyfile
	rm -rf html
	VERSION='$(VERSION)' \
	top_srcdir='$(top_srcdir)' \
	HAVE_DOT='$(HAVE_DOT)' \
	$(DOXYGEN) '$(srcdir)/Doxyfile'
	touch timestamp
endif

install-data-local: install-timestamp

install-timestamp: timestamp
	if test -d html; then \
		$(mkinstalldirs) $(DESTDIR)$(pkgdocdir)/html; \
		$(INSTALL_DATA) html/* $(DESTDIR)$(pkgdocdir)/html; \
	fi
	touch install-timestamp

uninstall-local:
	rm -rf $(DESTDIR)$(pkgdocdir)/html install-timestamp
	rmdir $(DESTDIR)$(pkgdocdir) || true

clean-local:
	rm -rf html timestamp install-timestamp
else
doc:
	@echo Doxygen was not found by the configure script >&2
	@exit 1
endif

EXTRA_DIST = \
	Doxyfile \
	html_footer \
	main.css \
	CustomException.doc \
	Exception.doc \
	Memory.doc \
	Memory-Exception.doc \
	Object.doc \
	RefCount.doc \
	RuntimeException.doc \
	SystemException.doc

MAINTAINERCLEANFILES = Makefile.in
