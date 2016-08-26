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

#ifndef B_ARRAY_SKIP_IMPLEMENTATION

#ifndef B_ARRAY_IMPLEMENTED
#define B_ARRAY_IMPLEMENTED

#include "array.h"

B_BEGIN_NAMESPACE

template <class T>
void array<T>::discard_and_alloc(size_t new_capacity)
{
	B_ASSERT(!IsLocked());

	if (capacity() != new_capacity || IsShared())
	{
		Release();

		elements = new_capacity > 0 ?
			AllocBufferExactly(new_capacity) :
			empty_array();
	}
	else
	{
		// It makes no sense to allocate memory block
		// of the same size. However, all elements are
		// destroyed to simulate usual behavior.
		Destroy(elements, size());
		metadata()->size = 0;
	}
}

template <class T>
void array<T>::alloc_and_copy(size_t new_capacity)
{
	B_ASSERT(!IsLocked());

	// Even if the array already has the capacity requested,
	// if the buffer is shared, it must be reallocated.
	if (capacity() != new_capacity || IsShared())
	{
		if (new_capacity > 0)
		{
			T* new_buffer_elements = AllocBufferExactly(new_capacity);

			size_t new_array_size =
				size() < new_capacity ? size() : new_capacity;

			Construct(new_buffer_elements, elements,
				metadata(new_buffer_elements)->size = new_array_size);

			ReplaceBuffer(new_buffer_elements);
		}
		else
			ReplaceBuffer(empty_array());
	}
}

template <class T>
void array<T>::Assign(const array<T>& source)
{
	if (!IsLocked() && !source.IsLocked())
	{
		if (source.elements != empty_array())
			++source.metadata()->refs;

		ReplaceBuffer(source.elements);
	}
	else
		if (elements != source.elements)
			Assign(source.data(), source.size());
}

template <class T>
void array<T>::Assign(const T* source, size_t count)
{
	if (count > 0)
	{
		if (!IsShared() && count <= capacity())
			if (count > size())
			{
				Copy(elements, source, size());
				Construct(elements + size(),
					source + size(),
					count - size());
			}
			else
			{
				Copy(elements, source, count);
				Destroy(elements + count,
					size() - count);
			}
		else
		{
			Alloc(count);
			Construct(elements, source, count);
		}

		metadata()->size = count;
	}
	else
		clear();
}

template <class T>
void array<T>::Assign(const T& element, size_t count)
{
	if (count > 0)
	{
		if (!IsShared() && count <= capacity())
			if (count > size())
			{
				Copy(elements, element, size());
				Construct(elements + size(),
					element, count - size());
			}
			else
			{
				Copy(elements, element, count);
				Destroy(elements + count,
					size() - count);
			}
		else
		{
			Alloc(count);
			Construct(elements, element, count);
		}

		metadata()->size = count;
	}
	else
		clear();
}

template <class T>
void array<T>::SetAt(size_t index, const T* source, size_t count)
{
	B_ASSERT(index <= size());
	// The source array cannot be a part of this array
	B_ASSERT(source >= elements + capacity() ||
		source + count < elements);

	if (count > 0)
	{
		size_t tail_index = index + count;

		if (!IsShared())
		{
			if (tail_index <= size())
			{
				// This is the most frequent case,
				// that's why it is processed first.

				Copy(elements + index, source, count);

				return;
			}
			else
				if (tail_index <= capacity())
				{
					Copy(elements + index, source,
						size() - index);
					Construct(elements + size(),
						source + size() - index,
						tail_index - size());

					metadata()->size = tail_index;

					return;
				}
		}

		// Either the buffer is shared among instances or
		// index of the last element exceeds array boundaries.

		// The array must not be locked, because its buffer
		// is to be reallocated.
		B_ASSERT(!IsLocked());

		T* new_buffer_elements;

		if (tail_index < size())
		{
			new_buffer_elements = AllocBuffer(size());

			Construct(new_buffer_elements + tail_index, elements + tail_index,
				(metadata(new_buffer_elements)->size =
					size()) - tail_index);
		}
		else
		{
			new_buffer_elements = AllocBuffer(tail_index);

			metadata(new_buffer_elements)->size = tail_index;
		}

		Construct(new_buffer_elements, elements, index);
		Construct(new_buffer_elements + index, source, count);

		ReplaceBuffer(new_buffer_elements);
	}
}

template <class T>
void array<T>::SetAt(size_t index, const T& element, size_t count)
{
	B_ASSERT(index <= size());

	if (count > 0)
	{
		size_t tail_index = index + count;

		if (!IsShared())
		{
			if (tail_index <= size())
			{
				Copy(elements + index, element, count);

				return;
			}
			else
				if (tail_index <= capacity())
				{
					Copy(elements + index, element,
						size() - index);

					Construct(elements + size(), element,
						tail_index - size());

					metadata()->size = tail_index;

					return;
				}
		}

		B_ASSERT(!IsLocked());

		T* new_buffer_elements;

		if (tail_index < size())
		{
			new_buffer_elements = AllocBuffer(size());

			Construct(new_buffer_elements + tail_index, elements + tail_index,
				(metadata(new_buffer_elements)->size =
					size()) - tail_index);
		}
		else
		{
			new_buffer_elements = AllocBuffer(tail_index);

			metadata(new_buffer_elements)->size = tail_index;
		}

		Construct(new_buffer_elements, elements, index);
		Construct(new_buffer_elements + index, element, count);

		ReplaceBuffer(new_buffer_elements);
	}
}

