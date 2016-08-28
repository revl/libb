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

void string::discard_and_alloc(size_t new_capacity)
{
	B_ASSERT(!is_locked());

	if (capacity() != new_capacity || is_shared())
	{
		// Release previous buffer before allocating a new one.
		ReplaceBuffer(empty_string());

		if (new_capacity > 0)
			// No need to call ReplaceBuffer again:
			// "ReplaceBuffer(AllocBufferExactly(new_capacity, 0));"
			// It does the same thing as the next line:
			chars = AllocBufferExactly(new_capacity, 0);
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

			char* new_buffer_chars =
				AllocBufferExactly(new_capacity, len);

			Copy(new_buffer_chars, chars, len + 1);

			ReplaceBuffer(new_buffer_chars);
		}
		else
			ReplaceBuffer(empty_string());
	}
}

void string::assign(const string& source)
{
	if (!is_locked() && !source.is_locked())
	{
		if (source.chars != empty_string())
			++source.metadata()->refs;

		ReplaceBuffer(source.chars);
	}
	else
		if (chars != source.chars)
			assign(source.GetBuffer(), source.length());
}

void string::assign(const char* source, size_t count)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count > capacity())
			Alloc(count);
		Copy(chars, source, count);
		chars[metadata()->length = count] = 0;
	}
	else
		clear();
}

void string::assign(char source, size_t count)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count > capacity())
			Alloc(count);
		Copy(chars, source, count);
		chars[metadata()->length = count] = 0;
	}
	else
		clear();
}

/*
void string::assign(const char* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Copy(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		clear();
}

void string::assign(char source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Copy(Allocate(count), source, count);
		UnlockBuffer();
	}
	else
		clear();
}

void string::replace(size_t index, const char* source, size_t count)
{
	B_ASSERT(index >= 0 && index <= length() && count >= 0);

	size_t unaffected = index + count;

	Copy((unaffected <= length() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}

void string::replace(size_t index, char source, size_t count)
{
	B_ASSERT(index >= 0 && index <= length() && count >= 0);

	size_t unaffected = index + count;

	Copy((unaffected <= length() ? LockBuffer() :
		LockBuffer(unaffected)) + index, source, count);

	UnlockBuffer();
}
*/

void string::insert(size_t index, const char* source, size_t count)
{
	B_ASSERT(index <= length());
	// source must not be a part of this array
	B_ASSERT(source >= chars + capacity() || source + count < chars);

	if (count > 0)
	{
		char* tail = chars + index;
		size_t tail_length = length() - index;
		size_t new_length = length() + count;

		if (new_length > capacity() || is_shared())
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, index);
			Copy(new_buffer_chars + index, source, count);
			Copy(new_buffer_chars + index + count,
				tail, tail_length);

			ReplaceBuffer(new_buffer_chars);
		}
		else
		{
			if (count < tail_length)
			{
				Copy(tail + tail_length,
					tail + tail_length - count, count);

				ReverseCopy(tail + count,
					tail, tail_length - count);

				Copy(tail, source, count);
			}
			else
			{
				Copy(tail + tail_length, source + tail_length,
					count - tail_length);

				Copy(tail + count, tail, tail_length);

				Copy(tail, source, tail_length);
			}

			metadata()->length = new_length;
		}
	}
}

void string::insert(size_t index, char source, size_t count)
{
	B_ASSERT(index <= length());

	if (count > 0)
	{
		char* tail = chars + index;
		size_t tail_length = length() - index;
		size_t new_length = length() + count;

		if (new_length > capacity() || is_shared())
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, index);
			Copy(new_buffer_chars + index, source, count);
			Copy(new_buffer_chars + index + count,
				tail, tail_length);

			ReplaceBuffer(new_buffer_chars);
		}
		else
		{
			if (count < tail_length)
			{
				Copy(tail + tail_length,
					tail + tail_length - count, count);

				ReverseCopy(tail + count,
					tail, tail_length - count);

				Copy(tail, source, count);
			}
			else
			{
				Copy(tail + tail_length,
					source, count - tail_length);

				Copy(tail + count, tail, tail_length);

				Copy(tail, source, tail_length);
			}

			metadata()->length = new_length;
		}
	}
}

void string::append(const char* source, size_t count)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count + length() > capacity())
			Realloc(length() + count);
		Copy(chars + length(), source, count);
		chars[metadata()->length += count] = 0;
	}
}

void string::append(char source, size_t count)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count + length() > capacity())
			Realloc(length() + count);
		Copy(chars + length(), source, count);
		chars[metadata()->length += count] = 0;
	}
}

