/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2019 Damon Revoe
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

#include "unit_test.h"

B_TEST_CASE(opaque_pointer)
{
	handle instance = handle::create_handle();

	B_CHECK(instance.value() == 100);
}
