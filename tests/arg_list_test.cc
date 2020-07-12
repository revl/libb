// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/arg_list.h>

#include "test_case.h"

// Test case 1. Define named arguments using an enumeration.
// Use a switch to recognize argument types.

struct parameters
{
	b::string sp;
	int ip;
	bool bp;

	bool operator ==(const parameters& rhs) const
	{
		return sp == rhs.sp && ip == rhs.ip && bp == rhs.bp;
	}
};

namespace init_args
{
	enum
	{
		sp_tag,
		ip_tag,
		bp_tag
	};

	b::arg_name<b::string, sp_tag> sp;
	b::arg_name<int, ip_tag> ip;
	b::arg_name<bool, bp_tag> bp;
}

static parameters init(const b::arg_list* arg)
{
	parameters result =
	{
		b::string(),
		0,
		false
	};

	for (; arg != NULL; arg = arg->prev_arg)
		switch (arg->tag)
		{
		case init_args::sp_tag:
			result.sp = init_args::sp.value(arg);
			break;

		case init_args::ip_tag:
			result.ip = init_args::ip.value(arg);
			break;

		case init_args::bp_tag:
			result.bp = init_args::bp.value(arg);
		}

	return result;
}

B_STRING_LITERAL(string_value, "string value");

B_TEST_CASE(arg_list)
{
	using namespace init_args;

	parameters set = init((bp = true, ip = 100, sp = string_value));

	parameters expected =
	{
		string_value,
		100,
		true
	};

	B_CHECK(set == expected);
}

// Test case 2. Define named arguments using integer constants.
// Use a series of if/else statements to recognize argument types.

b::arg_name<unsigned, 0> addend;
b::arg_name<unsigned*, 1> partial_sum;

static unsigned sum(const b::arg_list* arg = NULL)
{
	unsigned sum = 0U;

	for (; arg; arg = arg->prev_arg)
		if (addend.is_name_for(arg))
			sum += addend.value(arg);
		else
			if (partial_sum.is_name_for(arg))
				*partial_sum.value(arg) = sum;

	return sum;
}

B_TEST_CASE(argument_order)
{
	unsigned ps1, ps2;

	unsigned s = sum((addend = 1,
			partial_sum = &ps2,
			addend = 10,
			addend = 100,
			partial_sum = &ps1,
			addend = 1000,
			addend = 10000,
			addend = 100000));

	B_CHECK(s = 111111);
	B_CHECK(ps1 = 111000);
	B_CHECK(ps2 = 111110);
}

// Test case 3. Have a list of named parameters
// shared between two function calls.

b::arg_name<int, 0> int_arg;

static bool check_values(const b::arg_list* arg,
		const int* expected_values,
		int number_of_expected_values)
{
	for (; arg != NULL && --number_of_expected_values >= 0;
			arg = arg->prev_arg)
		if (expected_values[number_of_expected_values] !=
				int_arg.value(arg))
			return false;

	return number_of_expected_values == 0;
}

B_TEST_CASE(arg_list_continuation)
{
	const b::arg_list& one = int_arg = 1;
	const b::arg_list& two = int_arg = 2;
	const b::arg_list& three = int_arg = 3;

	const b::arg_list& common_args = (one, two, three);

	static const int e1[3] =
	{
		1,
		2,
		3
	};

	check_values(common_args, e1, 3);

	static const int e2[5] =
	{
		1,
		2,
		3,
		4,
		5
	};

	check_values((common_args, int_arg = 4, int_arg = 5), e2, 5);

	static const int e3[4] =
	{
		1,
		2,
		3,
		10
	};

	check_values((common_args, int_arg = 10), e3, 4);
}