template <class T>
void array<T>::InsertAt(size_t index, const T* source, size_t count)
{
	B_ASSERT(index <= size());
	// <source> must not be a part of this array
	B_ASSERT(source >= elements + capacity() ||
		source + count < elements);

	if (count > 0)
	{
		T* tail = elements + index;
		size_t tail_size = size() - index;
		size_t new_size = size() + count;

		if (new_size > capacity() || IsShared())
		{
			T* new_buffer_elements = AllocBuffer(new_size);

			Construct(new_buffer_elements, elements, index);
			Construct(new_buffer_elements + index, source, count);
			Construct(new_buffer_elements + index + count,
				tail, tail_size);

			ReplaceBuffer(new_buffer_elements);
		}
		else
			if (count < tail_size)
			{
				Construct(tail + tail_size,
					tail + tail_size - count, count);

				ReverseCopy(tail + count,
					tail, tail_size - count);

				Copy(tail, source, count);
			}
			else
			{
				Construct(tail + tail_size, source + tail_size,
					count - tail_size);

				Construct(tail + count, tail, tail_size);

				Copy(tail, source, tail_size);
			}

		metadata()->size = new_size;
	}
}

template <class T>
void array<T>::InsertAt(size_t index, const T& element, size_t count)
{
	B_ASSERT(index <= size());

	if (count > 0)
	{
		T* tail = elements + index;
		size_t tail_size = size() - index;
		size_t new_size = size() + count;

		if (new_size > capacity() || IsShared())
		{
			T* new_buffer_elements = AllocBuffer(new_size);

			Construct(new_buffer_elements, elements, index);
			Construct(new_buffer_elements + index, element, count);
			Construct(new_buffer_elements + index + count, tail, tail_size);

			ReplaceBuffer(new_buffer_elements);
		}
		else
			if (count < tail_size)
			{
				Construct(tail + tail_size,
					tail + tail_size - count, count);

				ReverseCopy(tail + count,
					tail, tail_size - count);

				Copy(tail, element, count);
			}
			else
			{
				Construct(tail + tail_size,
					element, count - tail_size);

				Construct(tail + count, tail, tail_size);

				Copy(tail, element, tail_size);
			}

		metadata()->size = new_size;
	}
}

template <class T>
void array<T>::Append(const T* source, size_t count)
{
	if (count > 0)
	{
		if (IsShared() || size() + count > capacity())
			Realloc(size() + count);

		Construct(elements + size(), source, count);
		metadata()->size += count;
	}
}

template <class T>
void array<T>::Append(const T& element, size_t count)
{
	if (count > 0)
	{
		if (IsShared() || size() + count > capacity())
			Realloc(size() + count);

		Construct(elements + size(), element, count);
		metadata()->size += count;
	}
}

template <class T>
void array<T>::RemoveAt(size_t index, size_t count)
{
	if (index + count > size())
		count = size() - index;

	if (count > 0)
	{
		size_t new_size = size() - count;

		if (!IsShared())
		{
			Copy(elements + index, elements + index + count,
				new_size - index);

			Destroy(elements + new_size, count);
		}
		else
		{
			T* new_buffer_elements = AllocBuffer(new_size);

			Construct(new_buffer_elements, elements, index);

			Construct(new_buffer_elements + index, elements + index + count,
				new_size - index);

			ReplaceBuffer(new_buffer_elements);
		}

		metadata()->size = new_size;
	}
}

template <class T>
void array<T>::clear()
{
	if (!IsShared())
	{
		Destroy(elements, size());
		metadata()->size = 0;
	}
	else
		ReplaceBuffer(empty_array());
}

template <class T>
T* array<T>::empty_array()
{
	static const array_metadata empty_array_metadata =
	{
		/* refs         */ B_REFCOUNT_STATIC_INIT(2),
		/* capacity     */ 0,
		/* size         */ 0
	};

	return &static_cast<buffer&>(
		const_cast<array_metadata&>(
			empty_array_metadata)).first_element;
}

template <class T>
T* array<T>::AllocBufferExactly(size_t capacity)
{
	buffer* new_buffer = (buffer*) Memory::Alloc((size_t)
		&((buffer*) (sizeof(T) * capacity))->first_element);

	new_buffer->refs = 0;
	new_buffer->capacity = capacity;
	new_buffer->size = 0;

	return &new_buffer->first_element;
}

template <class T>
void array<T>::Release()
{
	B_ASSERT(!IsLocked());

	if (elements != empty_array() && !--metadata()->refs)
	{
		Destroy(elements, size());
		Memory::Free(static_cast<buffer*>(metadata()));
	}
}

B_END_NAMESPACE

#endif // B_ARRAY_IMPLEMENTED

#else
#undef B_ARRAY_SKIP_IMPLEMENTATION
#endif // B_ARRAY_SKIP_IMPLEMENTATION