/*
void string::append(const char* source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(length() + count);
		Copy(chars + length(), source, count);
		Unlock();
	}
}

void string::append(char source, size_t count)
{
	B_ASSERT(count >= 0);

	if (count > 0)
	{
		Lock(length() + count);
		Copy(chars + length(), source, count);
		UnlockBuffer();
	}
}
*/

string string::operator +(const string& source) const
{
	string result(*this);
	return result += source;
}

void string::erase(size_t index, size_t count)
{
	if (index + count > length())
		count = length() - index;

	if (count > 0)
	{
		size_t new_length = length() - count;

		if (!is_shared())
		{
			Copy(chars + index, chars + index + count,
				new_length - index + 1);

			metadata()->length = new_length;
		}
		else
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, index);

			Copy(new_buffer_chars + index, chars + index + count,
				new_length - index + 1);

			ReplaceBuffer(new_buffer_chars);
		}
	}
}

void string::clear()
{
	if (!is_shared())
	{
		metadata()->length = 0;
		*chars = 0;
	}
	else
		ReplaceBuffer(empty_string());
}

void string::appendfv(const char* format, va_list arguments)
{
	alloc_and_copy(length() + 8 * 1024); // TODO reserve()?

	metadata()->length += FormatString(chars + length(),
		format, arguments);
}

void string::appendf(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	appendfv(format, arguments);
	va_end(arguments);
}

void string::format(const char* format, ...)
{
	va_list arguments;

	va_start(arguments, format);
	formatv(format, arguments);
	va_end(arguments);
}

size_t string::find(char c) const
{
	size_t counter = length();

	const char* pos = chars;

	while (counter-- > 0)
	{
		if (*pos == c)
			return (size_t) (pos - chars);

		++pos;
	}

	return (size_t) -1;
}

size_t string::rfind(char c) const
{
	size_t index = length();

	const char* pos = chars + index;

	while (index-- > 0)
		if (*--pos == c)
			return (size_t) (pos - chars);

	return (size_t) -1;
}

void string::trim_right(const char* samples)
{
	char* end = chars + length();

	while (--end >= chars &&
		FindCharInString(samples, *end) != NULL)
		;

	size_t new_length = ++end - chars;

	if (new_length < length())
	{
		if (!is_shared())
			*end = 0;
		else
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, chars, new_length);

			ReplaceBuffer(new_buffer_chars);

			new_buffer_chars[new_length] = 0;
		}

		metadata()->length = new_length;
	}
}

void string::trim_left(const char* samples)
{
	char* start = chars;

	while (*start && FindCharInString(samples, *start) != NULL)
		++start;

	if (start > chars)
	{
		size_t new_length = chars + length() - start;

		if (!is_shared())
			ReverseCopy(chars, start, new_length);
		else
		{
			char* new_buffer_chars = AllocBuffer(new_length);

			Copy(new_buffer_chars, start, new_length);

			ReplaceBuffer(new_buffer_chars);
		}

		chars[metadata()->length = new_length] = 0;
	}
}

char* string::empty_string()
{
	static const buffer empty_string_buffer =
	{
		/* refs         */ B_REFCOUNT_STATIC_INIT(2),
		/* capacity     */ 0,
		/* length       */ 0,
		/* first_char   */ {0}
	};

	return const_cast<char*>(empty_string_buffer.first_char);
}

char* string::AllocBufferExactly(size_t new_capacity, size_t length)
{
	B_ASSERT(new_capacity >= length);

	buffer* new_buffer = (buffer*) Memory::Alloc(sizeof(buffer) +
		new_capacity * sizeof(char));

	new_buffer->refs = 1;
	new_buffer->capacity = new_capacity;
	new_buffer->length = length;

	return new_buffer->first_char;
}

void string::ReplaceBuffer(char* new_buffer_chars)
{
	B_ASSERT(!is_locked());

	if (chars != empty_string() && !--metadata()->refs)
		Memory::Free(metadata());

	chars = new_buffer_chars;
}

string::~string()
	throw ()
{
	if (chars != empty_string() && (is_locked() || !--metadata()->refs))
		Memory::Free(metadata());
}

char* FindCharInString(const char* string, char c)
{
	while (*string != 0)
		if (*string++ == c)
			return const_cast<char*>(string);

	return NULL;
}

char* FindLastCharInString(const char* string, char c)
{
	size_t index = CalcLength(string);

	string += index;

	while (index-- > 0)
		if (*--string == c)
			return const_cast<char*>(string);

	return NULL;
}

B_END_NAMESPACE
