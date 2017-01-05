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

#ifndef B_UNIT_TEST_H
#define B_UNIT_TEST_H

#include <b/runtime_exception.h>
#include <b/linked_list.h>
#include <b/node_access_via_cast.h>

B_BEGIN_NAMESPACE

class test_case
{
public:
	virtual ~test_case() {}

	test_case(const char* name) : test_name(name), failed_checks(0)
	{
		test_case::test_case_list.append(this);
	}

	virtual void run() = 0;

	const char* const test_name;
	int failed_checks;

	typedef b::linked_list_node<test_case> list_node_type;

	list_node_type list_node;

	operator list_node_type&()
	{
		return list_node;
	}

	typedef node_access_via_cast<list_node_type> node_access;

	static linked_list<node_access> test_case_list;
};

linked_list<test_case::node_access> test_case::test_case_list =
	test_case::node_access();

B_END_NAMESPACE

#define B_TEST_CASE(class_name) \
	class test_##class_name : public b::test_case \
	{ \
	public: \
		test_##class_name(const char* name) : b::test_case(name) \
		{ \
		} \
		virtual void run(); \
	} static test_##class_name##_instance(#class_name); \
	void test_##class_name::run()

#define B_CHECK(condition) \
	do \
		if (!(condition)) { \
			++failed_checks; \
			fputs("FAILED CHECK: " #condition "\n", stderr); \
		} \
	while (0)

#define B_REQUIRE(condition) \
	do \
		if (!(condition)) { \
			++failed_checks; \
			fputs("FAILED REQUIREMENT: " #condition "\n", stderr); \
			return; \
		} \
	while (0)

int main(int /*argc*/, char* /*argv*/[])
{
	int failed_tests = 0;
	b::test_case* current_test_case = b::test_case::test_case_list.first();

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
				e.message().data());
		}
		catch (...)
		{
			++failed_tests;

			fprintf(stderr, "Unhandled exception in %s\n",
				current_test_case->test_name);
		}

		current_test_case =
			b::test_case::test_case_list.next(current_test_case);
	}

	return failed_tests;
}

#endif /* !defined(B_UNIT_TEST_H) */
