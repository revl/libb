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

#include "string_view.h"

#ifndef B_MISC_H
#define B_MISC_H

#include "memory.h"
#include "pseudorandom.h"
#include "arg_list.h"

B_BEGIN_NAMESPACE

// Global utility functions

// Increments the specified size value. This function is for use by
// the containers that reserve additional space for future growth.
inline size_t extra_capacity(size_t size)
{
	size_t extra = size >> 3;

	return size + (extra > 4 ? (extra <= 1024 ? extra : 1024) : 4);
}

// Operations on the file system directory structure

// Returns true if the specified directory exists.
bool is_directory(const string& directory);

// Named parameters for use with create_directory().
namespace args
{
	// Flag to enable creation of the missing parent directories.
	extern arg_name<bool, 0> create_parents;

	// The mode for the new directory.
	//
	// The default value is 0777 & ~umask.
	extern arg_name<mode_t, 1> mode;

	// If 'create_parents' parameter is also given, 'parent_mode'
	// is the mode for the new parent directories.
	//
	// If not specified, the value of 'mode' is used.
	extern arg_name<mode_t, 2> parent_mode;
}

// Creates a directory, possibly with its parents.
// The 'arg' argument allows a list of named parameters
// declared in the 'args' namespace above.
void create_directory(const string& path, const arg_list* arg = NULL);

void remove_directory(const string& directory);

// Global function templates

// Determines the lowest position before which 'value' can be inserted in
// the ordered sequence of 'count' elements preserving the ordering.
template <class Iter, class T>
Iter find_lower_bound(Iter first, size_t count, const T& value)
{
	Iter middle;
	size_t prev_count;

	while (count > 0)
	{
		prev_count = count;

		if (*(middle = first + (count >>= 1)) < value)
		{
			++(first = middle);

			if (!(prev_count & 1))
				--count;
		}
	}

	return first;
}

// Aligns an integer value on the 'alignment' boundary.
template <class T>
inline T align(T value, size_t alignment)
{
	size_t remainder = (size_t) value % alignment;

	return remainder == 0 ? value :
		(T) ((size_t) value + alignment - remainder);
}

// Exchanges values of two variables.
template <class T>
inline void swap(T& object1, T& object2)
{
	T dummy(object1);
	object1 = object2;
	object2 = dummy;
}

// Shuffles the sequence of objects using the pseudorandom number generator.
template <class T>
inline void shuffle(T* objects, pseudorandom::value_type count)
{
	if (count <= 1)
		return;

	pseudorandom prng;

	pseudorandom::value_type i = count;

	do
		swap(objects[--i], objects[prng.next(count)]);
	while (i > 0);
}

// Compares two objects. Returns zero if objects are equal,
// -1 if the left-hand object is less than the right-hand,
// and 1 otherwise.
template <class T>
inline int compare(const T& object1, const T& object2)
{
	return object1 < object2 ? -1 : object2 < object1;
}

template <>
inline int compare(const int& n1, const int& n2)
{
	return n1 - n2;
}

template <>
inline int compare(const long& l1, const long& l2)
{
	const long diff = l1 - l2;

	return diff < 0 ? -1 : diff > 0;
}

template <>
inline int compare(const char& c1, const char& c2)
{
	return int(c1) - int(c2);
}

template <>
inline int compare(const wchar_t& c1, const wchar_t& c2)
{
	return int(c1) - int(c2);
}

// Compares two object sequences of the same length.
template <class T>
inline int compare_arrays(const T* array1, const T* array2, size_t count)
{
	register int cmp_res;

	while (count-- > 0)
		if ((cmp_res = compare(*array1++, *array2++)) != 0)
			return cmp_res;

	return 0;
}

template <>
inline int compare_arrays(const char* array1, const char* array2, size_t count)
{
	return memory::compare(array1, array2, count);
}

// Operations on C strings

// Computes the length of a null-terminated string.
inline size_t calc_length(const char* string)
{
	return ::strlen(string);
}

// Computes the length of a null-terminated string (wchar_t version).
inline size_t calc_length(const wchar_t* string)
{
	return ::wcslen(string);
}

// Computes the length of a string either terminated by
// a null character or limited by the 'limit' parameter.
inline size_t calc_length(const char* string, size_t limit)
{
	const char* null_char_ptr = (const char*) memchr(string, 0, limit);

	return null_char_ptr != NULL ?
		(size_t) (null_char_ptr - string) : limit;
}

// Computes the length of a string either terminated by
// a null character or limited by the 'limit' parameter (wchar_t version).
inline size_t calc_length(const wchar_t* string, size_t limit)
{
	const wchar_t* null_char_ptr = wmemchr(string, L'\0', limit);

	return null_char_ptr != NULL ?
		(size_t) (null_char_ptr - string) : limit;
}

// Compares two null-terminated strings.
inline int compare_strings(const char* lhs, const char* rhs)
{
	return ::strcmp(lhs, rhs);
}

// Compares two null-terminated strings. Only the first (at most)
// 'max_len' characters are compared.
inline int compare_strings(const char* lhs, const char* rhs,
	size_t max_len)
{
	return ::strncmp(lhs, rhs, max_len);
}

