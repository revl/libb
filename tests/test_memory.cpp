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

#include <b/memory.h>

struct SizeAlignmentTestCase
{
	size_t value;
	size_t alignment;
	size_t expected_result;

	bool Test() const;
};

inline bool SizeAlignmentTestCase::Test() const
{
	return B::Memory::Align(value, alignment) == expected_result;
}

static const SizeAlignmentTestCase size_alignment_test_cases[] =
{
	{6, 4, 8},
	{7, 1, 7},
	{8, 4, 8},
	{9, 8, 16}
};

struct PtrAlignmentTestCase
{
	void* value;
	size_t alignment;
	void* expected_result;

	bool Test() const;
};

inline bool PtrAlignmentTestCase::Test() const
{
	return B::Memory::Align(value, alignment) == expected_result;
}

static const PtrAlignmentTestCase ptr_alignment_test_cases[] =
{
	{(void*) 9, 8, (void*) 16},
	{(void*) 0, 16, (void*) 0},
	{(void*) 1, 32, (void*) 32}
};

int main()
{
	int index = sizeof(size_alignment_test_cases) /
		sizeof(*size_alignment_test_cases);

	const SizeAlignmentTestCase* size_alignment_test_case =
		size_alignment_test_cases;

	while (--index >= 0)
		if (!size_alignment_test_case++->Test())
			return 1;

	index = sizeof(ptr_alignment_test_cases) /
		sizeof(*ptr_alignment_test_cases);

	const PtrAlignmentTestCase* ptr_alignment_test_case =
		ptr_alignment_test_cases;

	while (--index >= 0)
		if (!ptr_alignment_test_case++->Test())
			return 2;

	return 0;
}
