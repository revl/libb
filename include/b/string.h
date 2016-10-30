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

#ifndef B_STRING_H

#if defined(B_STRING_DECL)

B_BEGIN_NAMESPACE

// Sequence of characters.
class string
{
// Construction
public:
	// Creates an empty string.
	string();

	// Constructs a copy of an existing string.
	string(const string& source);

	// Constructs a string from a linear character array.
	string(const char_t* source, size_t count);

	// Creates a string of <count> copies of source.
	explicit string(char_t source, size_t count = 1);

// String Length
public:
	// Returns the size (in characters) of the allocated buffer.
	size_t capacity() const;

	// Returns the string length in characters.
	size_t length() const
	{
		return metadata()->length;
	}

	// Returns true if the string is empty.
	bool is_empty() const;

// Lock State
public:
	// Checks if the string is locked.
	bool is_locked() const;

// Memory Management
public:
	// Discards the string contents and allocates the exact
	// amount of memory for the buffer.
	void discard_and_alloc(size_t new_capacity);

	// Reallocates the buffer preserving the string contents.
	void alloc_and_copy(size_t new_capacity);

	// Allocates enough memory to store 'new_capacity' characters.
	// In case if the buffer gets reallocated, the contents is
	// not preserved.
	void reserve(size_t new_capacity);

	// Frees memory that is not used by the string.
	void shrink_to_fit();

// Buffer Access
public:
	// Returns a constant pointer to the null-terminated
	// array of characters that this string contains.
	const char_t* data() const
	{
		return chars;
	}

	// Fixes the buffer in memory disabling the memory reallocation.
	char_t* lock();

	// Gives control over the buffer back to this object.
	void unlock();

	// Unlocks the buffer and sets the new string length.
	void unlock(size_t new_length);

// Single Character Access
public:
	// Returns the character with the specified index.
	char_t at(size_t index) const;

	// Returns a reference to the character at the specified position.
	char_t& at(size_t index);

	// Returns the character with the specified index.
	char_t operator [](size_t index) const;

	// Returns a reference to the specified character.
	char_t& operator [](size_t index);

	// Returns the first character of the string.
	char_t front() const;

	// Returns a reference to the first character of the string.
	char_t& front();

	// Returns the last character of the string.
	char_t back() const;

	// Returns a reference to the last character of the string.
	char_t& back();

// Assignment
public:
	// Assigns the contents of <source> to this string.
	void assign(const string& source);

	// Replaces the string contents with a copy of character array.
	void assign(const char_t* source, size_t count);

	// Replaces the string contents with <count> copies of <source>.
	void assign(char_t source, size_t count = 1);

	// Assigns the contents of one string object to another.
	string& operator =(const string& source);

	// Replaces the contents of this string with a character.
	string& operator =(char_t source);

// Replacement
public:
	// Replaces a part of this string with a character sequence.
	void replace(size_t index, const char_t* source, size_t count);

	// Replaces a part of this string with a fill character.
	void replace(size_t index, char_t source, size_t count = 1);

// Insertion
public:
	// Inserts a character array into this string.
	void insert(size_t index, const char_t* source, size_t count);

	// Inserts the contents of <source> into this string.
	void insert(size_t index, const string& source);

	// Inserts <count> copies of <source> into this string.
	void insert(size_t index, char_t source, size_t count = 1);

// Concatenation
public:
	// Appends characters from <source> to this string.
	void append(const char_t* source, size_t count);

	// Appends the contents of <source> at the end of this string.
	void append(const string& source);

	// Appends <count> copies of <source> to this string.
	void append(char_t source, size_t count = 1);

	// Operator version of append(source).
	string& operator +=(const string& source);

	// Operator version of append(source).
	string& operator +=(char_t source);

	// Constructs a new string from the concatenation of
	// this one and <source>.
	string operator +(const string& source) const;

