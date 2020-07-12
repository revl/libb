// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/pseudorandom.h>

#include "test_case.h"

B_TEST_CASE(seed_1000)
{
	b::pseudorandom prng(1000);

	static const size_t values[] =
	{
#if B_SIZEOF_SIZE_T == 8
		3688224355998257013, 6832060044855208398,
		3027798556776375235, 8829085963151268580,
		7719923505559373441, 6524863260213776554,
		2362032554020457263, 7385465247587078304,
		102442391910277453, 5322705319107267654
#else
		1856145921, 520568998, 304958183, 533034644, 2074920253,
		1456914738, 117026435, 1204643840, 405108793, 665566846
#endif
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
		++counters[prng.next(B_COUNTOF(counters))];

	for (size_t i = 0; i < B_COUNTOF(counters); ++i)
	{
		B_CHECK(counters[i] > 98000);
		B_CHECK(counters[i] < 102000);
	}
}
