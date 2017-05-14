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

string::string(const string_view& source) : chars(empty_string())
{
	assign(source.data(), source.length());
}

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

string_view string::substr(size_t start, size_t substr_len) const
{
	B_ASSERT(start <= length());

	return string_view(chars + start,
		substr_len <= length() - start ? substr_len : length() - start);
}

string_view string::remainder(size_t pos) const
{
	B_ASSERT(pos <= length());

	return string_view(chars + pos, length() - pos);
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

void string::assign(size_t count, char_t ch)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count > capacity())
			discard_and_alloc(extra_capacity(count));
		assign_value(chars, count, ch);
		chars[metadata()->length = count] = 0;
	}
	else
		empty();
}

void string::replace(size_t index, const char_t* source, size_t count)
{
	B_ASSERT(index <= length());

	if (count == 0)
		return;

	size_t end_of_change = index + count;

	if (end_of_change > capacity() || is_shared())
	{
		char_t* new_buffer_chars;

		if (end_of_change >= length())
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(end_of_change), end_of_change);

			assign_pairwise(new_buffer_chars, chars, index);
			assign_pairwise(new_buffer_chars + index, source,
				count);
		}
		else
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(length()), length());

			assign_pairwise(new_buffer_chars, chars, index);
			assign_pairwise(new_buffer_chars + index, source,
				count);
			assign_pairwise(new_buffer_chars + end_of_change,
				chars + end_of_change,
				length() - end_of_change);
		}

		replace_buffer(new_buffer_chars);
	}
	else
	{
		assign_pairwise(chars + index, source, count);

		if (end_of_change > length())
			chars[metadata()->length = end_of_change] = 0;
	}
}

void string::replace(size_t index, char_t ch, size_t count)
{
	B_ASSERT(index <= length());

	if (count == 0)
		return;

	size_t end_of_change = index + count;

	if (end_of_change > capacity() || is_shared())
	{
		char_t* new_buffer_chars;

		if (end_of_change >= length())
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(end_of_change), end_of_change);

			assign_pairwise(new_buffer_chars, chars, index);
			assign_value(new_buffer_chars + index, count, ch);
		}
		else
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(length()), length());

			assign_pairwise(new_buffer_chars, chars, index);
			assign_value(new_buffer_chars + index, count, ch);
			assign_pairwise(new_buffer_chars + end_of_change,
				chars + end_of_change,
				length() - end_of_change);
		}

		replace_buffer(new_buffer_chars);
	}
	else
	{
		assign_value(chars + index, count, ch);

		if (end_of_change > length())
			chars[metadata()->length = end_of_change] = 0;
	}
}

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

void string::insert(size_t index, char_t ch, size_t count)
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
			assign_value(new_buffer_chars + index, count, ch);
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

				assign_value(tail, count, ch);
			}
			else
			{
				assign_value(tail + tail_length,
					count - tail_length, ch);

				assign_pairwise(tail + count,
					tail, tail_length);

				assign_value(tail, tail_length, ch);
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

void string::append(size_t count, char_t ch)
{
	B_ASSERT(!is_locked());

	if (count > 0)
	{
		if (is_shared() || count + length() > capacity())
			alloc_and_copy(extra_capacity(length() + count));
		assign_value(chars + length(), count, ch);
		chars[metadata()->length += count] = 0;
	}
}

string string::operator +(const string& source) const
{
	string result(*this);
	return result += source;
}

string string::operator +(char_t ch) const
{
	string result(*this);
	return result += ch;
}

void string::remove(size_t index, size_t count)
{
	B_ASSERT(index <= length());

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

void string::truncate(size_t new_length)
{
	B_ASSERT(new_length <= length());

	if (new_length < length())
	{
		if (!is_shared())
			chars[metadata()->length = new_length] = 0;
		else
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, new_length);
			new_buffer_chars[new_length] = 0;

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
	append_formatted_va(fmt, ap);
}

void string::append_formatted(const char_t* fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	append_formatted_va(fmt, ap);
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

void string::append_formatted_va(const char_t* fmt, va_list ap)
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

bool string::split(char_t delim, string_view* slice, string_view* rest) const
{
	size_t delim_pos = find(delim);

	if (delim_pos == (size_t) -1)
		return false;

	if (slice != NULL)
		slice->assign(chars, delim_pos);

	if (rest != NULL)
	{
		++delim_pos;

		rest->assign(chars + delim_pos, length() - delim_pos);
	}

	return true;
}

void string::trim_right(const char_t* samples)
{
	char_t* new_last = chars + length();

	while (--new_last >= chars && find_char(samples, *new_last) != NULL)
		;

	truncate((size_t) (new_last + 1 - chars));
}

void string::trim_left(const char_t* samples)
{
	if (is_empty())
		return;

	size_t new_length = length();
	const char_t* new_first = chars;

	do
		if (find_char(samples, *new_first) == NULL)
			break;
		else
			++new_first;
	while (--new_length > 0);

	if (new_first > chars)
	{
		if (!is_shared())
		{
			metadata()->length = new_length;
			assign_pairwise_backwards(chars, new_first,
				new_length + 1);
		}
		else
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, new_first,
				new_length);
			new_buffer_chars[new_length] = 0;

			replace_buffer(new_buffer_chars);
		}
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
