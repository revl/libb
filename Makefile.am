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

doc:
	@cd doc && $(MAKE) $(AM_MAKEFLAGS) $@

SUBDIRS = . config m4 include src tests doc

plaindocdir = $(pkgdocdir)
plaindoc_DATA = README

install-exec-local:
	@$(NORMAL_INSTALL)
	@$(mkinstalldirs) $(DESTDIR)$(bindir)
	@$(INSTALL_SCRIPT) $(PACKAGE)-config $(DESTDIR)$(bindir)/
	@$(INSTALL_DATA) $(PACKAGE)$(configsuffix).conf $(DESTDIR)$(bindir)/

uninstall-local:
	@$(NORMAL_UNINSTALL)
	@rm -f $(DESTDIR)$(bindir)/$(PACKAGE)$(configsuffix).conf
	@set $(DESTDIR)$(bindir)/$(PACKAGE)*.conf; \
		test -f $$1 || rm -f $(DESTDIR)$(bindir)/$(PACKAGE)-config

maintainer-clean-local:
	rm -rf autom4te.cache

EXTRA_DIST = autogen.sh

DISTCLEANFILES = $(PACKAGE)$(configsuffix).conf

MAINTAINERCLEANFILES = Makefile.in
