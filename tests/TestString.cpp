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

#ifdef B_USE_PRECOMPILED_HEADER
#include <B/Host.h>

#pragma hdrstop
#endif // B_USE_PRECOMPILED_HEADER

#include <B/String.h>

B_DEFINE_STATIC_STRING(hello, "hello");
B_DEFINE_STATIC_STRING(left, "lLC");
B_DEFINE_STATIC_STRING(right, "CRr");
B_DEFINE_STATIC_STRING(abc, "abc");
B_DEFINE_STATIC_STRING(cba, "cba");

int main()
{
/*	B::String str1(B_TEXT("012"));
	B::String str2(B_TEXT("345"));

	printf("str1 = %s\n", (const char*) str1);

	// append str2 to str1
	str1.Append(str2);

	printf("str1 = %s\n", (const char*) str1);

	// append the last 2 items in str2 to str1
	str2 = B_TEXT("567");
	str1.Append(str2, 1, 2);    // begin at pos 1, append 2 elements

	printf("str1 = %s\n", (const char*) str1);

	// append the first 2 items from an array of the element type
	char ach_test[] = {'8', '9', 'A'};
	str1.Append(ach_test, 2);

	printf("str1 = %s\n", (const char*) str1);

	// append all of a string literal to str1
	char test[] = B_TEXT("abc");
	str1.Append(test);

	printf("str1 = %s\n", (const char*) str1);

	// append one item of the element type
	str1.Append(1, 'D');

	printf("str1 = %s\n", (const char*) str1);

	// append str2 to str1 using iterators
	str2 = B_TEXT("EF");
	str1.Append(str2.begin(), str2.end());

	printf("str1 = %s\n", (const char*) str1);
*/
	B::String str1;
	if (!str1.IsEmpty())
		return 2;

	B::String str2(B_TEXT("abcd"), 3);
	if (str2.GetLength() != 3)
		return 1;

	B::String str3(B_TEXT("cba"), 3);
	B::String str4(B_TEXT('x'), 3);

	if (str4 < str3)
		return 3;

	str1 = B_STATIC_STRING(left) + str2 +
		str4 + str3 + B_STATIC_STRING(right);
	str1.TrimLeft(B_TEXT("Ll"));
	str1.TrimRight(B_TEXT("Rr"));
	str1.Trim(B_TEXT("C"));
	if (str1 != B_TEXT("abcxxxcba"))
		return 4;

	str2 = B_STATIC_STRING(abc) + str4 + str3;
	if (!(str1 == str2))
		return 5;

	int position = str3.ReverseFind(B_TEXT('b'));
	if (position != 1)
		return 6;

	if (str3[position] != B_TEXT('b'))
		return 7;

	if (str4.ReverseFind(B_TEXT('b')) != -1)
		return 8;

	str3.Format(B_TEXT("abc%s"), B_TEXT("xxx"));
	str3 += B_STATIC_STRING(cba);
	if (str1 != (const B_CHAR*) str3)
		return 9;

	B::String str5;
	str5.Format(B_TEXT("init"));
	if (str5 != B_TEXT("init"))
		return 10;

	const B::String& hello = B_STATIC_STRING(hello);

	if (hello.GetLength() != 5 || hello.GetCapacity() != 5 ||
		hello != hello || hello != B_TEXT("hello"))
		return 11;

	return 0;
}
