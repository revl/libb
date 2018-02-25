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

#include <b/array.h>

#include <b/heap.h>

#include "unit_test.h"

B_TEST_CASE(shared_data)
{
	static const size_t initial_length = 100;

	b::array<int> a1(1, initial_length);

	b::array<int> a2(a1);

	// Both arrays must share the same buffer.
	B_CHECK(a1.data() == a2.data());

	a2 += 2;

	// Now they must diverge.
	B_CHECK(a1.data() != a2.data());
}

B_TEST_CASE(array_realloc)
{
	b::array<int> a(1, 1);

	const int* original_buffer = a.data();

	a += 2;

	B_CHECK(a.data() == original_buffer);

	a.alloc_and_copy(3);

	B_CHECK(a.data() != original_buffer);
	B_CHECK(a.length() == 2);
	B_CHECK(a.capacity() == 3);
}

B_TEST_CASE(shrink_to_fit)
{
	static const size_t initial_length = 100;

	b::array<int> a1(initial_length, 1);

	size_t initial_capacity = a1.capacity();

	// Capacity must be greater than the initial length.
	B_CHECK(initial_capacity > initial_length);

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

B_TEST_CASE(array_copying)
{
	{
		test_array test(1, test_element(123));
		test_array test1(test);
		test_array test2;
		test2 = test1;

		// Number of elements must not change in a copy.
		B_CHECK(test2.length() == 1);
	}

	// All elements must be destructed.
	B_CHECK(element_counter == 0);
}

template <class T>
b::array<T> sequence_of_numbers(T array_size)
{
	b::array<T> numbers;

	numbers.discard_and_alloc((size_t) array_size);

	for (T i = 0; i < array_size; ++i)
		numbers.append(i);

	return numbers;
}

B_TEST_CASE(random_removal)
{
	static const int array_size = 100;

	b::array<int> numbers = sequence_of_numbers<int>(array_size);

	b::pseudorandom prg(10);

	for (int i = 0; i < array_size; ++i)
	{
		size_t random_index = prg.next(numbers.length());

		numbers.remove(random_index);

		for (size_t j = 1; j < numbers.length(); ++j)
			B_CHECK(numbers[j - 1] < numbers[j]);
	}

	B_REQUIRE(numbers.is_empty());
}

static void run_insertion_test(size_t tail_size)
{
	b::array<size_t> numbers;

	numbers.discard_and_alloc(100);

	size_t i;

	for (i = 0U; i < 10U; ++i)
		numbers.append(i);

	for (i = 20U; i < 20U + tail_size; ++i)
		numbers.append(i);

	static const size_t values[10] =
	{
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19
	};

	numbers.insert(10U, values, 10);

	for (i = 0U; i < numbers.length(); ++i)
		B_CHECK(numbers[i] == i);
}

B_TEST_CASE(insertion)
{
	run_insertion_test(20U);
	run_insertion_test(5U);
}

B_TEST_CASE(shuffle)
{
	b::array<unsigned> numbers = sequence_of_numbers<unsigned>(100);

	b::pseudorandom prng;

	numbers.shuffle(prng);

	bool ordered = true;

	for (unsigned i = 0; i < numbers.length(); ++i)
		if (numbers[i] != i)
			ordered = false;

	B_CHECK(!ordered);

	b::heapsort(numbers.lock(), numbers.length());

	for (unsigned i = 0; i < numbers.length(); ++i)
		B_CHECK(numbers[i] == i);

	numbers.unlock();
}

b::array<b::array<test_element> > test2d;

B_TEST_CASE(slice)
{
	b::array<unsigned> numbers = sequence_of_numbers<unsigned>(100);

	b::array_slice<unsigned> slice = numbers.slice(10, 20);

	B_REQUIRE(slice.length() == 20);

	unsigned n = 10;
	size_t i = 0;

	do
		B_CHECK(slice[i++] == n++);
	while (i <= 20);
}
