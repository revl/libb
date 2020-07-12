// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/memory.h>

#include "test_case.h"

B_TEST_CASE(size_alignment)
{
	B_CHECK(b::memory::align((size_t) 6, 4) == (size_t) 8);
	B_CHECK(b::memory::align((size_t) 7, 1) == (size_t) 7);
	B_CHECK(b::memory::align((size_t) 8, 4) == (size_t) 8);
	B_CHECK(b::memory::align((size_t) 9, 8) == (size_t) 16);
}

B_TEST_CASE(pointer_alignment)
{
	B_CHECK(b::memory::align((void*) 9, 8) == (void*) 16);
	B_CHECK(b::memory::align((void*) 0, 16) == (void*) 0);
	B_CHECK(b::memory::align((void*) 1, 32) == (void*) 32);
}
