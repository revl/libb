// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

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

#if defined(B_USE_STL)
string::string(const std::basic_string<char_t>& source) : chars(empty_string())
{
	assign(source.data(), source.length());
}

std::basic_string<char_t> string::to_std_string() const
{
	return std::basic_string<char_t>(data(), length());
}
#endif /* defined(B_USE_STL) */

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

void string::assign(const string_view& source)
{
	assign(source.data(), source.length());
}

void string::replace(size_t start, const char_t* source, size_t count)
{
	B_ASSERT(start <= length());

	if (count == 0)
		return;

	size_t end_of_change = start + count;

	if (end_of_change > capacity() || is_shared())
	{
		char_t* new_buffer_chars;

		if (end_of_change >= length())
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(end_of_change), end_of_change);

			assign_pairwise(new_buffer_chars, chars, start);
			assign_pairwise(new_buffer_chars + start, source,
				count);
		}
		else
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(length()), length());

			assign_pairwise(new_buffer_chars, chars, start);
			assign_pairwise(new_buffer_chars + start, source,
				count);
			assign_pairwise(new_buffer_chars + end_of_change,
				chars + end_of_change,
				length() - end_of_change);
		}

		replace_buffer(new_buffer_chars);
	}
	else
	{
		assign_pairwise(chars + start, source, count);

		if (end_of_change > length())
			chars[metadata()->length = end_of_change] = 0;
	}
}

void string::replace(size_t start, char_t ch, size_t count)
{
	B_ASSERT(start <= length());

	if (count == 0)
		return;

	size_t end_of_change = start + count;

	if (end_of_change > capacity() || is_shared())
	{
		char_t* new_buffer_chars;

		if (end_of_change >= length())
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(end_of_change), end_of_change);

			assign_pairwise(new_buffer_chars, chars, start);
			assign_value(new_buffer_chars + start, count, ch);
		}
		else
		{
			new_buffer_chars = alloc_buffer(
				extra_capacity(length()), length());

			assign_pairwise(new_buffer_chars, chars, start);
			assign_value(new_buffer_chars + start, count, ch);
			assign_pairwise(new_buffer_chars + end_of_change,
				chars + end_of_change,
				length() - end_of_change);
		}

		replace_buffer(new_buffer_chars);
	}
	else
	{
		assign_value(chars + start, count, ch);

		if (end_of_change > length())
			chars[metadata()->length = end_of_change] = 0;
	}
}

void string::insert(size_t pos, const char_t* source, size_t count)
{
	B_ASSERT(pos <= length());
	// source must not be a part of this array
	B_ASSERT(source >= chars + capacity() || source + count < chars);

	if (count > 0)
	{
		char_t* tail = chars + pos;
		size_t tail_length = length() - pos;
		size_t new_length = length() + count;

		if (new_length > capacity() || is_shared())
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, pos);
			assign_pairwise(new_buffer_chars + pos, source, count);
			assign_pairwise(new_buffer_chars + pos + count,
				tail, tail_length + 1);

			replace_buffer(new_buffer_chars);
		}
		else
		{
			move_right_and_insert(tail, tail_length, source, count);

			metadata()->length = new_length;
		}
	}
}

void string::insert(size_t pos, char_t ch, size_t count)
{
	B_ASSERT(pos <= length());

	if (count > 0)
	{
		char_t* tail = chars + pos;
		size_t tail_length = length() - pos;
		size_t new_length = length() + count;

		if (new_length > capacity() || is_shared())
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, pos);
			assign_value(new_buffer_chars + pos, count, ch);
			assign_pairwise(new_buffer_chars + pos + count,
				tail, tail_length + 1);

			replace_buffer(new_buffer_chars);
		}
		else
		{
			move_right_and_insert(tail, tail_length, ch, count);

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

string string::operator +(const string_view& sv) const
{
	string result(*this);
	return result += sv;
}

void string::remove(size_t start, size_t count)
{
	B_ASSERT(start <= length());

	if (start + count > length())
		count = length() - start;

	if (count > 0)
	{
		size_t new_length = length() - count;

		if (!is_shared())
		{
			move_left(chars + start,
				chars + start + count, new_length - start + 1);

			metadata()->length = new_length;
		}
		else
		{
			char_t* new_buffer_chars = alloc_buffer(
				extra_capacity(new_length), new_length);

			assign_pairwise(new_buffer_chars, chars, start);

			assign_pairwise(new_buffer_chars + start,
				chars + start + count, new_length - start + 1);

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

bool string::starts_with(const string_view& prefix) const
{
	return length() >= prefix.length() &&
		compare_arrays(data(), prefix.data(), prefix.length()) == 0;
}

bool string::ends_with(const string_view& suffix) const
{
	return length() >= suffix.length()  &&
		compare_arrays(data() + length() - suffix.length(),
			suffix.data(), suffix.length()) == 0;
}

bool string::matches_pattern(const char_t* pattern) const
{
	return match_pattern(*this, pattern);
}

bool string::matches_pattern(const string_view& pattern) const
{
	return match_pattern(*this, pattern);
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

size_t string::find(char_t ch) const
{
	size_t counter = length();

	const char_t* ptr = chars;

	while (counter-- > 0)
	{
		if (*ptr == ch)
			return (size_t) (ptr - chars);

		++ptr;
	}

	return (size_t) -1;
}

size_t string::rfind(char_t ch) const
{
	size_t pos = length();

	const char_t* ptr = chars + pos;

	while (pos-- > 0)
		if (*--ptr == ch)
			return (size_t) (ptr - chars);

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

void string::trim_right(const string_view& samples)
{
	char_t* new_last = chars + length();

	while (--new_last >= chars && samples.find(*new_last) != (size_t) -1)
		;

	truncate((size_t) (new_last + 1 - chars));
}

void string::trim_left(const string_view& samples)
{
	if (is_empty())
		return;

	size_t new_length = length();
	const char_t* new_first = chars;

	do
		if (samples.find(*new_first) == (size_t) -1)
			break;
		else
			++new_first;
	while (--new_length > 0);

	if (new_first > chars)
	{
		if (!is_shared())
		{
			metadata()->length = new_length;
			move_left(chars, new_first, new_length + 1);
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
		/* refs         */ 2,
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

char_t* find_char(const char_t* input, char_t ch)
{
	while (*input != 0)
		if (*input++ == ch)
			return const_cast<char_t*>(input);

	return NULL;
}

char_t* find_char_reverse(const char_t* input, char_t ch)
{
	size_t index = calc_length(input);

	input += index;

	while (index-- > 0)
		if (*--input == ch)
			return const_cast<char_t*>(input);

	return NULL;
}

B_END_NAMESPACE
