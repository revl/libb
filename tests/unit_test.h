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

#ifndef B_UNIT_TEST_DEFINED
#define B_UNIT_TEST_DEFINED

B_BEGIN_NAMESPACE

class test_case
{
public:
	test_case(const char* name);

	virtual bool run() const = 0;

	const char* const test_name;
	const test_case* const next_test_case;
};

const test_case* first_test_case = NULL;

inline test_case::test_case(const char* name) :
	test_name(name),
	next_test_case(first_test_case)
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
		virtual bool run() const; \
	} class_name##_instance(#class_name); \
	bool class_name::run() const
	
#define B_CHECK(condition, message) \
	if (!(condition)) { \
		fprintf(stderr, "%s\n", message); \
		return false; \
	}

int main(int /*argc*/, char* /*argv*/[])
{
	int failed_tests = 0;
	const b::test_case* current_test_case = b::first_test_case;

	while (current_test_case != NULL)
	{
		try
		{
			if (!current_test_case->run())
				++failed_tests;
		}
		catch (...)
		{
			fprintf(stderr, "Unhandled exception in %s\n",
				current_test_case->test_name);
		}

		current_test_case = current_test_case->next_test_case;
	}

	return failed_tests;
}

#endif // B_UNIT_TEST_DEFINED
