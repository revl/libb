/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
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

B_BEGIN_NAMESPACE

string::string(const char_t* source, size_t count, size_t times)
{
	size_t total_count = count * times;

	if (total_count == 0)
		chars = empty_string();
	else
	{
		chars = alloc_buffer(extra_capacity(total_count), total_count);

		if (count == 1)
			assign_value(chars, times, *source);
		else
		{
			char_t* dest = chars;

			do
			{
				assign_pairwise(dest, source, count);
				dest += count;
			}
			while (--times > 0);
		}

		chars[total_count] = 0;
	}
}

void string::discard_and_alloc(size_t new_capacity)
{
	B_ASSERT(!is_locked());

	if (capacity() != new_capacity || is_shared())
	{
		// Release previous buffer before allocating a new one.
		replace_buffer(empty_string());

		if (new_capacity > 0)
			// No need to call replace_buffer() again, as in:
			// "replace_buffer(alloc_buffer(new_capacity, 0));"
			// It does the same thing as the next line:
			chars = alloc_buffer(new_capacity, 0);
	}
	else
		metadata()->length = 0;
	*chars = 0;
}

void string::alloc_and_copy(size_t new_capacity)
{
	B_ASSERT(!is_locked());

	// Even if the array already has the capacity requested,
	// if the buffer is shared, it must be reallocated.
	// This behavior is used by other functions.
	if (capacity() != new_capacity || is_shared())
	{
		if (new_capacity > 0)
		{
			size_t len = length() < new_capacity ?
				length() : new_capacity;

			char_t* new_buffer_chars =
				alloc_buffer(new_capacity, len);

			assign_pairwise(new_buffer_chars, chars, len + 1);

			replace_buffer(new_buffer_chars);
		}
		else
			replace_buffer(empty_string());
	}
}

void string::assign(const string& source)
{
	if (!is_locked() && !source.is_locked())
	{
		if (source.chars != empty_string())
			++source.metadata()->refs;

		replace_buffer(source.chars);
	}
	else
		if (chars != source.chars)
			assign(source.data(), source.length());
}

void string::assign(const char_t* source, size_t count)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count > capacity())
			discard_and_alloc(extra_capacity(count));
		assign_pairwise(chars, source, count);
		chars[metadata()->length = count] = 0;
	}
	else
		empty();
}

void string::assign(size_t count, char_t source)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count > capacity())
			discard_and_alloc(extra_capacity(count));
		assign_value(chars, count, source);
		chars[metadata()->length = count] = 0;
	}
	else
		empty();
}

/*
void string::assign(const char_t* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		assign_pairwise(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		empty();
}

void string::assign(char_t source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		assign_value(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		empty();
}

void string::replace(size_t index, const char_t* source, size_t count)
{
	B_ASSERT(index >= 0 && index <= length() && count >= 0);

	size_t unaffected = index + count;

	assign_pairwise((unaffected <= length() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}

void string::replace(size_t index, char_t source, size_t count)
{
	B_ASSERT(index >= 0 && index <= length() && count >= 0);

	size_t unaffected = index + count;

	assign_value((unaffected <= length() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}
*/

void string::insert(size_t index, const char_t* source, size_t count)
{
	B_ASSERT(index <= length());
	// source must not be a part of this array
	B_ASSERT(source >= chars + capacity() || source + count < chars);

	if (count > 0)
	{
		char_t* tail = chars + index;
		size_t tail_length = length() - index;
		size_t new_length = length() + count;

		if (new_length > capacity() || is_shared())
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, index);
			assign_pairwise(new_buffer_chars + index,
				source, count);
			assign_pairwise(new_buffer_chars + index + count,
				tail, tail_length + 1);

			replace_buffer(new_buffer_chars);
		}
		else
		{
			if (count < tail_length)
			{
				assign_pairwise(tail + tail_length,
					tail + tail_length - count, count);

				assign_pairwise_backwards(tail + count,
					tail, tail_length - count);

				assign_pairwise(tail, source, count);
			}
			else
			{
				assign_pairwise(tail + tail_length,
					source + tail_length,
					count - tail_length);

				assign_pairwise(tail + count,
					tail, tail_length);

				assign_pairwise(tail, source, tail_length);
			}

			metadata()->length = new_length;
		}
	}
}

void string::insert(size_t index, char_t source, size_t count)
{
	B_ASSERT(index <= length());

	if (count > 0)
	{
		char_t* tail = chars + index;
		size_t tail_length = length() - index;
		size_t new_length = length() + count;

		if (new_length > capacity() || is_shared())
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, index);
			assign_value(new_buffer_chars + index,
				count, source);
			assign_pairwise(new_buffer_chars + index + count,
				tail, tail_length + 1);

			replace_buffer(new_buffer_chars);
		}
		else
		{
			if (count < tail_length)
			{
				assign_pairwise(tail + tail_length,
					tail + tail_length - count, count);

				assign_pairwise_backwards(tail + count,
					tail, tail_length - count);

				assign_value(tail, count, source);
			}
			else
			{
				assign_value(tail + tail_length,
					count - tail_length, source);

				assign_pairwise(tail + count,
					tail, tail_length);

				assign_value(tail, tail_length, source);
			}

			metadata()->length = new_length;
		}
	}
}

void string::append(const char_t* source, size_t count)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count + length() > capacity())
			alloc_and_copy(extra_capacity(length() + count));
		assign_pairwise(chars + length(), source, count);
		chars[metadata()->length += count] = 0;
	}
}

