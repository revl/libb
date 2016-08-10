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

#include <B/Array.h>

static int counter = 0;

struct TestClass
{
	int value;

	TestClass(int initial_value) : value(initial_value)
	{
		++counter;
	}
	TestClass(const TestClass& Source) : value(Source.value)
	{
		++counter;
	}
	~TestClass()
	{
		--counter;
	}
};

#if !defined(__WATCOM_CPLUSPLUS__) || defined(B_NO_NAMESPACE)
typedef B::Array<TestClass> TestArray;
template class B::Array<TestClass>;
#else
namespace B
{
	typedef Array<TestClass> TestArrayX;
}
typedef B::TestArrayX TestArray;
#endif // __WATCOM_CPLUSPLUS__

B::Array<B::Array<TestClass> > test2d;

int main(int /*argc*/, char* /*argv*/[])
{
	{
		TestArray test(TestClass(666));
		TestArray test1(test);
		TestArray test2;
		test2 = test1;

		if (test2.GetSize() != 1)
			return 1;
	}

	return counter != 0 ? 3 : 0;
}
