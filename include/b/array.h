/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

#ifndef B_ARRAY_H
#define B_ARRAY_H

#include "array_slice.h"
#include "fn.h"

B_BEGIN_NAMESPACE

template <class T>
class array_slice;

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
	array(size_t count, const T& element);

// Array Length
public:
	// Returns the number of elements for which memory has
	// been allocated.
	size_t capacity() const;

	// Returns the number of elements in the array.
	size_t length() const;

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
	void trim_to_size();

// Buffer Access
public:
	// Returns a constant pointer to the elements.
	const T* data() const;

	// Fixes the buffer in memory and returns a modifiable pointer to it.
	T* lock();

	// Gives control over the buffer back to this object.
	void unlock();

// Element Access
public:
	// Returns a constant reference to an element.
	const T& at(size_t index) const;

	// Returns a constant reference to an element (operator version
	// of the respective at() method).
	const T& operator [](size_t index) const;

	// Returns a modifiable reference to an element.
	T& at(size_t index);

	// Returns a modifiable reference to an element (operator version
	// of the respective at() method).
	T& operator [](size_t index);

	// Returns a constant reference to the first array element.
	const T& first() const;

	// Returns a modifiable reference to the first array element.
	T& first();

	// Returns a constant reference to the last array element.
	const T& last() const;

	// Returns a modifiable reference to the last array element.
	T& last();

	// Returns a slice of this array with element indices in
	// the specified range.
	array_slice<T> slice(size_t from, size_t slice_len) const;

// Assignment
public:
	// Assigns the contents of 'source' to this object.
	void assign(const array<T>& source);

	// Assigns a sequence of elements to this object.
	void assign(const T* source, size_t count);

	// Creates an array with 'count' copies of 'element'.
	void assign(size_t count, const T& element);

	// A shorter version of assign(source).
	array<T>& operator =(const array<T>& source);

	// Overwrites array range with the contents of 'source'.
	void overwrite(size_t index, const array<T>& source);

	// Overwrites array range with copies of elements
	// pointed to by 'source'.
	void overwrite(size_t index, const T* source, size_t count);

	// Overwrites array range with 'count' copies of 'element'.
	void overwrite(size_t index, size_t count, const T& element);

// Insertion
public:
	// Inserts the entire contents of 'source' into this array
	// at the specified position.
	void insert(size_t index, const array<T>& source);

	// Inserts elements pointed to by 'source' at the specified position.
	void insert(size_t index, const T* source, size_t count);

	// Inserts 'count' copies of 'element' at the specified position.
	void insert(size_t index, size_t count, const T& element);

// Appending
public:
	// Extends this array with copies of elements from 'source'.
	void append(const array<T>& source);

	// Extends this array with elements from 'source'.
	void append(const T* source, size_t count);

	// Appends 'count' copies of 'element' at the end of this array.
	void append(size_t count, const T& element);

	// Appends a single element to this array.
	void append(const T& element);

	// Extends this array with copies of elements from 'source'
	// (operator version of the respective append() method).
	array<T>& operator +=(const array<T>& source);

	// Appends a single element to this array (operator version
	// of the respective append() method).
	array<T>& operator +=(const T& element);

	// Concatenates this array with 'source' and returns the
	// result as a new array object.
	array<T> operator +(const array<T>& source) const;

// Removal
public:
	// Removes a number of elements at the specified position.
	void remove(size_t index, size_t count = 1);

	// Empties the array.
	void empty();

// Range-based 'for' Loop Compatibility
public:
	typedef const T* const_iterator;

	// Returns a pointer to the first element if the array
	// is not empty.
	// Returns an invalid pointer that is equal to end()
	// if the array is empty.
	const T* begin() const;

	// Returns a pointer past the last element in the array.
	const T* end() const;

// Utility methods
public:
	// Shuffle the array using the specified pseudorandom
	// number generator.
	void shuffle(pseudorandom& prng);

