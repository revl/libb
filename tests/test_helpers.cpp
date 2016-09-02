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

struct MatchPatternTestCase
{
	const B_CHAR* string;
	const B_CHAR* pattern;
	bool expected_result;

	bool Test() const;
	bool Assert(const char* function_name, bool result) const;
};

bool MatchPatternTestCase::Test() const
{
	size_t string_length = b::CalcLength(string);
	size_t pattern_length = b::CalcLength(pattern);

	return Assert("MatchPatternZZ", b::MatchPatternZZ(string,
			pattern)) &&
		Assert("MatchPatternZR", b::MatchPatternZR(string,
			pattern, pattern + pattern_length)) &&
		Assert("MatchPatternRZ", b::MatchPatternRZ(string,
			string + string_length, pattern)) &&
		Assert("MatchPatternRR", b::MatchPatternRR(string,
			string + string_length, pattern,
			pattern + pattern_length));
}

bool MatchPatternTestCase::Assert(const char* function_name, bool result) const
{
	if (result != expected_result)
	{
		fprintf(stderr,
#ifdef B_UNICODE
			"%s(\"%ls\", \"%ls\") returned %s\n",
#else
			"%s(\"%s\", \"%s\") returned %s\n",
#endif // defined(B_UNICODE)
			function_name, string, pattern,
			result ? "true" : "false");

		return false;
	}

	return true;
}

static const MatchPatternTestCase match_pattern_test_cases[] =
{
	{B_TEXT(""), B_TEXT("*?*"), false},
	{B_TEXT("abc"), B_TEXT("*b*"), true},
	{B_TEXT("abcd"), B_TEXT("ab*c"), false},
	{B_TEXT("abcdbc"), B_TEXT("abd*"), false},
	{B_TEXT("abcdbcda"), B_TEXT("a?*cda"), true},
	{B_TEXT("abcdefgh"), B_TEXT("ab?d*ef?h"), true},
	{B_TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
		B_TEXT("a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*b"), false},
	{B_TEXT("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"),
		B_TEXT("a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*a*"), true}
};

struct VersionComparisonTestCase
{
	const B_CHAR* version1;
	const B_CHAR* version2;
	int expected_result;

	bool Test() const;
};

bool VersionComparisonTestCase::Test() const
{
	int diff = b::CompareVersionStrings(version1, version2);

	if (expected_result == 1)
		return diff > 0;

	if (expected_result == 0)
		return diff == 0;

	return diff < 0;
}

static const VersionComparisonTestCase version_comparison_test_cases[] =
{
	{B_TEXT("11.2"), B_TEXT("2.11"), 1},
	{B_TEXT("1.1"), B_TEXT("1.1.1"), -1},
	{B_TEXT("1.2.3"), B_TEXT("1.2.3"), 0},
	{B_TEXT("1.22.1"), B_TEXT("1.6.1"), 1},
	{B_TEXT("1.2.3.4"), B_TEXT("1.2.3"), 1}
};

struct SignedCharAlignmentTestCase
{
	signed char value;
	size_t alignment;
	signed char expected_result;

	bool Test() const;
};

inline bool SignedCharAlignmentTestCase::Test() const
{
	return b::Align(value, alignment) == expected_result;
}

static const SignedCharAlignmentTestCase signed_char_alignment_test_cases[] =
{
	{6, 3, 6},
	{7, 3, 9},
	{8, 4, 8},
	{9, 6, 12}
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
	return b::Align(value, alignment) == expected_result;
}

static const PtrAlignmentTestCase ptr_alignment_test_cases[] =
{
	{(void*) 9, 8, (void*) 16},
	{(void*) 0, 16, (void*) 0},
	{(void*) 1, 32, (void*) 32}
};

int main()
{
	size_t index = sizeof(match_pattern_test_cases) /
		sizeof(*match_pattern_test_cases);

	const MatchPatternTestCase* match_pattern_test_case =
		match_pattern_test_cases;

	while (index-- > 0)
		if (!match_pattern_test_case++->Test())
			return 1;

	index = sizeof(version_comparison_test_cases) /
		sizeof(*version_comparison_test_cases);

	const VersionComparisonTestCase* version_comparison_test_case =
		version_comparison_test_cases;

	while (index-- > 0)
		if (!version_comparison_test_case++->Test())
			return 2;

	index = sizeof(signed_char_alignment_test_cases) /
		sizeof(*signed_char_alignment_test_cases);

	const SignedCharAlignmentTestCase* signed_char_alignment_test_case =
		signed_char_alignment_test_cases;

	while (index-- > 0)
		if (!signed_char_alignment_test_case++->Test())
			return 3;

	index = sizeof(ptr_alignment_test_cases) /
		sizeof(*ptr_alignment_test_cases);

	const PtrAlignmentTestCase* ptr_alignment_test_case =
		ptr_alignment_test_cases;

	while (index-- > 0)
		if (!ptr_alignment_test_case++->Test())
			return 4;

	return 0;
}
