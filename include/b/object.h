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

#ifndef B_OBJECT_H
#define B_OBJECT_H

#include "ref_count.h"
#include "memory.h"

B_BEGIN_NAMESPACE

// Base class with reference count support.
class Object
{
// Construction
public:
	// Allocates objects of the derived classes.
	static void* operator new(size_t size);

	// Deallocates objects previously allocated by operator new.
	// Inlined here to get rid of a warning issued by gcc 4.0.1:
	// 'operator delete' was used before it was declared inline.
	static void operator delete(void* object, size_t size)
	{
		Memory::FixedFree(object, size);
	}

protected:
	// Initializes the reference count with zero.
	Object();

	// Initializes the reference count with zero.
	Object(const Object&);

// Operations
public:
	// Increases the reference count by one protecting
	// this object from being deleted.
	void AddRef() const;

	// Decreases the reference count, and, if it becomes
	// zero, deletes this object.
	void Release() const;

	// Protects the reference counter from being modified
	// by the default (generated by the compiler)
	// assignment operator.
	Object& operator =(Object&);

// Implementation
protected:
	// Protected destructor prohibits direct calls to
	// operator delete.
	virtual ~Object()
		throw ()
	{
		B_ASSERT(refs <= 0);
	}

	// The reference count object.
	mutable RefCount refs;
};

inline void* Object::operator new(size_t size)
{
	return Memory::FixedAlloc(size > B_MIN_FIXED_ALLOC ?
		size : B_MIN_FIXED_ALLOC);
}

inline Object::Object()
{
	refs = 0;
}

inline Object::Object(const Object&)
{
	refs = 0;
}

inline void Object::AddRef() const
{
	++refs;
}

inline void Object::Release() const
{
	B_ASSERT(this != NULL);

	if (!--refs)
		delete const_cast<Object*>(this);
}

inline Object& Object::operator =(Object&)
{
	return *this;
}

B_END_NAMESPACE

#include <b/ref.h>

#endif /* !defined(B_OBJECT_H) */
