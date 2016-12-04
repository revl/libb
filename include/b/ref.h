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

#ifndef B_REF_H
#define B_REF_H

#include "host.h"

B_BEGIN_NAMESPACE

// Smart pointer to instances of all classes derived from the Object class.
template <class TYPE>
class ref
{
// Constructors
public:
	// Constructs a null pointer.
	ref();

	// Increases the reference count of the object pointed to by <that>.
	ref(const ref<TYPE>& that);

	// Initializes this instance with a pointer to an existing object.
	ref(TYPE* that_ptr);

// Attributes
public:
	// Tests if this is a null pointer.
	bool IsNull() const;

	// Returns a C++ pointer to the controlled object.
	TYPE* GetPtr() const;

	// The same as GetPtr().
	operator TYPE*() const;

	// Member selection operator.
	TYPE* operator ->() const;

	// Dereference operator.
	TYPE& operator *() const;

	// Various comparison operators.
	bool operator ==(const ref<TYPE>& that) const;
	bool operator ==(TYPE* that_ptr) const;

	bool operator !=(const ref<TYPE>& that) const;
	bool operator !=(TYPE* that_ptr) const;

	bool operator <(const ref<TYPE>& that) const;
	bool operator <(TYPE* that_ptr) const;

	bool operator >(const ref<TYPE>& that) const;
	bool operator >(TYPE* that_ptr) const;

	bool operator <=(const ref<TYPE>& that) const;
	bool operator <=(TYPE* that_ptr) const;

	bool operator >=(const ref<TYPE>& that) const;
	bool operator >=(TYPE* that_ptr) const;

// Operations
public:
	// Switches to a new object. Releases the previously
	// controlled object and grabs a reference to the new one.
	void Assign(TYPE* that_ptr);

	// Assignment operators.
	ref<TYPE>& operator =(const ref<TYPE>& that);
	ref<TYPE>& operator =(TYPE* that_ptr);

	// Switches to a new object without incrementing its
	// reference count. Releases the previously controlled object.
	void Attach(TYPE* that_ptr);

	// Detaches this smart pointer from the object it points to
	// without decrementing the reference count of the object,
	// then resets this smart pointer to zero. Returns a C++
	// pointer to the detached object.
	TYPE* Detach();

	// Makes this smart pointer instance point to the object
	// pointed to by <that> and vice versa.
	void swap(ref<TYPE>& that);

// Implementation
public:
	// Releases the controlled object.
	~ref();

protected:
	TYPE* ptr;
};

template <class TYPE>
ref<TYPE>::ref() : ptr(NULL)
{
}

template <class TYPE>
ref<TYPE>::ref(const ref<TYPE>& that)
{
	if ((ptr = that.ptr) != NULL)
		ptr->add_ref();
}

template <class TYPE>
ref<TYPE>::ref(TYPE* that_ptr)
{
	if ((ptr = that_ptr) != NULL)
		ptr->add_ref();
}

template <class TYPE>
bool ref<TYPE>::IsNull() const
{
	return ptr == NULL;
}

template <class TYPE>
TYPE* ref<TYPE>::GetPtr() const
{
	return ptr;
}

template <class TYPE>
ref<TYPE>::operator TYPE*() const
{
	return ptr;
}

template <class TYPE>
TYPE* ref<TYPE>::operator ->() const
{
	B_ASSERT(ptr != NULL);

	return ptr;
}

template <class TYPE>
TYPE& ref<TYPE>::operator *() const
{
	return *ptr;
}

template <class TYPE>
bool ref<TYPE>::operator ==(const ref<TYPE>& that) const
{
	return ptr == that.ptr;
}

template <class TYPE>
bool ref<TYPE>::operator ==(TYPE* that_ptr) const
{
	return ptr == that_ptr;
}

template <class TYPE>
bool ref<TYPE>::operator !=(const ref<TYPE>& that) const
{
	return ptr != that.ptr;
}

template <class TYPE>
bool ref<TYPE>::operator !=(TYPE* that_ptr) const
{
	return ptr != that_ptr;
}

template <class TYPE>
bool ref<TYPE>::operator <(const ref<TYPE>& that) const
{
	return ptr < that.ptr;
}

template <class TYPE>
bool ref<TYPE>::operator <(TYPE* that_ptr) const
{
	return ptr < that_ptr;
}

template <class TYPE>
bool ref<TYPE>::operator >(const ref<TYPE>& that) const
{
	return ptr > that.ptr;
}

template <class TYPE>
bool ref<TYPE>::operator >(TYPE* that_ptr) const
{
	return ptr > that_ptr;
}

template <class TYPE>
bool ref<TYPE>::operator <=(const ref<TYPE>& that) const
{
	return ptr <= that.ptr;
}

template <class TYPE>
bool ref<TYPE>::operator <=(TYPE* that_ptr) const
{
	return ptr <= that_ptr;
}

template <class TYPE>
bool ref<TYPE>::operator >=(const ref<TYPE>& that) const
{
	return ptr >= that.ptr;
}

template <class TYPE>
bool ref<TYPE>::operator >=(TYPE* that_ptr) const
{
	return ptr >= that_ptr;
}

template <class TYPE>
void ref<TYPE>::Assign(TYPE* that_ptr)
{
	if (that_ptr != NULL)
		that_ptr->add_ref();

	TYPE* old_ptr = ptr;

	ptr = that_ptr;

	if (old_ptr != NULL)
		old_ptr->release();
}

template <class TYPE>
ref<TYPE>& ref<TYPE>::operator =(const ref<TYPE>& that)
{
	Assign(that.ptr);

	return *this;
}

template <class TYPE>
ref<TYPE>& ref<TYPE>::operator =(TYPE* that_ptr)
{
	Assign(that_ptr);

	return *this;
}

template <class TYPE>
void ref<TYPE>::Attach(TYPE* that_ptr)
{
	if (ptr != NULL)
		ptr->release();

	ptr = that_ptr;
}

template <class TYPE>
TYPE* ref<TYPE>::Detach()
{
	TYPE* this_ptr = ptr;

	ptr = NULL;
	return this_ptr;
}

template <class TYPE>
void ref<TYPE>::swap(ref<TYPE>& that)
{
	TYPE* this_ptr = ptr;

	ptr = that.ptr;
	that.ptr = this_ptr;
}

template <class TYPE>
ref<TYPE>::~ref()
{
	if (ptr != NULL)
		ptr->release();
}

B_END_NAMESPACE

#endif /* !defined(B_REF_H) */
