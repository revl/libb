/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016 Damon Revoe
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

B_TEST_CASE(disabled_checks)
{
/*	b::string str1("012");
	b::string str2("345");

	printf("str1 = %s\n", (const char*) str1);

	// append str2 to str1
	str1.append(str2);

	printf("str1 = %s\n", (const char*) str1);

	// append the last 2 items in str2 to str1
	str2 = "567";
	str1.append(str2, 1, 2);    // begin at pos 1, append 2 elements

	printf("str1 = %s\n", (const char*) str1);

	// append the first 2 items from an array of the element type
	char ach_test[] = {'8', '9', 'A'};
	str1.append(ach_test, 2);

	printf("str1 = %s\n", (const char*) str1);

	// append all of a string literal to str1
	char test[] = "abc";
	str1.append(test);

	printf("str1 = %s\n", (const char*) str1);

	// append one item of the element type
	str1.append(1, 'D');

	printf("str1 = %s\n", (const char*) str1);

	// append str2 to str1 using iterators
	str2 = "EF";
	str1.append(str2.begin(), str2.end());

	printf("str1 = %s\n", (const char*) str1);
*/
}

// TODO FIXME Split into many small tests
B_TEST_CASE(test_string)
{
	b::string str1;

	B_CHECK(str1.is_empty());

	b::string str2("abcd", 3);
	B_CHECK(str2.length() == 3);

	b::string str3("cba", 3);
	b::string str4('x', 3);

	B_CHECK(str4 > str3);

	str1 = left + str2 + str4 + str3 + right;
	str1.trim_left("Ll");
	str1.trim_right("Rr");
	str1.trim("C");
	B_CHECK(str1 == "abcxxxcba");

	str2 = abc + str4 + str3;
	B_CHECK(str1 == str2);

	size_t position = str3.rfind('b');
	B_CHECK(position == 1);

	B_CHECK(str3[position] == 'b');

	B_CHECK(str4.rfind('b') == (size_t) -1);
}

B_TEST_CASE(test_string_formatting)
{
	b::string str1;
	str1.format("abc%s", "xxx");
	str1 += cba;
	B_CHECK(str1 == "abcxxxcba");

	b::string str2;
	str2.append_format("init");
	B_CHECK(str2 == "init");
}

B_TEST_CASE(test_static_string)
{
	B_STATIC_CONST_STRING(hello, "hello");

	B_CHECK(hello.length() == 5);
	B_CHECK(hello.capacity() == 5);
	B_CHECK(hello == hello);
	B_CHECK(hello == "hello");
}
