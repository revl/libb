/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
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

#ifndef B_ARRAY_SLICE_H
#define B_ARRAY_SLICE_H

#include "array.h"
#include "fn.h"

B_BEGIN_NAMESPACE

template <class T>
class array;

// Non-modifiable array of immutable elements. Because 'array_slice' is
// most often used to point to elements of the 'array' template, which
// employs memory management based on reference counting, it is possible
// that the internal buffer of such an array is shared between more than
// one 'array' object.  For this reason, 'array_slice' provides only
// 'const' access to its elements.  And because 'array_slice' does not
// implement any kind of memory management of its own, the calling code
// must ensure that the lifetime of the referred elements exceed the
// lifetime of the 'array_slice' object.
template <class T>
class array_slice
{
// Construction
public:
	// Constructs a new array_slice object.
	array_slice(const T* source, size_t len);

	// Constructs a copy of an existing array_slice object.
	array_slice(const array_slice<T>& source);

	// Constructs an empty array_slice object.
	array_slice();

	// Constructs an array_slice object from an array object.
	array_slice(const array<T>& source);

// Data access
public:
	// Returns a constant pointer to the first slice element.
	const T* data() const
	{
		return elements;
	}

	// Returns the number of elements in this slice.
	size_t length() const
	{
		return slice_length;
	}

	// Returns true if this slice contains no elements.
	bool is_empty() const;

	// Returns a constant reference to the element
	// with the specified index.
	const T& at(size_t index) const;

	// Returns a constant reference to the element
	// with the specified index.
	const T& operator [](size_t index) const;

	// Returns a constant reference to the first element
	// of this array slice.
	const T& first() const;

	// Returns the first 'n' elements of this slice
	// as a new array_slice object.
	array_slice<T> first_n(size_t n) const;

	// Drops the first 'n' elements of this slice and returns
	// the remaining elements as a new array_slice object.
	array_slice<T> without_first_n(size_t n) const;

	// Returns a constant reference to the last element
	// of this array slice.
	const T& last() const;

	// Returns the last 'n' elements of this slice
	// as a new array_slice object.
	array_slice<T> last_n(size_t n) const;

	// Drops the last 'n' elements of this slice and returns
	// the remaining elements as a new array_slice object.
	array_slice<T> without_last_n(size_t n) const;

	// Returns a subslice of this slice.
	array_slice slice(size_t start, size_t len) const;

// Assignment
public:
	// Makes this array slice point to the same
	// memory range that the 'source' slice points to.
	void assign(const array_slice& source);

	// Makes this array slice point to the same
	// memory range that the 'source' slice points to
	// (operator version of assign(const array_slice&)).
	array_slice& operator =(const array_slice& source);

	// Resets this object to point to a different memory range.
	void assign(const T* source, size_t len);

	// Makes this array_slice object point to the entire
	// contents of the 'source' array.
	void assign(const array<T>& source);

	// Makes this array_slice object point to the entire
	// contents of the 'source' array (operator version of
	// assign(const array<T>&)).
	array_slice& operator =(const array<T>& source);

// Implementation
private:
	const T* elements;
	size_t slice_length;
};

template <class T>
array_slice<T>::array_slice(const T* source, size_t len) :
	elements(source), slice_length(len)
{
}

template <class T>
array_slice<T>::array_slice(const array_slice<T>& source) :
	elements(source.data()), slice_length(source.length())
{
}

template <class T>
array_slice<T>::array_slice() : elements(NULL), slice_length(0)
{
}

template <class T>
array_slice<T>::array_slice(const array<T>& source) :
	elements(source.data()), slice_length(source.length())
{
}

template <class T>
bool array_slice<T>::is_empty() const
{
	return length() == 0;
}

template <class T>
const T& array_slice<T>::at(size_t index) const
{
	B_ASSERT(index < length());

	return elements[index];
}

template <class T>
const T& array_slice<T>::operator [](size_t index) const
{
	B_ASSERT(index < length());

	return elements[index];
}

template <class T>
const T& array_slice<T>::first() const
{
	B_ASSERT(!is_empty());

	return *elements;
}

template <class T>
array_slice<T> array_slice<T>::first_n(size_t n) const
{
	B_ASSERT(n <= length());

	return array_slice<T>(elements, n);
}

template <class T>
array_slice<T> array_slice<T>::without_first_n(size_t n) const
{
	B_ASSERT(n <= length());

	return array_slice<T>(elements + n, length() - n);
}

template <class T>
const T& array_slice<T>::last() const
{
	B_ASSERT(!is_empty());

	return elements[length() - 1];
}

template <class T>
array_slice<T> array_slice<T>::last_n(size_t n) const
{
	B_ASSERT(n <= length());

	return array_slice<T>(elements + length() - n, n);
}

template <class T>
array_slice<T> array_slice<T>::without_last_n(size_t n) const
{
	B_ASSERT(n <= length());

	return array_slice<T>(elements, length() - n);
}

template <class T>
array_slice<T> array_slice<T>::slice(size_t start, size_t len) const
{
	B_ASSERT(start <= length());

	return array_slice<T>(elements + start,
		len <= length() - start ? len : length() - start);
}

template <class T>
void array_slice<T>::assign(const array_slice<T>& source)
{
	elements = source.elements;
	slice_length = source.slice_length;
}

template <class T>
array_slice<T>& array_slice<T>::operator =(const array_slice<T>& source)
{
	assign(source);
	return *this;
}

template <class T>
void array_slice<T>::assign(const T* source, size_t len)
{
	elements = source;
	slice_length = len;
}

template <class T>
void array_slice<T>::assign(const array<T>& source)
{
	elements = source.data();
	slice_length = source.length();
}

template <class T>
array_slice<T>& array_slice<T>::operator =(const array<T>& source)
{
	assign(source);
	return *this;
}

B_END_NAMESPACE

#endif /* !defined(B_ARRAY_SLICE_H) */
