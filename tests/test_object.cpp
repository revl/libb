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

#include <b/object.h>
#include <b/ref.h>

B_BEGIN_NAMESPACE

class Base : public Object
{
// Types
public:
	typedef Ref<Base> Ptr;

// Construction
public:
	static Ptr Create(int initial_value);

// Attributes
public:
	int GetValue() const;

	static int GetInstanceCount();

// Implementation
protected:
	Base(int initial_value);

	virtual ~Base()
		throw ();

private:
	int value;

	static int instance_count;
};

Base::Base(int initial_value) : value(initial_value)
{
	++instance_count;
}

Base::Ptr Base::Create(int initial_value)
{
	return new Base(initial_value);
}

int Base::GetValue() const
{
	return value;
}

int Base::GetInstanceCount()
{
	return instance_count;
}

Base::~Base()
	throw ()
{
	--instance_count;
}

int Base::instance_count = 0;

class Derived : public Base
{
// Types
public:
	typedef Ref<Derived> Ptr;

// Construction
public:
	static Ptr Create(int initial_value);

// Implementation
public:
	Derived(int initial_value);
};

inline Derived::Derived(int initial_value) : Base(initial_value)
{
}

inline Derived::Ptr Derived::Create(int initial_value)
{
	return new Derived(initial_value);
}

static int TestObject()
{
	try
	{
		Base::Ptr base1 = Base::Create(1);
		Base::Ptr base2 = Base::Create(2);

		Derived::Ptr derived = Derived::Create(3);

		base1 = base2;
		base2 = derived;

		base1.Swap(base2);

		if (Base::GetInstanceCount() != 2 ||
			base1->GetValue() != 3 ||
			base2->GetValue() != 2)
			return 4;
	}
	catch (RuntimeException& e)
	{
		String message;

		e.GetMessage(message);

#ifdef B_UNICODE
#define fprintf fwprintf
#endif

		fprintf(stderr, B_TEXT("Error: %s\n"),
			(const B_CHAR*) message);

		return 5;
	}

	return Base::GetInstanceCount();
}

B_END_NAMESPACE

int main()
{
	return b::TestObject();
}
