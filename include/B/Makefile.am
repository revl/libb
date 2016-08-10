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

pkgincludedir = $(includedir)/B

pkginclude_HEADERS = \
	Array.h \
	ArrayImpl.h \
	CustomException.h \
	DoublyLinkedList.h \
	DoublyLinkedListContainer.h \
	Exception.h \
	Helpers.h \
	Host.h \
	LevenshteinDistance.h \
	LinkedList.h \
	LinkedListContainer.h \
	Memory.h \
	Meta.h \
	Object.h \
	Random.h \
	Ref.h \
	RefCount.h \
	RuntimeException.h \
	String.h \
	SystemException.h

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
