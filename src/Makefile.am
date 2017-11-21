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

lib_LTLIBRARIES = libb.la

libb_la_SOURCES = \
	binary_search_tree.cc \
	cli.cc \
	exceptions.cc \
	fn.cc \
	io_streams.cc \
	memory.cc \
	object.cc \
	pathname.cc \
	red_black_tree.cc \
	string.cc \
	string_stream.cc

libb_la_LDFLAGS = -version-info @library_version_info@ -release @VERSION@

EXTRA_DIST = \
	string_formatting.h \
	string_impl.h \
	string_view_impl.h

MAINTAINERCLEANFILES = Makefile.in
