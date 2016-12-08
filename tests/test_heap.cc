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

#include <b/heap.h>

#include "unit_test.h"

B_TEST_CASE(push_pop)
{
	b::heap<int> heap;

	heap.push(20);
	heap.push(10);

	B_CHECK(heap.pop() == 20);

	heap.push(30);

	B_CHECK(heap.pop() == 30);

	heap.push(60);
	heap.push(50);

	B_CHECK(heap.size() == 3);

	heap.push(40);
	heap.push(70);

	B_CHECK(heap.pop() == 70);
	B_CHECK(heap.pop() == 60);
	B_CHECK(heap.pop() == 50);
	B_CHECK(heap.pop() == 40);
	B_CHECK(heap.pop() == 10);

	B_CHECK(heap.is_empty());
}

#define COUNT (sizeof(data) / sizeof(*data))

B_TEST_CASE(heapsort)
{
	b::pseudorandom rand(123);

	int data[1000];

	for (size_t i = 0; i < COUNT; ++i)
		data[i] = rand.next();

	b::heap<int>::sort(data, COUNT);

	int prev_value = data[0];

	for (size_t i = 1; i < COUNT; ++i)
	{
		B_CHECK(prev_value <= data[i]);
		prev_value = data[i];
	}
}
