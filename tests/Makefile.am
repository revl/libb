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
	TestHelpers \
	TestString \
	TestArray \
	TestLevenshteinDistance \
	TestLists \
	TestMemory \
	TestObject \
	TestRandom \
	TestMakePath

check_PROGRAMS = \
	TestHelpers \
	TestString \
	TestArray \
	TestLevenshteinDistance \
	TestLists \
	TestMemory \
	TestObject \
	TestRandom \
	TestMakePath

TestHelpers_SOURCES = TestHelpers.cpp

TestString_SOURCES = TestString.cpp

TestArray_SOURCES = TestArray.cpp

TestLevenshteinDistance_SOURCES = TestLevenshteinDistance.cpp

TestLists_SOURCES = TestLists.cpp

TestMemory_SOURCES = TestMemory.cpp

TestObject_SOURCES = TestObject.cpp

TestRandom_SOURCES = TestRandom.cpp

TestMakePath_SOURCES = TestMakePath.cpp

MAINTAINERCLEANFILES = Makefile.in
