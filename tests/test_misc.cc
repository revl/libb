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

#include <b/misc.h>

#include <b/heap.h>

#include "unit_test.h"

static bool match_and_check_all(const char* input, const char* pattern,
	bool expected_match)
{
	b::string_view input_sv(input, b::calc_length(input));
	b::string_view pattern_sv(pattern, b::calc_length(pattern));

	return (expected_match ? 4 : 0) ==
		(int) b::match_pattern(input, pattern) +
		(int) b::match_pattern(input, pattern_sv) +
		(int) b::match_pattern(input_sv, pattern) +
		(int) b::match_pattern(input_sv, pattern_sv);
}

#define CHECK_MATCH(input, pattern, expected_match) \
	B_CHECK(match_and_check_all(input, pattern, expected_match))

B_TEST_CASE(pattern_matching)
{
	CHECK_MATCH("", "*?*", false);
	CHECK_MATCH("abc", "*b*", true);
	CHECK_MATCH("abcd", "ab*c", false);
	CHECK_MATCH("abcdbc", "abd*", false);
	CHECK_MATCH("abcdbcda", "a?*cda", true);
	CHECK_MATCH("abcdefgh", "ab?d*ef?h", true);
	CHECK_MATCH("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*b", false);
	CHECK_MATCH("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*", true);
	CHECK_MATCH("aaaaaa", "aaa*aaa", true);
	CHECK_MATCH("aaaaaa", "aaa**aaa", true);
}

B_TEST_CASE(version_comparison)
{
	B_CHECK(b::compare_versions("11.2", "2.11") > 0);
	B_CHECK(b::compare_versions("1.1", "1.1.1") < 0);
	B_CHECK(b::compare_versions("1.2.3", "1.2.3") == 0);
	B_CHECK(b::compare_versions("1.22.1", "1.6.1") > 0);
	B_CHECK(b::compare_versions("1.2.3.4", "1.2.3") > 0);
}

B_TEST_CASE(signed_char_alignment)
{
	B_CHECK(b::align((signed char) 6, 3) == (signed char) 6);
	B_CHECK(b::align((signed char) 7, 3) == (signed char) 9);
	B_CHECK(b::align((signed char) 8, 4) == (signed char) 8);
	B_CHECK(b::align((signed char) 9, 6) == (signed char) 12);
}

B_TEST_CASE(pointer_alignment)
{
	B_CHECK(b::align((void*) 9, 8) == (void*) 16);
	B_CHECK(b::align((void*) 0, 16) == (void*) 0);
	B_CHECK(b::align((void*) 1, 32) == (void*) 32);
}

B_TEST_CASE(shuffle)
{
	unsigned numbers[100];

	const unsigned count = sizeof(numbers) / sizeof(*numbers);

	for (unsigned i = 0; i < count; ++i)
		numbers[i] = i;

	bool ordered = true;

	b::shuffle(numbers, count);

	for (unsigned i = 0; i < count; ++i)
		if (numbers[i] != i)
			ordered = false;

	B_CHECK(!ordered);

	b::heap<unsigned>::sort(numbers, count);

	for (unsigned i = 0; i < count; ++i)
		B_CHECK(numbers[i] == i);
}
