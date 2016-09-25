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

#ifndef B_UNIT_TEST_H
#define B_UNIT_TEST_H

#include <b/runtime_exception.h>

B_BEGIN_NAMESPACE

class test_case
{
public:
	virtual ~test_case() {}

	test_case(const char* name);

	virtual void run() = 0;

	const char* const test_name;
	test_case* const next_test_case;
	int failed_checks;
};

test_case* first_test_case = NULL;

inline test_case::test_case(const char* name) :
	test_name(name),
	next_test_case(first_test_case),
	failed_checks(0)
{
	first_test_case = this;
}

B_END_NAMESPACE

#define B_TEST_CASE(class_name) \
	class class_name : public b::test_case \
	{ \
	public: \
		class_name(const char* name) : b::test_case(name) \
		{ \
		} \
		virtual void run(); \
	} class_name##_instance(#class_name); \
	void class_name::run()

#define B_CHECK(condition) \
	if (!(condition)) { \
		++failed_checks; \
		fputs("FAILED CHECK: " #condition "\n", stderr); \
	}

#define B_REQUIRE(condition) \
	if (!(condition)) { \
		++failed_checks; \
		fputs("FAILED REQUIREMENT: " #condition "\n", stderr); \
		return; \
	}

int main(int /*argc*/, char* /*argv*/[])
{
	int failed_tests = 0;
	b::test_case* current_test_case = b::first_test_case;

	while (current_test_case != NULL)
	{
		try
		{
			current_test_case->run();

			if (current_test_case->failed_checks > 0)
				++failed_tests;
		}
		catch (b::runtime_exception& e)
		{
			++failed_tests;

			fprintf(stderr, "Exception in %s: %s\n",
				current_test_case->test_name,
				e.message().c_str());
		}
		catch (...)
		{
			++failed_tests;

			fprintf(stderr, "Unhandled exception in %s\n",
				current_test_case->test_name);
		}

		current_test_case = current_test_case->next_test_case;
	}

	return failed_tests;
}

#endif /* !defined(B_UNIT_TEST_H) */
