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

LDADD = -L../src -l$(PACKAGE)$(configsuffix)

TESTS = \
	test_string \
	test_string_formatting \
	test_string_view \
	test_array \
	test_levenshtein_distance \
	test_lists \
	test_memory \
	test_misc \
	test_object \
	test_pathname \
	test_random \
	test_make_path

check_PROGRAMS = \
	test_string \
	test_string_formatting \
	test_string_view \
	test_array \
	test_levenshtein_distance \
	test_lists \
	test_memory \
	test_misc \
	test_object \
	test_pathname \
	test_random \
	test_make_path

test_string_SOURCES = test_string.cc

test_string_formatting_SOURCES = test_string_formatting.cc

test_string_view_SOURCES = test_string_view.cc

test_array_SOURCES = test_array.cc

test_levenshtein_distance_SOURCES = test_levenshtein_distance.cc

test_lists_SOURCES = test_lists.cc

test_memory_SOURCES = test_memory.cc

test_misc_SOURCES = test_misc.cc

test_object_SOURCES = test_object.cc

test_pathname_SOURCES = test_pathname.cc

test_random_SOURCES = test_random.cc

test_make_path_SOURCES = test_make_path.cc

MAINTAINERCLEANFILES = Makefile.in
