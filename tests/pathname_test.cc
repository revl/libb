/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

#include "test_case.h"

B_TEST_CASE(empty_pathname)
{
	B_CHECK(b::pathname().components().is_empty());

	b::pathname empty_path = b::string_view();

	B_CHECK(empty_path.components().is_empty());
}

B_TEST_CASE(pathname_normalization)
{
	B_STRING_LITERAL(with_double_dot, "d1/d2/../f.n.sh");

	b::pathname normalized_path(with_double_dot);

	B_CHECK(normalized_path.can_represent_file());

	const b::pathname::component_array& components =
		normalized_path.components();

	B_REQUIRE(components.length() == 2);

	B_CHECK(components.first().name() == "d1");

	const b::pathname::component& filename = components.last();

	B_CHECK(filename.name() == "f.n.sh");
	B_CHECK(filename.basename() == "f.n");
	B_CHECK(filename.suffix() == ".sh");
}

B_TEST_CASE(filename_with_empty_suffix)
{
	B_STRING_LITERAL(no_suffix, "README");

	B_CHECK(b::pathname(no_suffix).components().last().suffix().is_empty());

	B_STRING_LITERAL(ends_with_dot, "period.");

	B_CHECK(b::pathname(ends_with_dot).components().last().suffix() == ".");
}

B_TEST_CASE(pathname_can_represent_file)
{
	B_STRING_LITERAL(file_or_dir, "dir/subdir_or_file");

	B_CHECK(b::pathname(file_or_dir).can_represent_file());

	B_STRING_LITERAL(ending_with_slash, "dir/subdir/");

	B_CHECK(!b::pathname(ending_with_slash).can_represent_file());

	B_STRING_LITERAL(ending_with_dot, "dir/.");

	B_CHECK(!b::pathname(ending_with_dot).can_represent_file());

	B_STRING_LITERAL(ending_with_double_dot, "dir/..");

	B_CHECK(!b::pathname(ending_with_double_dot).can_represent_file());
}

B_TEST_CASE(absolute_pathnames)
{
	B_STRING_LITERAL(absolute_path, "/absolute/path");

	b::pathname path(absolute_path);

	B_CHECK(path.is_absolute());
	B_CHECK(path.can_represent_file());

	path.go_up_one_level();

	B_CHECK(!path.can_represent_file());

	B_CHECK(path.is_absolute());

	B_CHECK(path.str() == "/absolute");

	path.go_up_one_level();

	B_CHECK(path.str() == "/");

	path.go_up_one_level();

	B_CHECK(path.str() == "/");
}

B_TEST_CASE(up_levels)
{
	B_STRING_LITERAL(relative_path, "relative/path");

	b::pathname path(relative_path);

	B_CHECK(path.can_represent_file());

	path.join(B_STRING_VIEW(".."));

	B_CHECK(!path.can_represent_file());

	B_CHECK(path.str() == "relative");
	B_CHECK(path.number_of_levels_up() == 0);

	path.go_up_one_level();

	B_CHECK(path.str() == ".");
	B_CHECK(path.number_of_levels_up() == 0);

	path.go_up_one_level();

	B_CHECK(path.str() == "..");
	B_CHECK(path.number_of_levels_up() == 1);

	path.assign(relative_path);
	path.go_up(3);

	B_CHECK(!path.can_represent_file());

	B_CHECK(path.str() == "..");
	B_CHECK(path.number_of_levels_up() == 1);

	path.go_up(3);

	B_CHECK(path.str() == "../../../..");
	B_CHECK(path.number_of_levels_up() == 4);
}

B_STRING_LITERAL(initial_path, "dir/subdir");

static void check_join(const char* chdir, const char* expected,
		bool result_can_represent_file)
{
	b::string_view chdir_sv(chdir, b::calc_length(chdir));

	{
		static b::pathname path(initial_path);

		// Parse 'chdir' and join it to 'path'.
		path.join(chdir_sv);

		B_CHECK(path.str() == expected);
		B_CHECK(path.can_represent_file() == result_can_represent_file);
	}

	{
		static b::pathname path(initial_path);

		// Join a pre-parsed 'chdir'.
		path.join(b::pathname(chdir_sv));

		B_CHECK(path.str() == expected);
		B_CHECK(path.can_represent_file() == result_can_represent_file);
	}
}

