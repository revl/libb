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

#include <b/heap.h>

#include "unit_test.h"

B_TEST_CASE(heapsort)
{
	b::pseudorandom rand(123);

	size_t data[1000];

	for (size_t i = 0; i < B_COUNTOF(data); ++i)
		data[i] = rand.next();

	b::heapsort(data, B_COUNTOF(data));

	size_t prev_value = data[0];

	for (size_t i = 1; i < B_COUNTOF(data); ++i)
	{
		B_CHECK(prev_value <= data[i]);
		prev_value = data[i];
	}
}
