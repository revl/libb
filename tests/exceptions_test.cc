// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/custom_exception.h>

#include "test_case.h"

B_TEST_CASE(exception_message)
{
	B_STRING_LITERAL(test_message, "This is a test");

	try
	{
		throw b::custom_exception(test_message);
	}
	catch (b::runtime_exception& e)
	{
		B_CHECK(e.message() == test_message);
	}
}

B_TEST_CASE(formatted_exception_message)
{
	try
	{
		throw b::custom_exception("This is a %s", "test");
	}
	catch (b::runtime_exception& e)
	{
		B_CHECK(e.message() == "This is a test");
	}
}
