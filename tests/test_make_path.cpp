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

#include <b/misc.h>

B_DEFINE_STATIC_STRING(test_dir, "TestDir");
B_DEFINE_STATIC_STRING(dot_dir, ".");
B_DEFINE_STATIC_STRING(new_dir, "TestDir" B_PATH_SEPARATOR_SZ \
	"Intermed" B_PATH_SEPARATOR_SZ "NewDir");
B_DEFINE_STATIC_STRING(intermediate,
	"TestDir" B_PATH_SEPARATOR_SZ "Intermed");

int main()
{
	int test_number = 1;

	try
	{
		b::make_directory(B_STATIC_STRING(test_dir));

		++test_number;

		// Make sure the directory is created.
		if (!b::is_directory(B_STATIC_STRING(test_dir)))
			return test_number;

		++test_number;

		// Creation of existing directories must succeed.
		b::make_directory(B_STATIC_STRING(test_dir));

		++test_number;

		b::make_directory(B_STATIC_STRING(dot_dir));

		++test_number;

		// Empty parameter is allowed.
		b::make_directory(b::string());

		++test_number;

		// make_directory() should not create intermediate
		// directories.
		if (!b::is_directory(B_STATIC_STRING(new_dir)))
			try
			{
				b::make_directory(B_STATIC_STRING(new_dir));
				return test_number;
			}
			catch (b::system_exception&)
			{
			}

		++test_number;

		// And make_path() should.
		b::make_path(B_STATIC_STRING(new_dir));

		b::remove_directory(B_STATIC_STRING(new_dir));

		++test_number;

		b::remove_directory(B_STATIC_STRING(intermediate));

		++test_number;

		b::remove_directory(B_STATIC_STRING(test_dir));
	}
	catch (b::system_exception&)
	{
		return test_number;
	}

	return 0;
}
