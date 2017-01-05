/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
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

#include <b/array.h>

#include "unit_test.h"

B_TEST_CASE(test_shared)
{
	static const size_t initial_size = 100;

	b::array<int> a1(1, initial_size);

	b::array<int> a2(a1);

	// Both arrays must share the same buffer.
	B_CHECK(a1.data() == a2.data());

	a2 += 2;

	// Now they must diverge.
	B_CHECK(a1.data() != a2.data());
}

B_TEST_CASE(test_realloc)
{
	b::array<int> a(1, 1);

	const int* original_buffer = a.data();

	a += 2;

	B_CHECK(a.data() == original_buffer);

	a.alloc_and_copy(3);

	B_CHECK(a.data() != original_buffer);
	B_CHECK(a.size() == 2);
	B_CHECK(a.capacity() == 3);
}

B_TEST_CASE(test_shrink_to_fit)
{
	static const size_t initial_size = 100;

	b::array<int> a1(initial_size, 1);

	size_t initial_capacity = a1.capacity();

	// Capacity must be greater than the initial size.
	B_CHECK(initial_capacity > initial_size);

	b::array<int> a2(a1);

	a1.trim_to_size();

	// trim_to_size() must cause reallocation.
	B_CHECK(a1.data() != a2.data());
}

static int element_counter = 0;

struct test_element
{
	int value;

	test_element(int initial_value) : value(initial_value)
	{
		++element_counter;
	}
	test_element(const test_element& source) : value(source.value)
	{
		++element_counter;
	}
	~test_element()
	{
		--element_counter;
	}
};

typedef b::array<test_element> test_array;
template class b::array<test_element>;

B_TEST_CASE(test_copying)
{
	{
		test_array test(1, test_element(123));
		test_array test1(test);
		test_array test2;
		test2 = test1;

		// Number of elements must not change in a copy.
		B_CHECK(test2.size() == 1);
	}

	// All elements must be destructed.
	B_CHECK(element_counter == 0);
}

b::array<b::array<test_element> > test2d;