// Implementation
private:
	struct array_metadata
	{
		int refs;
		size_t capacity;
		size_t length;
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

	static T* alloc_buffer(size_t capacity, size_t length);

	static buffer* metadata(const T* elements);
	buffer* metadata() const;

	void release();

	void replace_buffer(T* new_buffer_elements);

	// Make sure that the buffer is not shared with other
	// array objects. Reallocate the buffer if it's shared;
	// keep the original contents.
	void isolate();

public:
	~array();
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
array<T>::array(size_t count, const T& element) : elements(empty_array())
{
	assign(count, element);
}

template <class T>
size_t array<T>::capacity() const
{
	return metadata()->capacity;
}

template <class T>
size_t array<T>::length() const
{
	return metadata()->length;
}

template <class T>
bool array<T>::is_empty() const
{
	return length() == 0;
}

template <class T>
bool array<T>::is_locked() const
{
	return metadata()->refs <= 0;
}

template <class T>
void array<T>::discard_and_alloc(size_t new_capacity)
{
	B_ASSERT(!is_locked());

	if (capacity() != new_capacity || is_shared())
	{
		release();

		elements = new_capacity > 0 ?
			alloc_buffer(new_capacity, 0) : empty_array();
	}
	else
	{
		// It makes no sense to allocate memory block
		// of the same size. However, all elements are
		// destructed to simulate usual behavior.
		destruct(elements, length());
		metadata()->length = 0;
	}
}

template <class T>
void array<T>::alloc_and_copy(size_t new_capacity)
{
	B_ASSERT(!is_locked());

	// Even if the array already has the capacity requested,
	// if the buffer is shared, it must be reallocated.
	if (capacity() != new_capacity || is_shared())
	{
		if (new_capacity > 0)
		{
			size_t new_size = length() < new_capacity ?
				length() : new_capacity;

			T* new_buffer_elements =
				alloc_buffer(new_capacity, new_size);

			construct_copies(new_buffer_elements,
				elements, new_size);

			replace_buffer(new_buffer_elements);
		}
		else
			replace_buffer(empty_array());
	}
}

template <class T>
void array<T>::trim_to_size()
{
	alloc_and_copy(length());
}

template <class T>
const T* array<T>::data() const
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
	B_ASSERT(index < length());

	return elements[index];
}

template <class T>
const T& array<T>::operator [](size_t index) const
{
	B_ASSERT(index < length());

	return elements[index];
}

template <class T>
T& array<T>::at(size_t index)
{
	B_ASSERT(index < length());

	isolate();
	return elements[index];
}

template <class T>
T& array<T>::operator [](size_t index)
{
	B_ASSERT(index < length());

	isolate();
	return elements[index];
}

template <class T>
const T& array<T>::first() const
{
	B_ASSERT(!is_empty());

	return *elements;
}

template <class T>
T& array<T>::first()
{
	B_ASSERT(!is_empty());

	isolate();
	return *elements;
}

template <class T>
const T& array<T>::last() const
{
	B_ASSERT(!is_empty());

	return elements[length() - 1];
}

template <class T>
T& array<T>::last()
{
	B_ASSERT(!is_empty());

	isolate();
	return elements[length() - 1];
}

template <class T>
array_slice<T> array<T>::slice(size_t from, size_t slice_len) const
{
	B_ASSERT(from + slice_len <= length());

	return array_slice<T>(elements + from, slice_len);
}

template <class T>
void array<T>::assign(const array<T>& source)
{
	if (!is_locked() && !source.is_locked())
	{
		if (source.elements != empty_array())
			++source.metadata()->refs;

		replace_buffer(source.elements);
	}
	else
		if (elements != source.elements)
			assign(source.data(), source.length());
}

template <class T>
void array<T>::assign(const T* source, size_t count)
{
	// This and the source array must not overlap.
	B_ASSERT(source >= elements + capacity() ||
		source + count < elements);

	if (count > 0)
	{
		if (!is_shared() && count <= capacity())
			if (count > length())
			{
				assign_pairwise(elements, source, length());
				construct_copies(elements + length(),
					source + length(),
					count - length());
			}
			else
			{
				assign_pairwise(elements, source, count);
				destruct(elements + count, length() - count);
			}
		else
		{
			discard_and_alloc(extra_capacity(count));
			construct_copies(elements, source, count);
		}

		metadata()->length = count;
	}
	else
		empty();
}

