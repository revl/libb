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

B_DEFINE_STATIC_STRING(hello, "hello");
B_DEFINE_STATIC_STRING(left, "lLC");
B_DEFINE_STATIC_STRING(right, "CRr");
B_DEFINE_STATIC_STRING(abc, "abc");
B_DEFINE_STATIC_STRING(cba, "cba");

int main()
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
	b::string str1;
	if (!str1.is_empty())
		return 2;

	b::string str2("abcd", 3);
	if (str2.length() != 3)
		return 1;

	b::string str3("cba", 3);
	b::string str4('x', 3);

	if (str4 < str3)
		return 3;

	str1 = B_STATIC_STRING(left) + str2 +
		str4 + str3 + B_STATIC_STRING(right);
	str1.trim_left("Ll");
	str1.trim_right("Rr");
	str1.trim("C");
	if (str1 != "abcxxxcba")
		return 4;

	str2 = B_STATIC_STRING(abc) + str4 + str3;
	if (!(str1 == str2))
		return 5;

	size_t position = str3.rfind('b');
	if (position != 1)
		return 6;

	if (str3[position] != 'b')
		return 7;

	if (str4.rfind('b') != (size_t) -1)
		return 8;

	str3.assignf("abc%s", "xxx");
	str3 += B_STATIC_STRING(cba);
	if (str1 != (const char*) str3)
		return 9;

	b::string str5;
	str5.assignf("init");
	if (str5 != "init")
		return 10;

	const b::string& hello = B_STATIC_STRING(hello);

	if (hello.length() != 5 || hello.capacity() != 5 ||
		hello != hello || hello != "hello")
		return 11;

	return 0;
}
