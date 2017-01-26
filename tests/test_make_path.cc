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

#include <b/misc.h>

#include "unit_test.h"

B_STATIC_CONST_STRING(dot_dir, ".");

B_STATIC_CONST_STRING(test_dir, "b_test_dir");

B_STATIC_CONST_STRING(intermediate, "b_test_dir" B_PATH_SEPARATOR_SZ
	"missing_parent");

B_STATIC_CONST_STRING(new_dir, "b_test_dir" B_PATH_SEPARATOR_SZ
	"missing_parent" B_PATH_SEPARATOR_SZ "new_dir");

B_TEST_CASE(create_directory)
{
	// Start with a clean slate.
	try
	{
		b::remove_directory(new_dir);
	}
	catch (b::system_exception&)
	{
	}

	try
	{
		b::remove_directory(intermediate);
	}
	catch (b::system_exception&)
	{
	}

	try
	{
		b::remove_directory(test_dir);
	}
	catch (b::system_exception&)
	{
	}

	try
	{
		b::create_directory(test_dir);
	}
	catch (b::system_exception&)
	{
		B_CHECK("create new" && false);
	}

	// Make sure the directory exists now.
	B_REQUIRE(b::is_directory(test_dir));

	try
	{
		// Creation of existing directories must succeed.
		b::create_directory(test_dir);
	}
	catch (b::system_exception&)
	{
		B_CHECK("create existing" && false);
	}

	try
	{
		// It is allowed to request creation of
		// the '.' directory.
		b::create_directory(dot_dir);
	}
	catch (b::system_exception&)
	{
		B_CHECK("create dot" && false);
	}

	try
	{
		// Empty parameter is allowed.
		b::create_directory(b::string());
	}
	catch (b::system_exception&)
	{
		B_CHECK("create empty" && false);
	}

	// create_directory() with no additional arguments
	// should not create missing parent directories.
	if (!b::is_directory(new_dir))
		try
		{
			b::create_directory(new_dir);

			B_REQUIRE("create no parents" && false);
		}
		catch (b::system_exception&)
		{
		}

	try
	{
		using namespace b::args;

		// And create_directory() with the 'create_parents'
		// argument should.
		b::create_directory(new_dir, create_parents = true);
	}
	catch (b::system_exception&)
	{
		B_CHECK("create parents" && false);
	}

	try
	{
		b::remove_directory(new_dir);
		b::remove_directory(intermediate);
		b::remove_directory(test_dir);
	}
	catch (b::system_exception&)
	{
		B_CHECK("cleanup" && false);
	}
}
