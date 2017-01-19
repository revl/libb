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

#include <b/object.h>

#include "unit_test.h"

class base : public b::object
{
public:
	typedef b::ref<base> ref;

	static ref create(int new_object_id);

	int id() const;

	static int instance_count();

protected:
	base(int new_object_id);

	virtual ~base();

private:
	const int object_id;

	static int instance_counter;
};

base::base(int new_object_id) : object_id(new_object_id)
{
	++instance_counter;
}

base::ref base::create(int new_object_id)
{
	return new base(new_object_id);
}

int base::id() const
{
	return object_id;
}

int base::instance_count()
{
	return instance_counter;
}

base::~base()
{
	--instance_counter;
}

int base::instance_counter = 0;

class derived : public base
{
public:
	typedef b::ref<derived> ref;

	static ref create(int new_object_id);

	derived(int new_object_id);
};

inline derived::derived(int new_object_id) : base(new_object_id)
{
}

inline derived::ref derived::create(int new_object_id)
{
	return new derived(new_object_id);
}

B_TEST_CASE(object_count)
{
	{
		base::ref base1 = base::create(1);
		base::ref base2 = base::create(2);

		derived::ref derived = derived::create(3);

		base1 = base2;
		base2 = derived;

		base1.swap(base2);

		B_CHECK(base::instance_count() == 2);
		B_CHECK(base1->id() == 3);
		B_CHECK(base2->id() == 2);
	}

	B_CHECK(base::instance_count() == 0);
}
