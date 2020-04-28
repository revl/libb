# B: Common C++ library
# Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

include(CheckCXXSourceCompiles)
include(CheckLibraryExists)

# Check for std::atomic<> in <atomic>
set(SAVE_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -std=c++11")
check_cxx_source_compiles("
#include <atomic>

int main()
{
	std::atomic<int> val = ATOMIC_VAR_INIT(1);
	++val;
	if (val != 2)
		return 2;
	val = 1;

	struct
	{
		std::atomic<int> val;
	} wrapper = {ATOMIC_VAR_INIT(1)};

	return --val || --wrapper.val;
}
" B_HAVE_STD_ATOMIC)
set(CMAKE_REQUIRED_FLAGS ${SAVE_CMAKE_REQUIRED_FLAGS})

# Check for __sync built-ins for atomic access
check_cxx_source_compiles("
int main()
{
	int val = 0;
	__sync_add_and_fetch(&val, 1);
	__sync_add_and_fetch(&val, -1);
	return val;
}
" B_HAVE_ATOMIC_SYNC)
