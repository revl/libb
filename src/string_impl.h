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

B_BEGIN_NAMESPACE

void string_t::discard_and_alloc(size_t new_capacity)
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

void string_t::alloc_and_copy(size_t new_capacity)
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

void string_t::assign(const string_t& source)
{
	if (!is_locked() && !source.is_locked())
	{
		if (source.chars != empty_string())
			++source.metadata()->refs;

		replace_buffer(source.chars);
	}
	else
		if (chars != source.chars)
			assign(source.c_str(), source.length());
}

void string_t::assign(const char_t* source, size_t count)
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
		clear();
}

void string_t::assign(char_t source, size_t count)
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
		clear();
}

/*
void string_t::assign(const char_t* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		assign_pairwise(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		clear();
}

void string_t::assign(char_t source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		assign_value(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		clear();
}

void string_t::replace(size_t index, const char_t* source, size_t count)
{
	B_ASSERT(index >= 0 && index <= length() && count >= 0);

	size_t unaffected = index + count;

	assign_pairwise((unaffected <= length() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}

void string_t::replace(size_t index, char_t source, size_t count)
{
	B_ASSERT(index >= 0 && index <= length() && count >= 0);

	size_t unaffected = index + count;

	assign_value((unaffected <= length() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}
*/

void string_t::insert(size_t index, const char_t* source, size_t count)
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

void string_t::insert(size_t index, char_t source, size_t count)
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

void string_t::append(const char_t* source, size_t count)
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

void string_t::append(char_t source, size_t count)
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
void string_t::append(const char_t* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(length() + count);
		assign_pairwise(chars + length(), source, count);
		Unlock();
	}
}

void string_t::append(char_t source, size_t count)
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

string_t string_t::operator +(const string_t& source) const
{
	string_t result(*this);
	return result += source;
}

void string_t::erase(size_t index, size_t count)
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

void string_t::clear()
{
	if (!is_shared())
	{
		metadata()->length = 0;
		*chars = 0;
	}
	else
		replace_buffer(empty_string());
}

void string_t::format(const char_t* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	format(fmt, args);
	va_end(args);
}

void string_t::format(const char_t* fmt, va_list args)
{
	clear();
	append_format(fmt, args);
}

void string_t::append_format(const char_t* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	append_format(fmt, args);
	va_end(args);
}

void string_t::append_format(const char_t* fmt, va_list args)
{
	alloc_and_copy(length() + 8 * 1024); // TODO reserve() if it preserves?

	metadata()->length += format_string(chars + length(), fmt, args);
}

size_t string_t::find(char_t c) const
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

size_t string_t::rfind(char_t c) const
{
	size_t index = length();

	const char_t* pos = chars + index;

	while (index-- > 0)
		if (*--pos == c)
			return (size_t) (pos - chars);

	return (size_t) -1;
}

void string_t::trim_right(const char_t* samples)
{
	char_t* end = chars + length();

	while (--end >= chars && find_char(samples, *end) != NULL)
		;

	size_t new_length = ++end - chars;

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

void string_t::trim_left(const char_t* samples)
{
	char_t* start = chars;

	while (*start && find_char(samples, *start) != NULL)
		++start;

	if (start > chars)
	{
		size_t new_length = chars + length() - start;

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

char_t* string_t::empty_string()
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

char_t* string_t::alloc_buffer(size_t capacity, size_t length)
{
	B_ASSERT(capacity >= length);

	buffer* new_buffer = (buffer*) Memory::Alloc(sizeof(buffer) +
		capacity * sizeof(char_t));

	new_buffer->refs = 1;
	new_buffer->capacity = capacity;
	new_buffer->length = length;

	return new_buffer->first_char;
}

void string_t::replace_buffer(char_t* new_buffer_chars)
{
	B_ASSERT(!is_locked());

	if (chars != empty_string() && !--metadata()->refs)
		Memory::Free(metadata());

	chars = new_buffer_chars;
}

string_t::~string_t()
	throw ()
{
	if (chars != empty_string() && (is_locked() || !--metadata()->refs))
		Memory::Free(metadata());
}

char_t* find_char(const char_t* string, char_t c)
{
	while (*string != 0)
		if (*string++ == c)
			return const_cast<char_t*>(string);

	return NULL;
}

char_t* find_char_backwards(const char_t* string, char_t c)
{
	size_t index = calc_length(string);

	string += index;

	while (index-- > 0)
		if (*--string == c)
			return const_cast<char_t*>(string);

	return NULL;
}

B_END_NAMESPACE
