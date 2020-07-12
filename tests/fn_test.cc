// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/fn.h>

#include <b/heap.h>

#include "test_case.h"

B_TEST_CASE(tag)
{
	B_CHECK(B_TAG() == 0);
	B_CHECK(B_TAG(a) == 1);

	B_CHECK(b::tag_to_string(B_TAG()) == "");
	B_CHECK(b::tag_to_string(B_TAG(abc)) == "abc");

	B_CHECK(b::tag_to_string(B_TAG(aaaaaa)) == "aaaaaa");
	B_CHECK(b::tag_to_string(B_TAG(zzzzzz)) == "zzzzzz");

	B_CHECK(b::tag_to_string(B_TAG(longerthansix)) == "longer");
}

static bool match_and_check_all(const char* input, const char* pattern,
	bool expected_match)
{
	b::string_view input_sv(input, b::calc_length(input));
	b::string_view pattern_sv(pattern, b::calc_length(pattern));

	return (expected_match ? 4 : 0) ==
		(int) b::match_pattern(input, pattern) +
		(int) b::match_pattern(input, pattern_sv) +
		(int) b::match_pattern(input_sv, pattern) +
		(int) b::match_pattern(input_sv, pattern_sv);
}

#define CHECK_MATCH(input, pattern, expected_match) \
	B_CHECK(match_and_check_all(input, pattern, expected_match))

B_TEST_CASE(pattern_matching)
{
	CHECK_MATCH("", "*?*", false);
	CHECK_MATCH("a", "a*", true);
	CHECK_MATCH("abc", "abc", true);
	CHECK_MATCH("abc", "a*", true);
	CHECK_MATCH("abc", "*b*", true);
	CHECK_MATCH("abc", "*c", true);
	CHECK_MATCH("abc", "*", true);
	CHECK_MATCH("abcd", "ab*c", false);
	CHECK_MATCH("abcdbc", "abd*", false);
	CHECK_MATCH("abcdbcda", "a?*cda", true);
	CHECK_MATCH("abcdefgh", "ab?d*ef?h", true);
	CHECK_MATCH("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*b", false);
	CHECK_MATCH("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
		"a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*", true);
	CHECK_MATCH("aaaaaa", "aaa*aaa", true);
	CHECK_MATCH("aaaaaa", "aaa**aaa", true);
	CHECK_MATCH("aaa", "*a", true);
	CHECK_MATCH("ab!bb!db!eb!czz!", "a*b?c*!", true);
	CHECK_MATCH("ab!bb!db!eb!czzy", "a*b?c*!", false);
}

B_TEST_CASE(version_comparison)
{
	B_CHECK(b::compare_versions("11.2", "2.11") > 0);
	B_CHECK(b::compare_versions("1.1", "1.1.1") < 0);
	B_CHECK(b::compare_versions("1.2.3", "1.2.3") == 0);
	B_CHECK(b::compare_versions("1.22.1", "1.6.1") > 0);
	B_CHECK(b::compare_versions("1.2.3.4", "1.2.3") > 0);
}

B_TEST_CASE(signed_char_alignment)
{
	B_CHECK(b::align((signed char) 6, 3) == (signed char) 6);
	B_CHECK(b::align((signed char) 7, 3) == (signed char) 9);
	B_CHECK(b::align((signed char) 8, 4) == (signed char) 8);
	B_CHECK(b::align((signed char) 9, 6) == (signed char) 12);
}

B_TEST_CASE(pointer_alignment)
{
	B_CHECK(b::align((void*) 9, 8) == (void*) 16);
	B_CHECK(b::align((void*) 0, 16) == (void*) 0);
	B_CHECK(b::align((void*) 1, 32) == (void*) 32);
}

B_TEST_CASE(shuffle)
{
	unsigned numbers[100];

	for (unsigned i = 0; i < B_COUNTOF(numbers); ++i)
		numbers[i] = i;

	bool ordered = true;

	b::pseudorandom prng;

	b::shuffle_array(numbers, B_COUNTOF(numbers), prng);

	for (unsigned i = 0; i < B_COUNTOF(numbers); ++i)
		if (numbers[i] != i)
			ordered = false;

	B_CHECK(!ordered);

	b::heapsort(numbers, B_COUNTOF(numbers));

	for (unsigned i = 0; i < B_COUNTOF(numbers); ++i)
		B_CHECK(numbers[i] == i);
}

B_STRING_LITERAL(dot_dir, ".");

B_STRING_LITERAL(test_dir, "b_test_dir");

B_STRING_LITERAL(intermediate, "b_test_dir" B_PATH_SEPARATOR_STR
	"missing_parent");

B_STRING_LITERAL(new_dir, "b_test_dir" B_PATH_SEPARATOR_STR
	"missing_parent" B_PATH_SEPARATOR_STR "new_dir");

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
		B_REQUIRE_EXCEPTION(b::create_directory(new_dir),
			(new_dir + '*').data());

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

static void check_base64url(const b::string_view& decoded,
		const b::string_view& encoded)
{
	B_CHECK(base64url_encode(decoded) == encoded);

	B_CHECK(base64url_decode(encoded) == decoded);
}

B_TEST_CASE(base64url)
{
	check_base64url(B_STRING_VIEW("Man"), B_STRING_VIEW("TWFu"));

	check_base64url(B_STRING_VIEW("Ma"), B_STRING_VIEW("TWE"));

	check_base64url(B_STRING_VIEW("M"), B_STRING_VIEW("TQ"));

	check_base64url(B_STRING_VIEW("pleasure."),
		B_STRING_VIEW("cGxlYXN1cmUu"));

	check_base64url(B_STRING_VIEW("leasure."),
		B_STRING_VIEW("bGVhc3VyZS4"));

	check_base64url(B_STRING_VIEW("easure."), B_STRING_VIEW("ZWFzdXJlLg"));

	check_base64url(B_STRING_VIEW("asure."), B_STRING_VIEW("YXN1cmUu"));

	check_base64url(B_STRING_VIEW("sure."), B_STRING_VIEW("c3VyZS4"));
}
