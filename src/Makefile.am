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

if THREADS
if DEBUG
lib_LTLIBRARIES = libbd_r.la
else
lib_LTLIBRARIES = libb_r.la
endif
else
if DEBUG
lib_LTLIBRARIES = libbd.la
else
lib_LTLIBRARIES = libb.la
endif
endif

sources = \
	binary_search_tree.cc \
	cli.cc \
	exceptions.cc \
	io_streams.cc \
	memory.cc \
	misc.cc \
	object.cc \
	pathname.cc \
	string.cc \
	string_impl.h \
	string_stream.cc

libbd_r_la_SOURCES = $(sources)
libb_r_la_SOURCES = $(sources)
libbd_la_SOURCES = $(sources)
libb_la_SOURCES = $(sources)

ldflags = -version-info @library_version_info@ -release @VERSION@

libbd_r_la_LDFLAGS = $(ldflags)
libb_r_la_LDFLAGS = $(ldflags)
libbd_la_LDFLAGS = $(ldflags)
libb_la_LDFLAGS = $(ldflags)

MAINTAINERCLEANFILES = Makefile.in
