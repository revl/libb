/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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
