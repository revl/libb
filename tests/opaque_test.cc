// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/opaque.h>

class handle
{
B_OPAQUE:
	static handle create_handle();

	int value() const;
};

#include <b/object.h>

class handle::impl : public b::object
{
public:
	impl() : value(100)
	{
	}

	int value;
};

handle handle::create_handle()
{
	handle new_object;

	new_object.impl_ref = new handle::impl;

	return new_object;
}

int handle::value() const
{
	return impl_ref->value;
}

#include "test_case.h"

B_TEST_CASE(opaque_pointer)
{
	handle instance = handle::create_handle();

	B_CHECK(instance.value() == 100);
}