B_TEST_CASE(pathname_increments)
{
	check_join("..", "dir", false);
	check_join("subdir", initial_path.data(), true);
	check_join("../..", ".", false);
	check_join("../..", "../..", false);
	check_join("/", "/", false);
	check_join("root", "/root", true);
	check_join("///usr", "/usr", true);
	check_join("../var///lib", "/var/lib", true);
	check_join("../../../../../srv/", "/srv", false);
}

B_TEST_CASE(pattern_match)
{
	b::pathname pn(B_STRING_VIEW(__FILE__));

	B_CHECK(pn.components().last().name().matches_pattern("*.cc"));
}

static void check_relative(const char* base_pathname,
		const char* target_pathname,
		const char* expected_result,
		bool result_can_represent_file)
{
	b::pathname base(b::string_view(base_pathname,
		b::calc_length(base_pathname)));

	b::pathname target(b::string_view(target_pathname,
		b::calc_length(target_pathname)));

	b::pathname relative = target.relative_to(base);

	B_CHECK(relative.str() == expected_result);
	B_CHECK(relative.can_represent_file() == result_can_represent_file);
}

B_TEST_CASE(relative)
{
	check_relative("../../../a/b", "../../../../c", "../../../c", true);
	check_relative("../../a/b/c", "../../../d/", "../../../../d", false);
	check_relative("a/b/.", "./a/b", ".", true);
	check_relative("./a/b", "a/b/.", ".", false);
	check_relative("a/b/c/d", "a/b/d", "../../d", true);
	check_relative("a/b", "a/b/c/d", "c/d", true);
	check_relative("a/b", "a/b/../c", "../c", true);
	check_relative("a/b/../c", "a/b", "../b", true);
	check_relative("a/b/c", "a/c/d", "../../c/d", true);
	check_relative("a/b", "c/d", "../../c/d", true);
	check_relative("a/b/c/d", "a/b", "../..", true);
	check_relative("a/b/c/d", "a/b/", "../..", false);
	check_relative("a/b/c/d/", "a/b", "../..", true);
	check_relative("a/b/c/d/", "a/b/", "../..", false);
	check_relative("../../a/b", "../../a/b/c/d", "c/d", true);
	check_relative("/a/b", "/a/b", ".", true);
	check_relative("/a/b/.", "/a/b", ".", true);
	check_relative("/a/b", "/a/b/.", ".", false);
	check_relative("/ab/cd", "/ab/cde", "../cde", true);
	check_relative("/ab/cd", "/ab/c", "../c", true);
	check_relative("/a/b", "/a/b/c/d", "c/d", true);
	check_relative("/a/b", "/a/b/../c", "../c", true);
	check_relative("/a/b/../c", "/a/b", "../b", true);
	check_relative("/a/b/c", "/a/c/d", "../../c/d", true);
	check_relative("/a/b", "/c/d", "../../c/d", true);
	check_relative("/a/b/c/d", "/a/b", "../..", true);
	check_relative("/a/b/c/d", "/a/b/", "../..", false);
	check_relative("/a/b/c/d/", "/a/b", "../..", true);
	check_relative("/a/b/c/d/", "/a/b/", "../..", false);
	check_relative("/../../a/b", "/../../a/b/c/d", "c/d", true);
	check_relative(".", "a/b", "a/b", true);
	check_relative(".", "..", "..", false);
}

B_TEST_CASE(relative_errors)
{
	const b::pathname abs(B_STRING_VIEW("/abs"));
	const b::pathname rel(B_STRING_VIEW("rel"));

	B_REQUIRE_EXCEPTION(abs.relative_to(rel),
		"Cannot make '/abs' relative to 'rel'.");

	B_REQUIRE_EXCEPTION(rel.relative_to(abs),
		"Cannot make 'rel' relative to '/abs'.");

	const b::pathname one_level_up(B_STRING_VIEW(".."));

	B_REQUIRE_EXCEPTION(rel.relative_to(one_level_up),
		"Cannot make 'rel' relative to '..'.");

	const b::pathname current_dir(B_STRING_VIEW("."));

	B_REQUIRE_EXCEPTION(current_dir.relative_to(one_level_up),
		"Cannot make '.' relative to '..'.");

	const b::pathname two_levels_up(B_STRING_VIEW("../.."));

	B_REQUIRE_EXCEPTION(one_level_up.relative_to(two_levels_up),
		"Cannot make '..' relative to '../..'.");
}
