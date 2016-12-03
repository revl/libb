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
class Ref
{
// Constructors
public:
	// Constructs a null pointer.
	Ref();

	// Increases the reference count of the object pointed to by <that>.
	Ref(const Ref<TYPE>& that);

	// Initializes this instance with a pointer to an existing object.
	Ref(TYPE* that_ptr);

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
	bool operator ==(const Ref<TYPE>& that) const;
	bool operator ==(TYPE* that_ptr) const;

	bool operator !=(const Ref<TYPE>& that) const;
	bool operator !=(TYPE* that_ptr) const;

	bool operator <(const Ref<TYPE>& that) const;
	bool operator <(TYPE* that_ptr) const;

	bool operator >(const Ref<TYPE>& that) const;
	bool operator >(TYPE* that_ptr) const;

	bool operator <=(const Ref<TYPE>& that) const;
	bool operator <=(TYPE* that_ptr) const;

	bool operator >=(const Ref<TYPE>& that) const;
	bool operator >=(TYPE* that_ptr) const;

// Operations
public:
	// Switches to a new object. Releases the previously
	// controlled object and grabs a reference to the new one.
	void Assign(TYPE* that_ptr);

	// Assignment operators.
	Ref<TYPE>& operator =(const Ref<TYPE>& that);
	Ref<TYPE>& operator =(TYPE* that_ptr);

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
	void swap(Ref<TYPE>& that);

// Implementation
public:
	// Releases the controlled object.
	~Ref();

protected:
	TYPE* ptr;
};

template <class TYPE>
Ref<TYPE>::Ref() : ptr(NULL)
{
}

template <class TYPE>
Ref<TYPE>::Ref(const Ref<TYPE>& that)
{
	if ((ptr = that.ptr) != NULL)
		ptr->add_ref();
}

template <class TYPE>
Ref<TYPE>::Ref(TYPE* that_ptr)
{
	if ((ptr = that_ptr) != NULL)
		ptr->add_ref();
}

template <class TYPE>
bool Ref<TYPE>::IsNull() const
{
	return ptr == NULL;
}

template <class TYPE>
TYPE* Ref<TYPE>::GetPtr() const
{
	return ptr;
}

template <class TYPE>
Ref<TYPE>::operator TYPE*() const
{
	return ptr;
}

template <class TYPE>
TYPE* Ref<TYPE>::operator ->() const
{
	B_ASSERT(ptr != NULL);

	return ptr;
}

template <class TYPE>
TYPE& Ref<TYPE>::operator *() const
{
	return *ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator ==(const Ref<TYPE>& that) const
{
	return ptr == that.ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator ==(TYPE* that_ptr) const
{
	return ptr == that_ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator !=(const Ref<TYPE>& that) const
{
	return ptr != that.ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator !=(TYPE* that_ptr) const
{
	return ptr != that_ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator <(const Ref<TYPE>& that) const
{
	return ptr < that.ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator <(TYPE* that_ptr) const
{
	return ptr < that_ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator >(const Ref<TYPE>& that) const
{
	return ptr > that.ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator >(TYPE* that_ptr) const
{
	return ptr > that_ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator <=(const Ref<TYPE>& that) const
{
	return ptr <= that.ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator <=(TYPE* that_ptr) const
{
	return ptr <= that_ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator >=(const Ref<TYPE>& that) const
{
	return ptr >= that.ptr;
}

template <class TYPE>
bool Ref<TYPE>::operator >=(TYPE* that_ptr) const
{
	return ptr >= that_ptr;
}

template <class TYPE>
void Ref<TYPE>::Assign(TYPE* that_ptr)
{
	if (that_ptr != NULL)
		that_ptr->add_ref();

	TYPE* old_ptr = ptr;

	ptr = that_ptr;

	if (old_ptr != NULL)
		old_ptr->release();
}

template <class TYPE>
Ref<TYPE>& Ref<TYPE>::operator =(const Ref<TYPE>& that)
{
	Assign(that.ptr);

	return *this;
}

template <class TYPE>
Ref<TYPE>& Ref<TYPE>::operator =(TYPE* that_ptr)
{
	Assign(that_ptr);

	return *this;
}

template <class TYPE>
void Ref<TYPE>::Attach(TYPE* that_ptr)
{
	if (ptr != NULL)
		ptr->release();

	ptr = that_ptr;
}

template <class TYPE>
TYPE* Ref<TYPE>::Detach()
{
	TYPE* this_ptr = ptr;

	ptr = NULL;
	return this_ptr;
}

template <class TYPE>
void Ref<TYPE>::swap(Ref<TYPE>& that)
{
	TYPE* this_ptr = ptr;

	ptr = that.ptr;
	that.ptr = this_ptr;
}

template <class TYPE>
Ref<TYPE>::~Ref()
{
	if (ptr != NULL)
		ptr->release();
}

B_END_NAMESPACE

#endif /* !defined(B_REF_H) */
