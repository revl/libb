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

pkgincludedir = $(includedir)/b

pkginclude_HEADERS = \
	array.h \
	array_impl.h \
	custom_exception.h \
	doubly_linked_list.h \
	doubly_linked_list_container.h \
	exception.h \
	host.h \
	levenshtein_distance.h \
	linked_list.h \
	linked_list_container.h \
	memory.h \
	misc.h \
	node_access_via_cast.h \
	object.h \
	random.h \
	ref.h \
	ref_count.h \
	runtime_exception.h \
	string.h \
	system_exception.h

all: config.h

check: all

config.h: $(CONFIG_HEADER)
	sed -e 's/#\([^ ][^ ]*\) \([^ ][^ ]*\)/#\1 B_\2/g' < \
		$(CONFIG_HEADER) > $@

install-data-hook: config.h
	$(INSTALL_DATA) config.h \
		$(DESTDIR)$(pkgincludedir)/config$(configsuffix).h

uninstall-hook:
	rm -f $(DESTDIR)$(pkgincludedir)/config$(configsuffix).h
	rmdir $(DESTDIR)$(pkgincludedir) || true

# For older versions of Automake.
uninstall-local:
	rm -f $(DESTDIR)$(pkgincludedir)/config$(configsuffix).h
	rmdir $(DESTDIR)$(pkgincludedir) || true

CLEANFILES = config.h

MAINTAINERCLEANFILES = Makefile.in
