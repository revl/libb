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

#include "ref_count.h"
#include "misc.h"

#ifndef B_ARRAY_DEFINED
#define B_ARRAY_DEFINED

B_BEGIN_NAMESPACE

// Linear array template class.
template <class T>
class array
{
// Construction
public:
	// Creates an empty array.
	array();

	// Constructs a copy of an existing array object.
	array(const array<T>& source);

	// Constructs an array from a sequence of objects.
	array(const T* source, size_t count);

	// Creates an array of 'count' copies of 'element'.
	array(const T& element, size_t count = 1);

// Array Length
public:
	// Returns the number of elements for which memory has
	// been allocated.
	size_t capacity() const;

	// Returns the number of elements in the array.
	size_t size() const;

	// Checks if the array is empty.
	bool is_empty() const;

// Lock State
public:
	// Checks if the array is locked.
	bool is_locked() const;

// Memory Management
public:
	// Allocates the exact amount of memory for the buffer.
	void discard_and_alloc(size_t new_capacity);

	// Reallocates the buffer preserving its contents.
	void alloc_and_copy(size_t new_capacity);

	// Frees memory that is not occupied by the array elements.
	void shrink_to_fit();

// Buffer Access
public:
	// Returns a constant pointer to the elements.
	const T* data() const;

	// Returns 'data()'.
	operator const T*() const;

	// Fixes the buffer in memory and returns a modifiable pointer to it.
	T* lock();

	// Gives control over the buffer back to this object.
	void unlock();

// Element Access
public:
	// Returns a constant reference to an element.
	const T& at(size_t index) const;

	// Operator version of 'at(index)'.
	const T& operator [](size_t index) const;

	// Returns a modifiable reference to an element.
	T& at(size_t index);

	// Operator version of 'at(index)'.
	T& operator [](size_t index);

	// Returns a constant reference to the first array element.
	const T& front() const;

	// Returns a modifiable reference to the first array element.
	T& front();

	// Returns a constant reference to the last array element.
	const T& back() const;

	// Returns a modifiable reference to the last array element.
	T& back();

// Assignment
public:
	// Assigns the contents of 'source' to this object.
	void assign(const array<T>& source);

	// Assigns a sequence of elements to this object.
	void assign(const T* source, size_t count);

	// Creates an array with 'count' copies of 'element'.
	void assign(const T& element, size_t count);

	// A shorter version of assign(source).
	array<T>& operator =(const array<T>& source);

	// Overwrites array range with the contents of 'source'.
	void overwrite(size_t index, const array<T>& source);

	// Overwrites array range with copies of elements
	// pointed to by 'source'.
	void overwrite(size_t index, const T* source, size_t count);

	// Overwrites array range with 'count' copies of 'element'.
	void overwrite(size_t index, const T& element, size_t count = 1);

// Insertion
public:
	// Inserts the entire contents of 'source' into this array
	// at the specified position.
	void insert(size_t index, const array<T>& source);

	// Inserts elements pointed to by 'source' at the specified position.
	void insert(size_t index, const T* source, size_t count);

	// Inserts 'count' copies of 'element' at the specified position.
	void insert(size_t index, const T& element, size_t count = 1);

// Appending
public:
	// Extends this array with copies of elements from 'source'.
	void append(const array<T>& source);

	// Extends this array with elements from 'source'.
	void append(const T* source, size_t count);

	// Appends 'count' copies of 'element' at the end of this array.
	void append(const T& element, size_t count = 1);

	// Operator version of 'append(source)'.
	array<T>& operator +=(const array<T>& source);

	// Operator version of 'append(element, 1)'.
	array<T>& operator +=(const T& element);

	// Concatenates this array with 'source' and returns the
	// result as a new array object.
	array<T> operator +(const array<T>& source) const;

// Removal
public:
	// Removes a number of elements at the specified position.
	void erase(size_t index, size_t count = 1);

	// Empties the array.
	void clear();

// Implementation
private:
	struct array_metadata
	{
		RefCount refs;
		size_t capacity;
		size_t size;
	};

	struct buffer : public array_metadata
	{
		T first_element;

		// The constructor is never used, because this structure
		// is never constructed explicitly.
		explicit buffer(const T& value) : first_element(value)
		{
		}
	};

	T* elements;

	bool is_shared() const;

