// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/object.h>

B_BEGIN_NAMESPACE

void object::release() const
{
	B_ASSERT(this != NULL);

	if (--refs == 0)
		delete_this();
}

void object::delete_this() const
{
	delete const_cast<object*>(this);
}

B_END_NAMESPACE
