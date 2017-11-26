# B: Common C++ library
# Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
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

ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign

.PHONY: doc

doc: doc/timestamp

doc/timestamp: $(top_srcdir)/include/b/*.h \
	$(top_srcdir)/README \
	$(top_srcdir)/doc/mkdoc.pl
	$(MKDIR_P) doc/html
	'$(top_srcdir)/doc/mkdoc.pl' '$(top_srcdir)' doc/html
	touch doc/timestamp

plaindocdir = $(pkgdocdir)
plaindoc_DATA = README

install-data-local: doc/install-timestamp

doc/install-timestamp: doc/timestamp
	$(mkinstalldirs) $(DESTDIR)$(pkgdocdir)
	$(INSTALL_DATA) doc/html/* $(DESTDIR)$(pkgdocdir)/
	touch doc/install-timestamp

install-exec-local:
	@$(NORMAL_INSTALL)

uninstall-local:
	rm -rf $(DESTDIR)$(pkgdocdir) doc/install-timestamp
	@$(NORMAL_UNINSTALL)

clean-local:
	rm -rf doc/html doc/timestamp doc/install-timestamp

SUBDIRS = . include src tests

EXTRA_DIST = \
	doc/mkdoc.pl \
	doc/html_footer \
	m4/ac_cxx_have_stl.m4 \
	m4/ax_pthread.m4 \
	autogen.sh

pkgconfig_DATA = b.pc