// Compares two strings of the specified lengths.
int compare_strings(const char* lhs, size_t lhs_len,
	const char* rhs, size_t rhs_len);

// Compares two strings.
inline int compare_strings(const wchar_t* lhs, const wchar_t* rhs)
{
	return ::wcscmp(lhs, rhs);
}

// Compares two null-terminated strings. Only the first (at most)
// 'max_len' characters are compared.
inline int compare_strings(const wchar_t* lhs, const wchar_t* rhs,
	size_t max_len)
{
	return ::wcsncmp(lhs, rhs, max_len);
}

// Compares two strings of the specified lengths.
int compare_strings(const wchar_t* lhs, size_t lhs_len,
	const wchar_t* rhs, size_t rhs_len);

// Custom allocator interface.
class allocator
{
protected:
	virtual ~allocator()
	{
	}

public:
	// Allocate a buffer large enough to contain 'size' bytes.
	virtual void* allocate(size_t size) = 0;
};

// Formats a string buffer using a format specification.
// These functions do not append the null character automatically.
// The supplied allocator has the option to perform this task.
// Consequently, the amount of memory requested from the allocator
// does not account for the terminating null character.
string_view format_buffer(allocator* alloc, const char* fmt, ...);
string_view format_buffer_va(allocator* alloc, const char* fmt, va_list ap);
wstring_view format_buffer(allocator* alloc, const wchar_t* fmt, ...);
wstring_view format_buffer_va(allocator* alloc, const wchar_t* fmt, va_list ap);

// Wildcard pattern matching.
bool match_pattern(const char* input, const char* pattern);
bool match_pattern(const char* input, const string_view& pattern);
bool match_pattern(const string_view& input, const char* pattern);
bool match_pattern(const string_view& input, const string_view& pattern);
bool match_pattern(const wchar_t* input, const wchar_t* pattern);
bool match_pattern(const wchar_t* input, const wstring_view& pattern);
bool match_pattern(const wstring_view& input, const wchar_t* pattern);
bool match_pattern(const wstring_view& input, const wstring_view& pattern);

// Compares two version strings.
int compare_versions(const char* version1, const char* version2);

// Converts sequence of wide characters to sequence of multibyte characters.
inline void wchar_to_char(char* result, const wchar_t* source, size_t length)
{
	::wcstombs(result, source, reinterpret_cast<size_t&>(length));
}

// Converts a sequence of multibyte characters to a sequence of wide
// characters.
inline void char_to_wchar(wchar_t* result, const char* source, size_t length)
{
	::mbstowcs(result, source, reinterpret_cast<size_t&>(length));
}

// Template functions for construction, destruction,
// copying and moving of arrays of various objects

// Calls the default constructor of the class 'T'.
template <class T>
inline void construct(T* objects, size_t count)
{
	while (count-- > 0)
		new (objects++) T;
}

template <>
inline void construct(short* dest, size_t count)
{
	memory::zero(dest, count * sizeof(*dest));
}

template <>
inline void construct(int* dest, size_t count)
{
	memory::zero(dest, count * sizeof(*dest));
}

template <>
inline void construct(long* dest, size_t count)
{
	memory::zero(dest, count * sizeof(long));
}

template <>
inline void construct(char* dest, size_t count)
{
	memory::zero(dest, count * sizeof(char));
}

template <>
inline void construct(wchar_t* dest, size_t count)
{
	memory::zero(dest, count * sizeof(*dest));
}

// Calls the copy constructor of the class 'T' (sequence
// initialization with a single value).
template <class T>
inline void construct_identical_copies(T* dest,
	const T& master_copy, size_t count)
{
	while (count-- > 0)
		new (dest++) T(master_copy);
}

template <>
inline void construct_identical_copies(int* dest,
	const int& master_copy, size_t count)
{
	while (count-- > 0)
		*dest++ = master_copy;
}

template <>
inline void construct_identical_copies(long* dest,
	const long& master_copy, size_t count)
{
	while (count-- > 0)
		*dest++ = master_copy;
}

template <>
inline void construct_identical_copies(char* dest,
	const char& master_copy, size_t count)
{
	memory::fill(dest, count, master_copy);
}

// Calls the copy constructor of the class 'T' (initialization
// with objects from the same-sized array).
template <class T>
inline void construct_copies(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		new (dest++) T(*source++);
}

