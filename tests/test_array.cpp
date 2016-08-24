/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016 Damon Revoe
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

#define CHECK(condition, message) \
	if (!(condition)) { \
		fprintf(stderr, message); \
		return false; \
	}

static bool test_shrink_to_fit()
{
	static const size_t initial_size = 100;

	b::array<int> a1(1, initial_size);

	size_t initial_capacity = a1.capacity();

	CHECK(initial_capacity > initial_size,
		"Capacity must be greater than the initial size\n");

	b::array<int> a2(a1);

	a1.shrink_to_fit();

	CHECK(a1.data() != a2.data(),
		"shrink_to_fit() must cause reallocation");

	return true;
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

static bool test_copying()
{
	{
		test_array test(test_element(123));
		test_array test1(test);
		test_array test2;
		test2 = test1;

		if (test2.size() != 1) {
			fprintf(stderr, "Number of elements "
				"must not change in a copy.\n");
			return false;
		}
	}

	if (element_counter != 0)
	{
		fprintf(stderr, "All elements must be destructed.\n");
		return false;
	}

	return true;
}

b::array<b::array<test_element> > test2d;

int main(int /*argc*/, char* /*argv*/[])
{
	return !test_shrink_to_fit() +
		!test_copying();
}
