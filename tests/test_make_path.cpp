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

#include <b/helpers.h>

B_DEFINE_STATIC_STRING(test_dir, "TestDir");
B_DEFINE_STATIC_STRING(dot_dir, ".");
B_DEFINE_STATIC_STRING_T(new_dir, B_TEXT("TestDir") B_PATH_SEPARATOR_SZ \
	B_TEXT("Intermed") B_PATH_SEPARATOR_SZ B_TEXT("NewDir"));
B_DEFINE_STATIC_STRING_T(intermediate,
	B_TEXT("TestDir") B_PATH_SEPARATOR_SZ B_TEXT("Intermed"));

int main()
{
	int test_number = 1;

	try
	{
		B::MakeDirectory(B_STATIC_STRING(test_dir));

		++test_number;

		// Make sure the directory is created.
		if (!B::IsDirectory(B_STATIC_STRING(test_dir)))
			return test_number;

		++test_number;

		// Creation of existing directories must succeed.
		B::MakeDirectory(B_STATIC_STRING(test_dir));

		++test_number;

		B::MakeDirectory(B_STATIC_STRING(dot_dir));

		++test_number;

		// Empty parameter is allowed.
		B::MakeDirectory(B::String());

		++test_number;

		// MakeDirectory() should not create intermediate
		// directories.
		if (!B::IsDirectory(B_STATIC_STRING(new_dir)))
			try
			{
				B::MakeDirectory(B_STATIC_STRING(new_dir));
				return test_number;
			}
			catch (B::SystemException&)
			{
			}

		++test_number;

		// And MakePath() should.
		B::MakePath(B_STATIC_STRING(new_dir));

		B::RemoveDirectory(B_STATIC_STRING(new_dir));

		++test_number;

		B::RemoveDirectory(B_STATIC_STRING(intermediate));

		++test_number;

		B::RemoveDirectory(B_STATIC_STRING(test_dir));
	}
	catch (B::SystemException&)
	{
		return test_number;
	}

	return 0;
}
