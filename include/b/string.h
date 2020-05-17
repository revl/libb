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

// Declaration of the string and wstring classes.

#ifndef B_STRING_H

#if defined(B_STRING_DECL)

B_BEGIN_NAMESPACE

class string_view;

// Sequence of characters.
class string
{
// Construction
public:
	// Creates an empty string.
	string();

	// Constructs a copy of an existing string.
	string(const string& source);

	// Constructs a string from a string view.
	explicit string(const string_view& source);

	// Constructs a string from a linear character array.
	string(const char_t* source, size_t count);

	// Constructs a string from a linear character array
	// repeated multiple times.
	string(const char_t* source, size_t count, size_t times);

	// Creates a string of 'count' copies of 'ch'.
	string(size_t count, char_t ch);

#ifndef string
	// Creates a multibyte string from a wide character string.
	static string from_wstring(const wstring& wstr);

	// Creates a wide character string from 'this' multibyte string.
	wstring to_wstring() const;
#endif

// Interoperability
public:
#if defined(B_USE_STL)
	// Creates a b::string from an std::string.
	explicit string(const std::basic_string<char_t>& source);

	// Creates an std::string from this b::string.
	std::basic_string<char_t> to_std_string() const;
#endif /* defined(B_USE_STL) */

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
	void trim_to_size();

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

	// Returns the character with the specified index.
	char_t at(size_t index) const;

	// Returns a reference to the character at the specified position.
	char_t& at(size_t index);

	// Returns the character with the specified index.
	char_t operator [](size_t index) const;

	// Returns a reference to the specified character.
	char_t& operator [](size_t index);

	// Returns the first character of the string.
	char_t first() const;

	// Returns a reference to the first character of the string.
	char_t& first();

	// Returns the last character of the string.
	char_t last() const;

	// Returns a reference to the last character of the string.
	char_t& last();

	// Returns a substring of this string as a string_view object.
	string_view substr(size_t start, size_t substr_len) const;

	// Returns a string view that spans from 'pos' to the end
	// of this string.
	string_view remainder(size_t pos) const;

// Assignment
public:
	// Assigns the contents of 'source' to this string.
	void assign(const string& source);

	// Replaces the string contents with a copy of character array.
	void assign(const char_t* source, size_t count);

	// Replaces the string contents with 'count' copies of 'ch'.
	void assign(size_t count, char_t ch);

	// Copies the contents of the specified string view to this string.
	void assign(const string_view& source);

	// Assigns the contents of one string object to another.
	string& operator =(const string& source);

// Replacement
public:
	// Replaces a part of this string with a character sequence.
	void replace(size_t start, const char_t* source, size_t count);

	// Replaces a part of this string with a fill character.
	void replace(size_t start, char_t ch, size_t count);

// Insertion
public:
	// Inserts a character array into this string.
	void insert(size_t pos, const char_t* source, size_t count);

	// Inserts the contents of 'source' into this string.
	void insert(size_t pos, const string& source);

	// Inserts 'count' copies of 'ch' into this string.
	void insert(size_t pos, char_t ch, size_t count = 1);

// Concatenation
public:
	// Appends characters from 'source' to this string.
	void append(const char_t* source, size_t count);

	// Appends the contents of 'source' at the end of this string.
	void append(const string& source);

	// Appends 'count' copies of 'ch' to this string.
	void append(size_t count, char_t ch);

	// Appends a single character to this string.
	void append(char_t ch);

	// Appends the contents of 'source' at the end of this
	// string (operator version of append(source)).
	string& operator +=(const string& source);

	// Appends a single character to this string
	// (operator version of append(char_t)).
	string& operator +=(char_t ch);

	// Constructs a new string from the concatenation of
	// this one and 'source'.
	string operator +(const string& source) const;

	// Constructs a new string from the concatenation of
	// this one and character 'ch'.
	string operator +(char_t ch) const;

	// Concatenates 'times' copies of this string and
	// returns the result.
	string repeat(size_t times) const;

	// Concatenates 'times' copies of this string and
	// returns the result (operator version of
	// repeat(times)).
	string operator *(size_t times) const;

// Deletion
public:
	// Deletes a range of characters.
	void remove(size_t start, size_t count = 1);

	// Deletes characters starting at 'new_length' until
	// the end of the string.
	void truncate(size_t new_length);

	// Empties the string.
	void empty();

// Comparison
public:
	// Compares two strings lexicographically.
	int compare(const string& rhs) const;

	// Compares this string and a string_view lexicographically.
	int compare(const string_view& rhs) const;

	// Compares this string with a null-terminated string
	// lexicographically.
	int compare(const char_t* rhs) const;

	// Checks if this string starts with the specified prefix.
	bool starts_with(const string_view& prefix) const;

	// Checks if this string ends with the specified suffix.
	bool ends_with(const string_view& suffix) const;

	// Checks if this string matches the specified glob-style pattern.
	bool matches_pattern(const char_t* pattern) const;

	// Checks if this string matches the specified glob-style pattern.
	bool matches_pattern(const string_view& pattern) const;

	// Returns true if this string and 'rhs' are equal.
	bool operator ==(const string& rhs) const;

	// Checks if this string and the specified string view are equal.
	bool operator ==(const string_view& rhs) const;

	// Checks this string and 'rhs' for equality.
	bool operator ==(const char_t* rhs) const;

	// Returns true if this string and 'rhs' differ.
	bool operator !=(const string& rhs) const;

	// Returns true if this string and the specified string view
	// are not equal.
	bool operator !=(const string_view& rhs) const;

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
	static string formatted(const char_t* fmt, ...) B_STRING_PRINTF(1, 2);
	static string formatted_va(const char_t* fmt, va_list ap);

