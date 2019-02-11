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

#ifndef B_OBJECT_H
#define B_OBJECT_H

#include "host.h"
#include "memory.h"

B_BEGIN_NAMESPACE

// Base class with reference count support. Designed to be used
// with the ref<> template.
class object
{
public:
	// Allocates objects of the derived classes.
	static void* operator new(size_t size);

	// Deallocates objects previously allocated by operator new.
	// Inlined here to get rid of a warning issued by gcc 4.0.1:
	// 'operator delete' was used before it was declared inline.
	static void operator delete(void* object, size_t size)
	{
		memory::fixed_free(object, size);
	}

protected:
	// Initializes the reference count with zero.
	object();

	// Initializes the reference count with zero.
	// A newly created object has no references,
	// even if it is a copy of an existing object.
	object(const object&);

public:
	// Increases the reference count by one.
	void add_ref() const;

	// Decreases the reference count and, if it becomes
	// zero, calls the delete_this() method.
	void release() const;

	// Makes sure the counter is not overwritten when
	// one object is assigned to another.
	object& operator =(object&);

protected:
	// Deletes this object. The method is called by the
	// release() method when there are no more references
	// to this object.
	virtual void delete_this() const;

	// Protected destructor prohibits explicit deletion
	// of this object.
	virtual ~object()
	{
		B_ASSERT(refs <= 0);
	}

	// The reference count object.
	mutable int refs;
};

inline void* object::operator new(size_t size)
{
	return memory::fixed_alloc(size > B_MIN_FIXED_ALLOC ?
		size : B_MIN_FIXED_ALLOC);
}

inline object::object()
{
	refs = 0;
}

inline object::object(const object&)
{
	refs = 0;
}

inline void object::add_ref() const
{
	++refs;
}

inline object& object::operator =(object&)
{
	return *this;
}

B_END_NAMESPACE

#include <b/ref.h>

#endif /* !defined(B_OBJECT_H) */