template <>
inline void construct_copies(int* dest, const int* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void construct_copies(long* dest, const long* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void construct_copies(char* dest, const char* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void construct_copies(wchar_t* dest, const wchar_t* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

// Calls the destructors of the 'T' class.
template <class T>
inline void destruct(T* objects, size_t count)
{
	while (count-- > 0)
		(objects++)->~T();
}

template <>
inline void destruct(short*, size_t)
{
}

template <>
inline void destruct(int*, size_t)
{
}

template <>
inline void destruct(long*, size_t)
{
}

template <>
inline void destruct(char*, size_t)
{
}

template <>
inline void destruct(wchar_t*, size_t)
{
}

// Calls T assignment operator (assignment of a single value
// to each element of the array pointed to by 'dest').
template <class T>
inline void assign_value(T* dest, size_t count, const T& value)
{
	while (count-- > 0)
		*dest++ = value;
}

template <>
inline void assign_value(char* dest, size_t count, const char& value)
{
	memory::fill(dest, count, value);
}

template <>
inline void assign_value(wchar_t* dest, size_t count, const wchar_t& value)
{
	while (count-- > 0)
		*dest++ = value;
}

// Calls T assignment operator (element-based array assignment).
template <class T>
inline void assign_pairwise(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		*dest++ = *source++;
}

template <>
inline void assign_pairwise(int* dest, const int* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise(long* dest, const long* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise(char* dest, const char* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise(wchar_t* dest, const wchar_t* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

// Assign elements of one array to elements of another array.
// The arrays are allowed to overlap.
template <class T>
inline void assign_overlapping(T* dest, const T* source, size_t count)
{
	if (dest <= source)
		while (count-- > 0)
			*dest++ = *source++;
	else
		while (count-- > 0)
			dest[count] = source[count];
}

template <>
inline void assign_overlapping(int* dest, const int* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_overlapping(long* dest, const long* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_overlapping(unsigned long* dest,
		const unsigned long* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_overlapping(char* dest, const char* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_overlapping(wchar_t* dest,
	const wchar_t* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

// Repeated calls of the assignment operator (element-based
// overlapping array assignment).
template <class T>
inline void assign_pairwise_backwards(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		dest[count] = source[count];
}

template <>
inline void assign_pairwise_backwards(int* dest,
	const int* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(long* dest,
	const long* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(char* dest,
	const char* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(wchar_t* dest,
	const wchar_t* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

// Moves elements of an array to the right (assuming that there is
// sufficient memory) and inserts the specified range of source elements
// into the freed space.
template <class T>
void displace(T* dst, size_t dst_size, const T* src, size_t src_size)
{
	if (src_size < dst_size)
	{
		construct_copies(dst + dst_size,
			dst + dst_size - src_size, src_size);

		assign_pairwise_backwards(dst + src_size,
			dst, dst_size - src_size);

		assign_pairwise(dst, src, src_size);
	}
	else
	{
		construct_copies(dst + dst_size,
			src + dst_size, src_size - dst_size);

		construct_copies(dst + src_size, dst, dst_size);

		assign_pairwise(dst, src, dst_size);
	}
}

template <class T>
void displace(T* dst, size_t dst_size, const T& value, size_t count)
{
	if (count < dst_size)
	{
		construct_copies(dst + dst_size,
			dst + dst_size - count, count);

		assign_pairwise_backwards(dst + count,
			dst, dst_size - count);

		assign_value(dst, count, value);
	}
	else
	{
		construct_identical_copies(dst + dst_size,
			value, count - dst_size);

		construct_copies(dst + count, dst, dst_size);

		assign_value(dst, dst_size, value);
	}
}

// Base64url_encode() encodes binary data using the base64url variant
// of the Base64 family of encoding schemes.
//
// The function reads its input from src_buf and stores the result in
// dst_buf. This implementation does not pad the result with '='.
//
// Base64url_encode() always returns the number of bytes required to
// encode the given input buffer even if that number is greater than the
// size of the provided output buffer, in which case the output buffer
// is left untouched.
//
// A large input can be encoded incrementally by dividing it into chunks
// and calling base64url_encode() on each chunk. The size of all but the
// last chunk must be divisible by 3.
//
// For information about the base64url encoding, please refer to RFC 4648.
size_t base64url_encode(const void* src_buf, size_t src_size,
	void* dst_buf, size_t dst_size);

// This variant of base64url_encode() encodes a buffer and returns
// the encoded data as a new string.
string base64url_encode(const void* src_buf, size_t src_size);

// This variant of base64url_encode() encodes a string or a string_view
// and returns the encoded data as a new string.
string base64url_encode(const string_view& src);

// Base64url_decode() decodes the base64url-encoded src_buf and stores
// the result in dst_buf.
//
// The function throws a runtime exception if it encounters a character
// in src_buf that is not an alphanumeric character, underscore, or
// dash.  In particular, the padding character ('=') and any whitespace
// characters are not allowed.
//
// Base64url_decode() returns the number of bytes required to decode
// the input buffer regardless of whether the size of the provided output
// buffer is sufficient or not.
//
// When the output buffer is too small to fit the decoded data, the
// output buffer is left untouched.
//
// A large input can be decoded incrementally by dividing it into chunks
// and calling base64url_decode() on each chunk. The size of all but the
// last chunk must be divisible by 4.
//
// For information about the base64url encoding, please refer to RFC 4648.
size_t base64url_decode(const void* src_buf, size_t src_size,
	void* dst_buf, size_t dst_size);

// This variant of base64url_decode() decodes a base64url-encoded buffer
// and returns the decoded data as a new string.
string base64url_decode(const void* src_buf, size_t src_size);

// This variant of base64url_decode() decodes a base64url-encoded string
// or string_view and returns the decoded data as a new string.
string base64url_decode(const string_view& src);

B_END_NAMESPACE

#endif /* !defined(B_MISC_H) */
