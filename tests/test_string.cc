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

#include <b/string.h>

#include "unit_test.h"

B_STATIC_CONST_STRING(left, "lLC");
B_STATIC_CONST_STRING(right, "CRr");
B_STATIC_CONST_STRING(abc, "abc");
B_STATIC_CONST_STRING(cba, "cba");

B_TEST_CASE(replace)
{
	b::string str(abc);

	str.replace(1, "B", 1);

	B_CHECK(str == "aBc");

	str.replace(1, "BB", 2);

	B_CHECK(str == "aBB");

	B_REQUIRE(str.capacity() > str.length());

	str.replace(2, "CC", 2);

	B_CHECK(str == "aBCC");

	size_t old_capacity = str.capacity();

	str.replace(3, 'D', old_capacity);

	B_REQUIRE(str.capacity() != old_capacity);

	B_CHECK(str == b::string("aBC", 3) + b::string(old_capacity, 'D'));
}

B_TEST_CASE(basic_checks)
{
	b::string str1("012", 3);
	b::string str2("345", 3);

	B_CHECK(str1 == "012");

	// Append str2 to str1.
	str1.append(str2);

	B_CHECK(str1 == "012345");

	str2.assign("567", 3);
	// Append the last 2 characters of str2 to str1.
	str1.append(str2.data() + 1, 2);

	B_CHECK(str1 == "01234567");

	// Append the first 2 characters of a character
	// array to str1.
	str1.append("89A", 2);

	B_CHECK(str1 == "0123456789");

	// Append a series of characters to str1.
	str1.append(2, 'D');
	str1.append('D');

	B_CHECK(str1 == "0123456789DDD");

	str1.empty();
	str2.assign("abcd", 3);

	B_CHECK(str1.is_empty());
	B_CHECK(str2.length() == 3);

	B_CHECK(str1.substr(0, 10).is_empty());

	b::string str3("cba", 3);
	b::string str4(3, 'x');

	B_CHECK(str4 > str3);

	str1 = left + str2 + str4 + str3 + right;
	str1.trim_left(B_STRING_VIEW("Ll"));
	str1.trim_right(B_STRING_VIEW("Rr"));
	str1.trim(B_STRING_VIEW("C"));
	B_CHECK(str1 == "abcxxxcba");

	B_CHECK(str1.substr(3, 3) == "xxx");
	B_CHECK(str1.substr(6, 6) == "cba");

	B_CHECK(str1.remainder(6) == "cba");

	str2 = abc + str4 + str3;
	B_CHECK(str1 == str2);

	size_t position = str3.rfind('b');
	B_CHECK(position == 1);

	B_CHECK(str3[position] == 'b');

	B_CHECK(str4.rfind('b') == (size_t) -1);
}

B_TEST_CASE(comparison)
{
	B_STATIC_CONST_STRING(abc1, "abc\000123");

	B_STATIC_CONST_STRING(abc2, "abc\000def");

	B_CHECK(abc1 != abc2);

	B_CHECK(abc1 != B_STRING_VIEW("abc"));
}

B_TEST_CASE(starts_with)
{
	b::string str("012", 3);

	B_CHECK(str.starts_with(B_STRING_VIEW("")));
	B_CHECK(str.starts_with(B_STRING_VIEW("01")));
	B_CHECK(str.starts_with(B_STRING_VIEW("012")));

	B_CHECK(!str.starts_with(B_STRING_VIEW("02")));
	B_CHECK(!str.starts_with(B_STRING_VIEW("0123")));

	b::string with_null("012\0", 4);

	B_CHECK(with_null.starts_with(with_null));
}

B_TEST_CASE(ends_with)
{
	b::string str("123", 3);

	B_CHECK(str.ends_with(B_STRING_VIEW("")));
	B_CHECK(str.ends_with(B_STRING_VIEW("23")));
	B_CHECK(str.ends_with(B_STRING_VIEW("123")));

	B_CHECK(!str.ends_with(B_STRING_VIEW("13")));
	B_CHECK(!str.ends_with(B_STRING_VIEW("0123")));

	b::string with_null("012\0", 4);

	B_CHECK(with_null.ends_with(with_null));
}

B_TEST_CASE(string_formatting)
{
	b::string str1;
	str1.format("abc%s", "xxx");
	str1 += cba;
	B_CHECK(str1 == "abcxxxcba");

	b::string str2;
	str2.append_formatted("init");
	B_CHECK(str2 == "init");

	B_CHECK(b::string::formatted("%s %d", "cloud", 9) == "cloud 9");
}

B_TEST_CASE(static_string)
{
	B_STATIC_CONST_STRING(hello, "hello");

	B_CHECK(hello.length() == 5);
	B_CHECK(hello.capacity() == 5);
	B_CHECK(hello == hello);
	B_CHECK(hello == "hello");
}

B_TEST_CASE(repeat)
{
	B_CHECK(abc.repeat(3) == "abcabcabc");

	B_CHECK(abc * 3 == "abcabcabc");
}

B_TEST_CASE(split)
{
	B_CHECK(!abc.split('\t', NULL, NULL));

	B_STATIC_CONST_STRING(tab_separated, "one\ttwo\tthree");

	B_REQUIRE(tab_separated.split('\t', NULL, NULL));

	b::string_view one, two_three;

	B_REQUIRE(tab_separated.split('\t', &one, &two_three));
	B_CHECK(one == "one");
	B_CHECK(two_three == "two\tthree");

	b::string_view single;

	B_REQUIRE(tab_separated.split('\t', &single, NULL));
	B_CHECK(single == "one");

	B_REQUIRE(tab_separated.split('\t', NULL, &single));
	B_CHECK(single == "two\tthree");
}

B_TEST_CASE(truncate)
{
	b::string str1;
	str1.format("abc%s", "xxx");

	str1.truncate(3);

	B_CHECK(str1 == abc);

	b::string str2 = str1;

	str1.truncate(1);

	B_CHECK(str1 == "a");
	B_CHECK(str2 == abc);
}

B_TEST_CASE(wide_chars)
{
	B_STATIC_CONST_WSTRING(unicode, "Unicode");

	B_CHECK(unicode == L"Unicode");

	B_CHECK(unicode.matches_pattern(L"*code"));
}

B_TEST_CASE(random_removal)
{
	static const int str_len = 100U;

	b::wstring str;

	for (int i = 0; i < str_len; ++i)
		str.append((wchar_t) i);

	b::pseudorandom prg(10);

	for (int i = 0; i < str_len; ++i)
	{
		b::pseudorandom::value_type random_index =
			prg.next((b::pseudorandom::value_type) str.length());

		str.remove(random_index);

		for (size_t j = 1; j < str.length(); ++j)
			B_CHECK(str[j - 1] < str[j]);
	}

	B_REQUIRE(str.is_empty());
}
