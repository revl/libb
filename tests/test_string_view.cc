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

#include <b/string_view.h>

#include "unit_test.h"

B_STATIC_CONST_STRING(static_const_string, "static_const_string");

B_STATIC_CONST_STRING(padded, "abcxxxcba");

B_TEST_CASE(string_view)
{
	b::string_view sv1;

	B_CHECK(sv1.is_empty());
	B_CHECK(sv1 == "");
	B_CHECK(sv1 == b::string_view());

	b::string_view sv2(static_const_string);

	B_CHECK(sv2.length() == static_const_string.length());
	B_CHECK(sv2 == static_const_string);

	sv1.assign("1234", 3);

	B_CHECK(sv1.length() == 3);
	B_CHECK(sv1 == "123");
	B_CHECK(sv1 < "1234");

	B_CHECK(b::string_view("123a", 3) == b::string_view("123b", 3));
}

B_TEST_CASE(find)
{
	b::string_view sv = padded;

	size_t b_index = sv.find('b');

	B_REQUIRE(b_index == 1);
	B_CHECK(sv[b_index] == 'b');

	B_CHECK(sv.find('z') == (size_t) -1);
}

B_TEST_CASE(rfind)
{
	b::string_view sv = padded;

	size_t b_index = sv.rfind('b');

	B_REQUIRE(b_index == 7);
	B_CHECK(sv[b_index] == 'b');

	B_CHECK(sv.rfind('z') == (size_t) -1);
}

B_TEST_CASE(repeat)
{
	b::string_view empty;

	B_CHECK(empty.repeat(10).is_empty());

	b::string_view x("x", 1);

	B_CHECK(x.repeat(3) == "xxx");

	b::string_view abc("abc", 3);

	B_CHECK(abc.repeat(0).is_empty());
	B_CHECK(abc.repeat(1) == "abc");
	B_CHECK(abc.repeat(2) == "abcabc");
	B_CHECK(abc.repeat(3) == "abcabcabc");
}

B_TEST_CASE(split)
{
	b::string_view abc("abc", 3);

	B_CHECK(!abc.split('\t', NULL, NULL));

	B_STATIC_CONST_STRING(tab_separated, "one\ttwo\tthree");

	B_REQUIRE(tab_separated.split('\t', NULL, NULL));

	b::string_view one, two_three;

	B_REQUIRE(tab_separated.split('\t', &one, &two_three));
	B_CHECK(one == "one");
	B_CHECK(two_three == "two\tthree");

	b::string_view two, three;

	B_REQUIRE(two_three.split('\t', &two, &three));
	B_CHECK(two == "two");
	B_CHECK(three == "three");

	b::string_view single;

	B_REQUIRE(tab_separated.split('\t', &single, NULL));
	B_CHECK(single == "one");

	B_REQUIRE(tab_separated.split('\t', NULL, &single));
	B_CHECK(single == "two\tthree");

	B_REQUIRE(single.split('\t', &single, &single));
	B_CHECK(single == "three");
}

B_TEST_CASE(trim)
{
	b::string_view sv = padded;

	sv.trim("abc");

	B_CHECK(sv == "xxx");

	B_CHECK(sv > padded);
}