template <class T>
void array<T>::assign(size_t count, const T& element)
{
	if (count > 0)
	{
		if (!is_shared() && count <= capacity())
			if (count > length())
			{
				assign_value(elements, length(), element);
				construct_identical_copies(elements + length(),
					element, count - length());
			}
			else
			{
				assign_value(elements, count, element);
				destruct(elements + count, length() - count);
			}
		else
		{
			discard_and_alloc(extra_capacity(count));
			construct_identical_copies(elements, element, count);
		}

		metadata()->length = count;
	}
	else
		empty();
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
	overwrite(index, source.data(), source.length());
}

template <class T>
void array<T>::overwrite(size_t index, const T* source, size_t count)
{
	B_ASSERT(index <= length());
	// This and the source array must not overlap.
	B_ASSERT(source >= elements + capacity() ||
		source + count < elements);

	if (count > 0)
	{
		size_t tail_index = index + count;

		if (!is_shared())
		{
			if (tail_index <= length())
			{
				// This is the most frequent case,
				// that's why it is processed first.

				assign_pairwise(elements + index,
					source, count);

				return;
			}
			else
				if (tail_index <= capacity())
				{
					assign_pairwise(elements + index,
						source, length() - index);
					construct_copies(elements + length(),
						source + length() - index,
						tail_index - length());

					metadata()->length = tail_index;

					return;
				}
		}

		// Either the buffer is shared among instances or
		// index of the last element exceeds array boundaries.

		// The array must not be locked, because its buffer
		// is to be reallocated.
		B_ASSERT(!is_locked());

		T* new_buffer_elements;

		if (tail_index < length())
		{
			const size_t unaffected_size = length();

			new_buffer_elements = alloc_buffer(extra_capacity(
				unaffected_size), unaffected_size);

			construct_copies(new_buffer_elements + tail_index,
				elements + tail_index,
				unaffected_size - tail_index);
		}
		else
			new_buffer_elements = alloc_buffer(extra_capacity(
				tail_index), tail_index);

		construct_copies(new_buffer_elements, elements, index);
		construct_copies(new_buffer_elements + index, source, count);

		replace_buffer(new_buffer_elements);
	}
}

template <class T>
void array<T>::overwrite(size_t index, size_t count, const T& element)
{
	B_ASSERT(index <= length());

	if (count > 0)
	{
		size_t tail_index = index + count;

		if (!is_shared())
		{
			if (tail_index <= length())
			{
				assign_value(elements + index, count, element);

				return;
			}
			else
				if (tail_index <= capacity())
				{
					assign_value(elements + index,
						length() - index, element);

					construct_identical_copies(
						elements + length(), element,
						tail_index - length());

					metadata()->length = tail_index;

					return;
				}
		}

		B_ASSERT(!is_locked());

		T* new_buffer_elements;

		if (tail_index < length())
		{
			const size_t unaffected_size = length();

			new_buffer_elements = alloc_buffer(extra_capacity(
				unaffected_size), unaffected_size);

			construct_copies(new_buffer_elements + tail_index,
				elements + tail_index,
				unaffected_size - tail_index);
		}
		else
			new_buffer_elements = alloc_buffer(extra_capacity(
				tail_index), tail_index);

		construct_copies(new_buffer_elements, elements, index);
		construct_identical_copies(new_buffer_elements + index,
			element, count);

		replace_buffer(new_buffer_elements);
	}
}

template <class T>
void array<T>::insert(size_t index, const array<T>& source)
{
	insert(index, source.data(), source.length());
}

template <class T>
void array<T>::insert(size_t index, const T* source, size_t count)
{
	B_ASSERT(index <= length());
	// This and the source array must not overlap.
	B_ASSERT(source >= elements + capacity() ||
		source + count < elements);

	if (count > 0)
	{
		T* tail = elements + index;
		size_t tail_size = length() - index;
		size_t new_size = length() + count;

		if (new_size > capacity() || is_shared())
		{
			T* new_buffer_elements = alloc_buffer(extra_capacity(
				new_size), new_size);

			construct_copies(new_buffer_elements, elements, index);
			construct_copies(new_buffer_elements + index,
				source, count);
			construct_copies(new_buffer_elements + index + count,
				tail, tail_size);

			replace_buffer(new_buffer_elements);
		}
		else
		{
			move_right_and_insert(tail, tail_size, source, count);

			metadata()->length = new_size;
		}
	}
}

