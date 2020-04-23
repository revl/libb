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

#include <b/string_view.h>

#include "test_case.h"

B_STRING_LITERAL(static_const_string, "static_const_string");

B_STRING_LITERAL(padded, "abcxxxcba");

B_TEST_CASE(basic_checks)
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

B_TEST_CASE(starts_with)
{
	b::string_view sv("012", 3);

	B_CHECK(sv.starts_with(B_STRING_VIEW("")));
	B_CHECK(sv.starts_with(B_STRING_VIEW("01")));
	B_CHECK(sv.starts_with(B_STRING_VIEW("012")));

	B_CHECK(!sv.starts_with(B_STRING_VIEW("02")));
	B_CHECK(!sv.starts_with(B_STRING_VIEW("0123")));

	b::string_view with_null("012\0", 4);

	B_CHECK(with_null.starts_with(with_null));
}

B_TEST_CASE(ends_with)
{
	b::string_view sv("123", 3);

	B_CHECK(sv.ends_with(B_STRING_VIEW("")));
	B_CHECK(sv.ends_with(B_STRING_VIEW("23")));
	B_CHECK(sv.ends_with(B_STRING_VIEW("123")));

	B_CHECK(!sv.ends_with(B_STRING_VIEW("13")));
	B_CHECK(!sv.ends_with(B_STRING_VIEW("0123")));

	b::string_view with_null("012\0", 4);

	B_CHECK(with_null.ends_with(with_null));
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

	B_STRING_LITERAL(tab_separated, "one\ttwo\tthree");

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

	sv.trim(B_STRING_VIEW("abc"));

	B_CHECK(sv == "xxx");

	B_CHECK(sv > padded);

	sv = padded;

	sv.truncate(3);

	B_CHECK(sv == "abc");
}

B_TEST_CASE(substr)
{
	B_CHECK(b::string_view().substr(0, 10).is_empty());

	b::string_view sv = padded;

	B_CHECK(sv.substr(3, 3) == "xxx");
	B_CHECK(sv.substr(6, 6) == "cba");

	B_CHECK(sv.remainder(6) == "cba");
}

B_TEST_CASE(wide_chars)
{
	b::wstring_view unicode = B_WSTRING_VIEW("Unicode");

	B_CHECK(unicode == L"Unicode");

	B_CHECK(unicode.matches_pattern(L"*code"));
}
