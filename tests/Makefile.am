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

LDADD = -L../src -l$(PACKAGE)$(configsuffix)

TESTS = \
	test_array \
	test_binary_search_tree \
	test_heap \
	test_levenshtein_distance \
	test_lists \
	test_make_path \
	test_map \
	test_memory \
	test_misc \
	test_object \
	test_opaque \
	test_pathname \
	test_pseudorandom \
	test_string \
	test_string_formatting \
	test_string_view

check_PROGRAMS = \
	test_array \
	test_binary_search_tree \
	test_heap \
	test_levenshtein_distance \
	test_lists \
	test_make_path \
	test_map \
	test_memory \
	test_misc \
	test_object \
	test_opaque \
	test_pathname \
	test_pseudorandom \
	test_string \
	test_string_formatting \
	test_string_view

test_array_SOURCES = test_array.cc

test_binary_search_tree_SOURCES = test_binary_search_tree.cc

test_heap_SOURCES = test_heap.cc

test_levenshtein_distance_SOURCES = test_levenshtein_distance.cc

test_lists_SOURCES = test_lists.cc

test_make_path_SOURCES = test_make_path.cc

test_map_SOURCES = test_map.cc

test_memory_SOURCES = test_memory.cc

test_misc_SOURCES = test_misc.cc

test_object_SOURCES = test_object.cc

test_opaque_SOURCES = test_opaque.cc

test_pathname_SOURCES = test_pathname.cc

test_pseudorandom_SOURCES = test_pseudorandom.cc

test_string_formatting_SOURCES = test_string_formatting.cc

test_string_SOURCES = test_string.cc

test_string_view_SOURCES = test_string_view.cc

MAINTAINERCLEANFILES = Makefile.in