template <class T>
void array<T>::insert(size_t index, size_t count, const T& element)
{
	B_ASSERT(index <= length());

	if (count > 0)
	{
		T* tail = elements + index;
		size_t tail_size = length() - index;
		size_t new_size = length() + count;

		if (new_size > capacity() || is_shared())
		{
			T* new_buffer_elements = alloc_buffer(extra_capacity(
				new_size), new_size);

			construct_copies(new_buffer_elements, elements, index);
			construct_identical_copies(new_buffer_elements + index,
				element, count);
			construct_copies(new_buffer_elements + index + count,
				tail, tail_size);

			replace_buffer(new_buffer_elements);
		}
		else
		{
			move_right_and_insert(tail, tail_size, element, count);

			metadata()->length = new_size;
		}
	}
}

template <class T>
void array<T>::append(const array<T>& source)
{
	append(source.elements, source.length());
}

template <class T>
void array<T>::append(const T* source, size_t count)
{
	// This and the source array must not overlap.
	B_ASSERT(source >= elements + capacity() ||
		source + count < elements);

	if (count > 0)
	{
		if (is_shared() || length() + count > capacity())
			alloc_and_copy(extra_capacity(length() + count));

		construct_copies(elements + length(), source, count);
		metadata()->length += count;
	}
}

template <class T>
void array<T>::append(size_t count, const T& element)
{
	if (count > 0)
	{
		if (is_shared() || length() + count > capacity())
			alloc_and_copy(extra_capacity(length() + count));

		construct_identical_copies(elements + length(), element, count);
		metadata()->length += count;
	}
}

template <class T>
void array<T>::append(const T& element)
{
	append(1, element);
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
	append(1, element);
	return *this;
}

template <class T>
array<T> array<T>::operator +(const array<T>& source) const
{
	array<T> result(*this);
	return result += source;
}

template <class T>
void array<T>::remove(size_t index, size_t count)
{
	B_ASSERT(index <= length());

	if (index + count > length())
		count = length() - index;

	if (count > 0)
	{
		size_t new_size = length() - count;

		if (!is_shared())
		{
			move_left(elements + index,
				elements + index + count, new_size - index);

			destruct(elements + new_size, count);

			metadata()->length = new_size;
		}
		else
		{
			T* new_buffer_elements = alloc_buffer(extra_capacity(
				new_size), new_size);

			construct_copies(new_buffer_elements, elements, index);

			construct_copies(new_buffer_elements + index,
				elements + index + count, new_size - index);

			replace_buffer(new_buffer_elements);
		}
	}
}

template <class T>
void array<T>::empty()
{
	if (!is_shared())
	{
		destruct(elements, length());
		metadata()->length = 0;
	}
	else
		replace_buffer(empty_array());
}

template <class T>
const T* array<T>::begin() const
{
	return elements;
}

template <class T>
const T* array<T>::end() const
{
	return elements + length();
}

template <class T>
void array<T>::shuffle(pseudorandom& prng)
{
	isolate();

	shuffle_array(elements, length(), prng);
}

template <class T>
bool array<T>::is_shared() const
{
	return metadata()->refs > 1;
}

template <class T>
T* array<T>::empty_array()
{
	static const array_metadata empty_array_metadata =
	{
		/* refs         */ 2,
		/* capacity     */ 0,
		/* length       */ 0
	};

	return &static_cast<buffer&>(
		const_cast<array_metadata&>(
			empty_array_metadata)).first_element;
}

template <class T>
T* array<T>::alloc_buffer(size_t capacity, size_t length)
{
	buffer* new_buffer = (buffer*) memory::alloc((size_t)
		&((buffer*) (sizeof(T) * capacity))->first_element);

	new_buffer->refs = 1;
	new_buffer->capacity = capacity;
	new_buffer->length = length;

	return &new_buffer->first_element;
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
void array<T>::release()
{
	B_ASSERT(!is_locked());

	if (elements != empty_array() && !--metadata()->refs)
	{
		destruct(elements, length());
		memory::free(metadata());
	}
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
		alloc_and_copy(extra_capacity(length()));
}

template <class T>
array<T>::~array()
{
	release();
}

B_END_NAMESPACE

#endif /* !defined(B_ARRAY_H) */