	static T* empty_array();

	static T* alloc_buffer(size_t capacity, size_t size);

	static buffer* metadata(const T* elements);
	buffer* metadata() const;

	void release();

	void replace_buffer(T* new_buffer_elements);

	// Make sure that the buffer is not shared with other
	// array objects. Reallocate the buffer if it's shared;
	// keep the original contents.
	void isolate();

public:
	~array()
		throw ();
};

template <class T>
array<T>::array() : elements(empty_array())
{
}

template <class T>
array<T>::array(const array<T>& source) : elements(empty_array())
{
	assign(source);
}

template <class T>
array<T>::array(const T* source, size_t count) : elements(empty_array())
{
	assign(source, count);
}

template <class T>
array<T>::array(const T& element, size_t count) : elements(empty_array())
{
	assign(element, count);
}

template <class T>
size_t array<T>::capacity() const
{
	return metadata()->capacity;
}

template <class T>
size_t array<T>::size() const
{
	return metadata()->size;
}

template <class T>
bool array<T>::is_empty() const
{
	return size() == 0;
}

template <class T>
bool array<T>::is_locked() const
{
	return metadata()->refs < 0;
}

template <class T>
void array<T>::shrink_to_fit()
{
	alloc_and_copy(size());
}

template <class T>
const T* array<T>::data() const
{
	return elements;
}

template <class T>
array<T>::operator const T*() const
{
	return elements;
}

template <class T>
T* array<T>::lock()
{
	isolate();

	--metadata()->refs;

	return elements;
}

template <class T>
void array<T>::unlock()
{
	B_ASSERT(is_locked());

	++metadata()->refs;
}

template <class T>
const T& array<T>::at(size_t index) const
{
	B_ASSERT(index < size());

	return elements[index];
}

template <class T>
const T& array<T>::operator [](size_t index) const
{
	B_ASSERT(index < size());

	return elements[index];
}

template <class T>
T& array<T>::at(size_t index)
{
	B_ASSERT(index < size());

	isolate();
	return elements[index];
}

template <class T>
T& array<T>::operator [](size_t index)
{
	B_ASSERT(index < size());

	isolate();
	return elements[index];
}

template <class T>
const T& array<T>::front() const
{
	B_ASSERT(!is_empty());

	return *elements;
}

template <class T>
T& array<T>::front()
{
	B_ASSERT(!is_empty());

	isolate();
	return *elements;
}

template <class T>
const T& array<T>::back() const
{
	B_ASSERT(!is_empty());

	return elements[size() - 1];
}

template <class T>
T& array<T>::back()
{
	B_ASSERT(!is_empty());

	isolate();
	return elements[size() - 1];
}

template <class T>
array<T>& array<T>::operator =(const array<T>& source)
{
	assign(source);
	return *this;
}

template <class T>
void array<T>::overwrite(size_t index, const array<T>& source)
{
	overwrite(index, source.data(), source.size());
}

template <class T>
void array<T>::insert(size_t index, const array<T>& source)
{
	insert(index, source.data(), source.size());
}

template <class T>
void array<T>::append(const array<T>& source)
{
	append(source.elements, source.size());
}

template <class T>
array<T>& array<T>::operator +=(const array<T>& source)
{
	append(source);
	return *this;
}

template <class T>
array<T>& array<T>::operator +=(const T& element)
{
	append(element);
	return *this;
}

template <class T>
array<T> array<T>::operator +(const array<T>& source) const
{
	array<T> result(*this);
	return result += source;
}

template <class T>
bool array<T>::is_shared() const
{
	return metadata()->refs > 0;
}

template <class T>
typename array<T>::buffer* array<T>::metadata(const T* data)
{
	return B_OUTERSTRUCT(buffer, first_element, data);
}

template <class T>
typename array<T>::buffer* array<T>::metadata() const
{
	return metadata(elements);
}

template <class T>
void array<T>::replace_buffer(T* new_buffer_elements)
{
	release();

	elements = new_buffer_elements;
}

template <class T>
void array<T>::isolate()
{
	if (is_shared())
		alloc_and_copy(extra_capacity(size()));
}

template <class T>
array<T>::~array()
	throw ()
{
	release();
}

B_END_NAMESPACE

#endif // B_ARRAY_DEFINED

#include "array_impl.h"
