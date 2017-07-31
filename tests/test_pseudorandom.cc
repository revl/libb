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

#include <b/pseudorandom.h>

#include "unit_test.h"

B_TEST_CASE(seed_1000)
{
	b::pseudorandom prng(1000);

	static const b::pseudorandom::value_type values[] =
	{
		1856145921, 520568998, 304958183, 533034644, 2074920253,
		1456914738, 117026435, 1204643840, 405108793, 665566846
	};

	for (size_t i = 0; i < B_COUNTOF(values); ++i)
		B_CHECK(values[i] == prng.next());
}

B_TEST_CASE(limit)
{
	b::pseudorandom prng(0);

	for (size_t i = 0; i < 100000; ++i)
		B_CHECK(prng.next(100) < 100);
}

B_TEST_CASE(uniform_distribution)
{
	size_t counters[10] = {0};

	b::pseudorandom prng;

	for (size_t i = 0; i < B_COUNTOF(counters) * 100000; ++i)
		++counters[prng.next(
			(b::pseudorandom::value_type) B_COUNTOF(counters))];

	for (size_t i = 0; i < B_COUNTOF(counters); ++i)
	{
		B_CHECK(counters[i] > 98000);
		B_CHECK(counters[i] < 102000);
	}
}
