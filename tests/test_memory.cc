/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <b/memory.h>

#include "unit_test.h"

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
