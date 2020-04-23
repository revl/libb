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

#ifndef B_TEST_CASE_H
#define B_TEST_CASE_H

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

	virtual void run() const = 0;

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

class abort_test_case : public exception
{
};

linked_list<test_case::node_access> test_case::test_case_list =
	test_case::node_access();

test_case* current_test_case = NULL;

B_END_NAMESPACE

#define B_TEST_CASE(class_name) \
	class test_##class_name : public b::test_case \
	{ \
	public: \
		test_##class_name(const char* name) : b::test_case(name) \
		{ \
		} \
		virtual void run() const; \
	} static test_##class_name##_instance(#class_name); \
	void test_##class_name::run() const

#define B_CHECK(condition) \
	do \
		if (!(condition)) { \
			fprintf(stderr, "%s:%d: error: " \
				"check \"%s\" failed\n", \
				__FILE__, __LINE__, #condition); \
			++b::current_test_case->failed_checks; \
		} \
	while (0)

#define B_REQUIRE(condition) \
	do \
		if (!(condition)) { \
			fprintf(stderr, "%s:%d: error: required " \
				"check \"%s\" failed\n", \
				__FILE__, __LINE__, #condition); \
			throw b::abort_test_case(); \
		} \
	while (0)

#define B_REQUIRE_EXCEPTION(operation, expected_message_pattern) \
	do \
		try \
		{ \
			operation; \
			fprintf(stderr, "%s:%d: error: \"%s\" failed " \
				"to throw an exception\n", \
				__FILE__, __LINE__, #operation); \
			throw b::abort_test_case(); \
		} \
		catch (b::runtime_exception& e) \
		{ \
			if (!b::match_pattern(e.message(), \
					expected_message_pattern)) \
			{ \
				fprintf(stderr, "%s:%d: error: exception " \
					"message \"%s\" does not match " \
					"the expected pattern \"%s\"\n", \
					__FILE__, __LINE__, \
					e.message().data(), \
					expected_message_pattern); \
				throw b::abort_test_case(); \
			} \
		} \
	while (0)

int main(int /*argc*/, char* /*argv*/[])
{
	int failed_tests = 0;
	b::current_test_case = b::test_case::test_case_list.first();

	while (b::current_test_case != NULL)
	{
		try
		{
			b::current_test_case->run();

			if (b::current_test_case->failed_checks > 0)
				++failed_tests;
		}
		catch (b::abort_test_case&)
		{
			++failed_tests;
		}
		catch (b::runtime_exception& e)
		{
			++failed_tests;

			fprintf(stderr, "Exception in %s: %s\n",
				b::current_test_case->test_name,
				e.message().data());
		}
		catch (...)
		{
			++failed_tests;

			fprintf(stderr, "Unhandled exception in %s\n",
				b::current_test_case->test_name);
		}

		b::current_test_case =
			b::test_case::test_case_list.next(b::current_test_case);
	}

	return failed_tests;
}

#endif /* !defined(B_TEST_CASE_H) */