	// Formats this string using the specified format.
	void format(const char_t* fmt, ...) B_STRING_PRINTF(2, 3);
	void format_va(const char_t* fmt, va_list ap);

	// Appends formatted data to this string.
	void append_formatted(const char_t* fmt, ...) B_STRING_PRINTF(2, 3);
	void append_formatted_va(const char_t* fmt, va_list ap);

// Searching
public:
	// Finds a character in this string.
	size_t find(char_t ch) const;

	// Finds the last occurrence of a character in this string.
	size_t rfind(char_t ch) const;

	// Splits the string into two parts at the first occurrence of
	// the delimiter character.  Either of the output pointers can
	// be NULL.  Returns true if the delimiter was found and the
	// split happened.
	bool split(char_t delim, string_view* slice, string_view* rest) const;

// Character Removal Operations
public:
	// Removes all occurrences of characters from 'samples'
	// from the end of this string.
	void trim_right(const string_view& samples);

	// Removes all occurrences of characters from 'samples'
	// found at the beginning of this string.
	void trim_left(const string_view& samples);

	// Removes all characters from 'samples' from both ends of this string.
	void trim(const string_view& samples);

// Implementation
private:
	struct buffer
	{
		int refs;
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

inline string::string(size_t count, char_t ch) : chars(empty_string())
{
	assign(count, ch);
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

inline void string::trim_to_size()
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

inline char_t string::first() const
{
	B_ASSERT(!is_empty());

	return *chars;
}

inline char_t& string::first()
{
	B_ASSERT(!is_empty());

	isolate();
	return *chars;
}

inline char_t string::last() const
{
	B_ASSERT(!is_empty());

	return chars[length() - 1];
}

inline char_t& string::last()
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

inline void string::insert(size_t index, const string& source)
{
	insert(index, source.data(), source.length());
}

inline void string::append(const string& source)
{
	append(source.data(), source.length());
}

inline void string::append(char_t ch)
{
	append(1, ch);
}

inline string& string::operator +=(const string& source)
{
	append(source.data(), source.length());
	return *this;
}

inline string& string::operator +=(char_t ch)
{
	append(1, ch);
	return *this;
}

inline string string::repeat(size_t times) const
{
	if (times == 1)
		return *this;

	return string(chars, length(), times);
}

inline string string::operator *(size_t times) const
{
	return repeat(times);
}

inline int string::compare(const string& rhs) const
{
	return compare_strings(data(), length(), rhs.data(), rhs.length());
}

inline int string::compare(const string_view& rhs) const
{
	return compare_strings(data(), length(), rhs.data(), rhs.length());
}

inline int string::compare(const char_t* rhs) const
{
	return compare_strings(chars, rhs);
}

inline bool string::operator ==(const string& rhs) const
{
	return compare(rhs) == 0;
}

inline bool string::operator ==(const string_view& rhs) const
{
	return compare(rhs) == 0;
}

inline bool string::operator ==(const char_t* rhs) const
{
	return compare(rhs) == 0;
}

inline bool string::operator !=(const string& rhs) const
{
	return compare(rhs) != 0;
}

inline bool string::operator !=(const string_view& rhs) const
{
	return compare(rhs) != 0;
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

inline void string::trim(const string_view& samples)
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

// Finds the first occurrence of character 'ch' in 'input'.
char_t* find_char(const char_t* input, char_t ch);

// Finds the last occurrence of character 'ch' in 'input'.
char_t* find_char_backwards(const char_t* input, char_t ch);

B_END_NAMESPACE

#else /* !defined(B_STRING_DECL) && !defined(B_STRING_INLINE) */

#include "host.h"

#define B_STRING_DECL

#define B_STRING_PRINTF(fmt_index, arg_index)

#define string wstring
#define string_view wstring_view
#define char_t wchar_t
#include "string.h"
#undef char_t
#undef string_view
#undef string

#undef B_STRING_PRINTF
#define B_STRING_PRINTF(fmt_index, arg_index) \
	B_PRINTF_STYLE(fmt_index, arg_index)

#define char_t char
#include "string.h"
#undef char_t

#undef B_STRING_PRINTF

#undef B_STRING_DECL

#define B_STRING_H
#include "fn.h"
#undef B_STRING_H

#define B_STRING_INLINE

#define string wstring
#define string_view wstring_view
#define char_t wchar_t
#define B_L_PREFIX(ch) L##ch
#include "string.h"
#undef B_L_PREFIX
#undef char_t
#undef string_view
#undef string

#define char_t char
#define B_L_PREFIX(ch) ch
#include "string.h"
#undef B_L_PREFIX
#undef char_t

#undef B_STRING_INLINE

#define B_STRING_LITERAL_IMPL(char_type, string_type, name, value) \
	static struct \
	{ \
		int refs; \
		size_t capacity; \
		size_t length; \
		char_type chars[sizeof(value)]; \
	} \
	const name##_buffer = \
	{ \
		-1, \
		sizeof(value) / sizeof(char_type) - 1, \
		sizeof(value) / sizeof(char_type) - 1, \
		value \
	}; \
	static const char_type* name##_buffer_chars = name##_buffer.chars; \
	static const b::string_type& name = \
		(*(const b::string_type*) &name##_buffer_chars)

#define B_STRING_LITERAL(name, value) \
	B_STRING_LITERAL_IMPL(char, string, name, value)

#define B_WSTRING_LITERAL(name, value) \
	B_STRING_LITERAL_IMPL(wchar_t, wstring, name, L##value)

#define B_STRING_H

#endif /* defined(B_STRING_DECL) || defined(B_STRING_INLINE) */

#endif /* !defined(B_STRING_H) */
