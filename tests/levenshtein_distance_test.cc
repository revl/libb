// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/levenshtein_distance.h>

#include "test_case.h"

#define DIST(s1, s2) ld(s1, sizeof(s1) - 1, s2, sizeof(s2) - 1)

B_TEST_CASE(levenshtein_distance)
{
	b::levenshtein_distance ld;

	B_CHECK(DIST("QWERTY", "WER") == 3);

	B_CHECK(DIST("[pskdfbvjcv", "osdkfj") == 8);

	B_CHECK(DIST("jksefuyrthber", "uwufuyvghseyfa") == 10);

	B_CHECK(DIST("ASDF", "ASCDF") == 1);

	B_CHECK(DIST("1234", "") == 4);

	B_CHECK(DIST("", "1234") == 4);

	B_CHECK(DIST("string", "string") == 0);
}