	// Constructs a new string from the concatenation of
	// this one and character <source>.
	string operator +(char_t source) const;

// Deletion
public:
	// Deletes a range of characters.
	void erase(size_t index, size_t count = 1);

	// Empties the string.
	void clear();

// Comparison
public:
	// Compares this string against 'rhs' lexicographically.
	int compare(const char_t* rhs) const;

	// Returns true if this string and 'rhs' are equal.
	bool operator ==(const string& rhs) const;

	// Checks this string and 'rhs' for equality.
	bool operator ==(const char_t* rhs) const;

	// Returns true if this string and 'rhs' differ.
	bool operator !=(const string& rhs) const;

	// Returns true if this string and 'rhs' differ.
	bool operator !=(const char_t* rhs) const;

	// Returns true if this string is less than 'rhs'.
	bool operator <(const string& rhs) const;

	// Returns true if this string is less than 'rhs'.
	bool operator <(const char_t* rhs) const;

	// Returns true if this string is greater than 'rhs'.
	bool operator >(const string& rhs) const;

	// Returns true if this string is greater than 'rhs'.
	bool operator >(const char_t* rhs) const;

	// Returns true if this string is less or equal than 'rhs'.
	bool operator <=(const string& rhs) const;

	// Returns true if this string is less or equal than 'rhs'.
	bool operator <=(const char_t* rhs) const;

	// Returns true if this string is greater or equal than 'rhs'.
	bool operator >=(const string& rhs) const;

	// Returns true if this string is greater or equal than 'rhs'.
	bool operator >=(const char_t* rhs) const;

// Formatting
public:
	// Constructs a new string from a format string.
	static string formatted(const char_t* fmt, ...) B_PRINTF_STYLE(1, 2);
	static string formatted_va(const char_t* fmt, va_list ap);

	// Formats this string using the specified format.
	void format(const char_t* fmt, ...) B_PRINTF_STYLE(2, 3);
	void format_va(const char_t* fmt, va_list ap);

	// Appends formatted data to this string.
	void append_format(const char_t* fmt, ...) B_PRINTF_STYLE(2, 3);
	void append_format_va(const char_t* fmt, va_list ap);

// Searching
public:
	// Finds a character in this string.
	size_t find(char_t c) const;

	// Finds the last occurrence of a character in this string.
	size_t rfind(char_t c) const;

// Character Removal Operations
public:
	// Removes all occurrences of characters from <samples>
	// from the end of this string.
	void trim_right(const char_t* samples);

	// Removes all occurrences of characters from <samples>
	// found at the beginning of this string.
	void trim_left(const char_t* samples);

	// Removes all characters from <samples> from both ends of this string.
	void trim(const char_t* samples);

// Implementation
private:
	struct buffer
	{
		RefCount refs;
		size_t capacity;
		size_t length;
		char_t first_char[1];
	};

	char_t* chars;

	bool is_shared() const;

	static char_t* empty_string();

	static char_t* alloc_buffer(size_t capacity, size_t length);

	static buffer* metadata(const char_t* chars)
	{
		return B_OUTERSTRUCT(buffer, first_char[0], chars);
	}

	buffer* metadata() const
	{
		return metadata(chars);
	}

	void replace_buffer(char_t* new_buffer_chars);

	// Make sure that the buffer is not shared with other strings.
	// Reallocate the buffer if it's shared; preserve the original
	// buffer contents.
	void isolate();

public:
	~string();
};

B_END_NAMESPACE

#elif defined(B_STRING_INLINE)

B_BEGIN_NAMESPACE

inline string::string() : chars(empty_string())
{
}

inline string::string(const string& source) : chars(empty_string())
{
	assign(source);
}

inline string::string(const char_t* source, size_t count) :
	chars(empty_string())
{
	assign(source, count);
}

inline string::string(char_t source, size_t count) : chars(empty_string())
{
	assign(source, count);
}

inline bool string::is_locked() const
{
	return metadata()->refs <= 0;
}

