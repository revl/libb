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

#include <b/pathname.h>

#include "unit_test.h"

B_TEST_CASE(test_pathname_normalization)
{
	B_STATIC_CONST_STRING(with_double_dot, "d1/d2/../f.n.sh");

	b::pathname normalized_path(with_double_dot);

	B_CHECK(normalized_path.can_represent_file());

	const b::pathname::component_array& components =
		normalized_path.components();

	B_REQUIRE(components.size() == 2);

	B_CHECK(components.front().name() == "d1");

	const b::pathname::component& filename = components.back();

	B_CHECK(filename.name() == "f.n.sh");
	B_CHECK(filename.basename() == "f.n");
	B_CHECK(filename.suffix() == ".sh");
}

B_TEST_CASE(test_filename_with_empty_suffix)
{
	B_STATIC_CONST_STRING(no_suffix, "README");

	B_CHECK(b::pathname(no_suffix).components().back().suffix().is_empty());

	B_STATIC_CONST_STRING(ends_with_dot, "period.");

	B_CHECK(b::pathname(ends_with_dot).components().back().suffix() == ".");
}

B_TEST_CASE(test_pathname_can_represent_file)
{
	B_STATIC_CONST_STRING(file_or_dir, "dir/subdir_or_file");

	B_CHECK(b::pathname(file_or_dir).can_represent_file());

	B_STATIC_CONST_STRING(ending_with_slash, "dir/subdir/");

	B_CHECK(!b::pathname(ending_with_slash).can_represent_file());

	B_STATIC_CONST_STRING(ending_with_dot, "dir/.");

	B_CHECK(!b::pathname(ending_with_dot).can_represent_file());

	B_STATIC_CONST_STRING(ending_with_double_dot, "dir/..");

	B_CHECK(!b::pathname(ending_with_double_dot).can_represent_file());
}

B_TEST_CASE(test_absolute_pathnames)
{
	B_STATIC_CONST_STRING(absolute_path, "/absolute/path");

	b::pathname path(absolute_path);

	B_CHECK(path.is_absolute());

	path.go_up_one_level();

	B_CHECK(path.is_absolute());

	B_CHECK(path.str() == "/absolute");

	path.go_up_one_level();
	path.go_up_one_level();

	B_CHECK(path.str() == "/");
}

static b::string inc(b::pathname& path, const char* increment)
{
	path.append(b::string_view(increment, b::calc_length(increment)));

	return path.str();
}

B_TEST_CASE(test_pathname_increments)
{
	B_STATIC_CONST_STRING(initial_path, "dir/subdir");

	b::pathname path(initial_path);

	B_CHECK(inc(path, "..") == "dir");
	B_CHECK(inc(path, "subdir") == initial_path);
	B_CHECK(inc(path, "../..") == ".");
	B_CHECK(inc(path, "../..") == "../..");
	B_CHECK(inc(path, "/") == "/");
	B_CHECK(inc(path, "root") == "/root");
	B_CHECK(inc(path, "///usr") == "/usr");
	B_CHECK(inc(path, "../var///lib") == "/var/lib");
	B_CHECK(inc(path, "../../../../../srv/") == "/srv");
}
