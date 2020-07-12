// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/heap.h>

#include "test_case.h"

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