void string::append(size_t count, char_t source)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count + length() > capacity())
			alloc_and_copy(extra_capacity(length() + count));
		assign_value(chars + length(), count, source);
		chars[metadata()->length += count] = 0;
	}
}

/*
void string::append(const char_t* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(length() + count);
		assign_pairwise(chars + length(), source, count);
		Unlock();
	}
}

void string::append(char_t source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(length() + count);
		assign_value(chars + length(), source, count);
		UnlockBuffer();
	}
}
*/

string string::operator +(const string& source) const
{
	string result(*this);
	return result += source;
}

void string::remove(size_t index, size_t count)
{
	if (index + count > length())
		count = length() - index;

	if (count > 0)
	{
		size_t new_length = length() - count;

		if (!is_shared())
		{
			assign_pairwise(chars + index, chars + index + count,
				new_length - index + 1);

			metadata()->length = new_length;
		}
		else
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, index);

			assign_pairwise(new_buffer_chars + index,
				chars + index + count, new_length - index + 1);

			replace_buffer(new_buffer_chars);
		}
	}
}

void string::empty()
{
	if (!is_shared())
	{
		metadata()->length = 0;
		*chars = 0;
	}
	else
		replace_buffer(empty_string());
}

string string::formatted(const char_t* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	return formatted_va(fmt, ap);
	va_end(ap);
}

string string::formatted_va(const char_t* fmt, va_list ap)
{
	string result;

	result.format_va(fmt, ap);

	return result;
}

void string::format(const char_t* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	format_va(fmt, ap);
	va_end(ap);
}

void string::format_va(const char_t* fmt, va_list ap)
{
	empty();
	append_format_va(fmt, ap);
}

void string::append_format(const char_t* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	append_format_va(fmt, ap);
	va_end(ap);
}

class string_allocator : public allocator
{
public:
	string_allocator(string* d) : dest(d), allocated(false)
	{
	}

	virtual void* allocate(size_t size);

	virtual ~string_allocator();

private:
	string* dest;
	size_t new_length;
	bool allocated;
};

void* string_allocator::allocate(size_t size)
{
	// TODO reserve() if it preserves?
	new_length = dest->length() + size / sizeof(char_t);

	dest->alloc_and_copy(extra_capacity(new_length));
	allocated = true;

	return dest->lock() + dest->length();
}

string_allocator::~string_allocator()
{
	if (allocated)
		dest->unlock(new_length);
}

void string::append_format_va(const char_t* fmt, va_list ap)
{
	string_allocator str_alloc(this);

	format_buffer_va(&str_alloc, fmt, ap);
}

size_t string::find(char_t c) const
{
	size_t counter = length();

	const char_t* pos = chars;

	while (counter-- > 0)
	{
		if (*pos == c)
			return (size_t) (pos - chars);

		++pos;
	}

	return (size_t) -1;
}

size_t string::rfind(char_t c) const
{
	size_t index = length();

	const char_t* pos = chars + index;

	while (index-- > 0)
		if (*--pos == c)
			return (size_t) (pos - chars);

	return (size_t) -1;
}

void string::trim_right(const char_t* samples)
{
	char_t* end = chars + length();

	while (--end >= chars && find_char(samples, *end) != NULL)
		;

	size_t new_length = (size_t) (++end - chars);

	if (new_length < length())
	{
		if (!is_shared())
			*end = 0;
		else
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, new_length);
			new_buffer_chars[new_length] = 0;

			replace_buffer(new_buffer_chars);
		}

		metadata()->length = new_length;
	}
}

void string::trim_left(const char_t* samples)
{
	const char_t* start = chars;

	while (*start && find_char(samples, *start) != NULL)
		++start;

	if (start > chars)
	{
		size_t new_length = (size_t) (chars + length() - start);

		if (!is_shared())
			assign_pairwise_backwards(chars, start, new_length + 1);
		else
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, start, new_length);
			new_buffer_chars[new_length] = 0;

			replace_buffer(new_buffer_chars);
		}

		metadata()->length = new_length;
	}
}

char_t* string::empty_string()
{
	static const buffer empty_string_buffer =
	{
		/* refs         */ B_REFCOUNT_STATIC_INIT(2),
		/* capacity     */ 0,
		/* length       */ 0,
		/* first_char   */ {0}
	};

	return const_cast<char_t*>(empty_string_buffer.first_char);
}

char_t* string::alloc_buffer(size_t capacity, size_t length)
{
	B_ASSERT(capacity >= length);

	buffer* new_buffer = (buffer*) memory::alloc(sizeof(buffer) +
		capacity * sizeof(char_t));

	new_buffer->refs = 1;
	new_buffer->capacity = capacity;
	new_buffer->length = length;

	return new_buffer->first_char;
}

void string::replace_buffer(char_t* new_buffer_chars)
{
	B_ASSERT(!is_locked());

	if (chars != empty_string() && !--metadata()->refs)
		memory::free(metadata());

	chars = new_buffer_chars;
}

string::~string()
{
	if (chars != empty_string() && (is_locked() || !--metadata()->refs))
		memory::free(metadata());
}

char_t* find_char(const char_t* input, char_t c)
{
	while (*input != 0)
		if (*input++ == c)
			return const_cast<char_t*>(input);

	return NULL;
}

char_t* find_char_backwards(const char_t* input, char_t c)
{
	size_t index = calc_length(input);

	input += index;

	while (index-- > 0)
		if (*--input == c)
			return const_cast<char_t*>(input);

	return NULL;
}

B_END_NAMESPACE