inline size_t string::capacity() const
{
	return metadata()->capacity;
}

inline bool string::is_empty() const
{
	return length() == 0;
}

inline bool string::is_shared() const
{
	return metadata()->refs > 1;
}

inline void string::reserve(size_t new_capacity)
{
	if (capacity() < new_capacity || is_shared())
		alloc_and_copy(extra_capacity(new_capacity));
}

inline void string::shrink_to_fit()
{
	size_t len = length();

	if (!is_shared() && len != capacity())
	{
		char_t* new_buffer_chars = alloc_buffer(len, len);

		assign_pairwise(new_buffer_chars, chars, len + 1);

		replace_buffer(new_buffer_chars);
	}
}

inline void string::isolate()
{
	if (is_shared())
	{
		size_t len = length();

		char_t* new_buffer_chars =
			alloc_buffer(extra_capacity(len), len);

		assign_pairwise(new_buffer_chars, chars, len + 1);

		replace_buffer(new_buffer_chars);
	}
}

inline char_t* string::lock()
{
	isolate();

	--metadata()->refs;

	return chars;
}

inline void string::unlock()
{
	B_ASSERT(is_locked());

	++metadata()->refs;
}

inline void string::unlock(size_t new_length)
{
	B_ASSERT(is_locked() && new_length <= capacity());

	chars[metadata()->length = new_length] = B_L_PREFIX('\0');
	++metadata()->refs;
}

inline char_t string::at(size_t index) const
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	return chars[index];
}

inline char_t& string::at(size_t index)
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	isolate();
	return chars[index];
}

inline char_t string::operator [](size_t index) const
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	return chars[index];
}

inline char_t& string::operator [](size_t index)
{
	B_ASSERT(index < (is_locked() ? capacity() : length()));

	isolate();
	return chars[index];
}

inline char_t string::front() const
{
	B_ASSERT(!is_empty());

	return *chars;
}

inline char_t& string::front()
{
	B_ASSERT(!is_empty());

	isolate();
	return *chars;
}

inline char_t string::back() const
{
	B_ASSERT(!is_empty());

	return chars[length() - 1];
}

inline char_t& string::back()
{
	B_ASSERT(!is_empty());

	isolate();
	return chars[length() - 1];
}

inline string& string::operator =(const string& source)
{
	assign(source);
	return *this;
}

inline string& string::operator =(char_t source)
{
	assign(source);
	return *this;
}

inline void string::insert(size_t index, const string& source)
{
	insert(index, source.data(), source.length());
}

inline void string::append(const string& source)
{
	append(source.data(), source.length());
}

inline string& string::operator +=(const string& source)
{
	append(source.data(), source.length());
	return *this;
}

inline string& string::operator +=(char_t source)
{
	append(source);
	return *this;
}

inline string string::operator +(char_t source) const
{
	string result(*this);
	return result += source;
}

inline int string::compare(const char_t* rhs) const
{
	return compare_strings(chars, rhs);
}

inline bool string::operator ==(const string& rhs) const
{
	return compare(rhs.data()) == 0;
}

inline bool string::operator ==(const char_t* rhs) const
{
	return compare(rhs) == 0;
}

inline bool string::operator !=(const string& rhs) const
{
	return compare(rhs.data()) != 0;
}

inline bool string::operator !=(const char_t* rhs) const
{
	return compare(rhs) != 0;
}

inline bool string::operator <(const string& rhs) const
{
	return compare(rhs.data()) < 0;
}

inline bool string::operator <(const char_t* rhs) const
{
	return compare(rhs) < 0;
}

inline bool string::operator >(const string& rhs) const
{
	return compare(rhs.data()) > 0;
}

inline bool string::operator >(const char_t* rhs) const
{
	return compare(rhs) > 0;
}

inline bool string::operator <=(const string& rhs) const
{
	return compare(rhs.data()) <= 0;
}

