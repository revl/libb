# This file is part of the B library, which is released under the MIT license.
# Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
# See the file LICENSE for the license terms.

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

# Check for the __gnu_cxx:: atomic ops in ext/atomicity.h
check_cxx_source_compiles("
#include <ext/atomicity.h>

int main()
{
	_Atomic_word val = 0;
	__gnu_cxx::__atomic_add(&val, 1);
	__gnu_cxx::__exchange_and_add(&val, -1);
	return (int) val;
}
" B_HAVE_EXT_ATOMICITY_H)

# Check for for the __gnu_cxx:: atomic ops in bits/atomicity.h
check_cxx_source_compiles("
#include <bits/atomicity.h>

int main()
{
	_Atomic_word val = 0;
	__gnu_cxx::__atomic_add(&val, 1);
	__gnu_cxx::__exchange_and_add(&val, -1);
	return (int) val;
}
" B_HAVE_BITS_ATOMICITY_H)

# Check if asm/atomic.h defines atomic functions
check_cxx_source_compiles("
#include <asm/atomic.h>

int main()
{
	atomic_t val = ATOMIC_INIT(0);
	atomic_set(&val, atomic_read(&val));
	atomic_inc(&val);
	atomic_dec_and_test(&val);
	return (int) val;
}
" B_HAVE_ASM_ATOMIC_H)