inline bool string::operator <=(const char_t* rhs) const
{
	return compare(rhs) <= 0;
}

inline bool string::operator >=(const string& rhs) const
{
	return compare(rhs.data()) >= 0;
}

inline bool string::operator >=(const char_t* rhs) const
{
	return compare(rhs) >= 0;
}

inline void string::trim(const char_t* samples)
{
	trim_right(samples);
	trim_left(samples);
}

inline string operator +(char_t ch, const string& str)
{
	string result;
	size_t length = str.length();

	result.discard_and_alloc(extra_capacity(length + 1));
	char_t* chars = result.lock();

	*chars = ch;
	assign_pairwise(chars + 1, str.data(), length);

	result.unlock(length + 1);
	return result;
}

inline bool operator ==(const char_t* chars, const string& str)
{
	return str.compare(chars) == 0;
}

inline bool operator !=(const char_t* chars, const string& str)
{
	return str.compare(chars) != 0;
}

inline bool operator <(const char_t* chars, const string& str)
{
	return str.compare(chars) > 0;
}

inline bool operator >(const char_t* chars, const string& str)
{
	return str.compare(chars) < 0;
}

inline bool operator <=(const char_t* chars, const string& str)
{
	return str.compare(chars) >= 0;
}

inline bool operator >=(const char_t* chars, const string& str)
{
	return str.compare(chars) <= 0;
}

// Finds the first occurrence of character 'c' in 'input'.
char_t* find_char(const char_t* input, char_t c);

// Finds the last occurrence of character 'c' in 'input'.
char_t* find_char_backwards(const char_t* input, char_t c);

B_END_NAMESPACE

#else /* !defined(B_STRING_DECL) && !defined(B_STRING_INLINE) */

#include "ref_count.h"

#define B_STRING_DECL

#define B_PRINTF_STYLE(fmt_index, arg_index)

#define string wstring
#define char_t wchar_t
#include "string.h"
#undef char_t
#undef string

#if defined(__GNUG__)
#undef B_PRINTF_STYLE
#define B_PRINTF_STYLE(fmt_index, arg_index) \
	__attribute__ ((format (printf, fmt_index, arg_index)))
#endif /* defined(__GNUG__) */

#define char_t char
#include "string.h"
#undef char_t

#undef B_PRINTF_STYLE

#undef B_STRING_DECL

#define B_STRING_H
#include "misc.h"
#undef B_STRING_H

#define B_STRING_INLINE

#define string wstring
#define char_t wchar_t
#define B_L_PREFIX(ch) L##ch
#include "string.h"
#undef B_L_PREFIX
#undef char_t
#undef string

#define char_t char
#define B_L_PREFIX(ch) ch
#include "string.h"
#undef B_L_PREFIX
#undef char_t

#undef B_STRING_INLINE

#define B_STATIC_CONST_STRING_IMPL(char_type, string_type, name, value) \
	static struct \
	{ \
		b::RefCount refs; \
		size_t capacity; \
		size_t length; \
		char_type chars[sizeof(value)]; \
	} \
	const name##_buffer = \
	{ \
		B_REFCOUNT_STATIC_INIT(-1), \
		sizeof(value) / sizeof(char_type) - 1, \
		sizeof(value) / sizeof(char_type) - 1, \
		value \
	}; \
	static const char_type* name##_buffer_chars = name##_buffer.chars; \
	static const b::string_type& name = \
		(*(const b::string_type*) &name##_buffer_chars)

#define B_STATIC_CONST_STRING(name, value) \
	B_STATIC_CONST_STRING_IMPL(char, string, name, value)

#define B_STATIC_CONST_WSTRING(name, value) \
	B_STATIC_CONST_STRING_IMPL(wchar_t, wstring, name, L##value)

#define B_STRING_H

#endif /* defined(B_STRING_DECL) || defined(B_STRING_INLINE) */

#endif /* !defined(B_STRING_H) */
