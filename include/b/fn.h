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

// Compares two objects. Returns zero, if objects are equal;
// -1, if the first object is less, and 1, if the second is less.
template <class T>
inline int compare(const T& object1, const T& object2)
{
	return object1 < object2 ? -1 : object2 < object1;
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
inline void construct_default(T* objects, size_t count)
{
	while (count-- > 0)
		new (objects++) T;
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

// Calls the copy constructor of the class 'T' (initialization
// with objects from the same-sized array).
template <class T>
inline void construct_copies(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		new (dest++) T(*source++);
}

// Calls the destructors of the 'T' class.
template <class T>
inline void destruct(T* objects, size_t count)
{
	while (count-- > 0)
		(objects++)->~T();
}

// Calls T assignment operator (assignment of a single value
// to each element of the array pointed to by 'dest').
template <class T>
inline void assign_value(T* dest, size_t count, const T& value)
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

// Repeated calls of the assignment operator (element-based
// overlapping array assignment).
template <class T>
inline void assign_pairwise_backwards(T* dest, const T* source, size_t count)
{
	while (count-- > 0)
		dest[count] = source[count];
}

// Template specializations

// int

template <>
inline int compare(const int& n1, const int& n2)
{
	return n1 - n2;
}

template <>
inline void construct_default(int* dest, size_t count)
{
	memory::zero(dest, count * sizeof(*dest));
}

template <>
inline void construct_identical_copies(int* dest,
	const int& master_copy, size_t count)
{
	while (count-- > 0)
		*dest++ = master_copy;
}

template <>
inline void construct_copies(int* dest, const int* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(int*, size_t)
{
}

template <>
inline void assign_pairwise(int* dest, const int* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(int* dest,
	const int* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

// long

template <>
inline int compare(const long& l1, const long& l2)
{
	return int(l1 - l2);
}

template <>
inline void construct_default(long* dest, size_t count)
{
	memory::zero(dest, count * sizeof(long));
}

template <>
inline void construct_identical_copies(long* dest,
	const long& master_copy, size_t count)
{
	while (count-- > 0)
		*dest++ = master_copy;
}

template <>
inline void construct_copies(long* dest, const long* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(long*, size_t)
{
}

template <>
inline void assign_pairwise(long* dest, const long* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(long* dest,
	const long* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

// char

template <>
inline int compare(const char& c1, const char& c2)
{
	return int(c1) - int(c2);
}

template <>
inline int compare_arrays(const char* array1, const char* array2, size_t count)
{
	return memory::compare(array1, array2, count);
}

template <>
inline void construct_default(char* dest, size_t count)
{
	memory::zero(dest, count * sizeof(char));
}

template <>
inline void construct_identical_copies(char* dest,
	const char& master_copy, size_t count)
{
	memory::fill(dest, count, master_copy);
}

template <>
inline void construct_copies(char* dest, const char* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(char*, size_t)
{
}

template <>
inline void assign_value(char* dest, size_t count, const char& value)
{
	memory::fill(dest, count, value);
}

template <>
inline void assign_pairwise(char* dest, const char* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(char* dest,
	const char* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

// wchar_t

template <>
inline int compare(const wchar_t& c1, const wchar_t& c2)
{
	return int(c1) - int(c2);
}

template <>
inline void construct_default(wchar_t* dest, size_t count)
{
	memory::zero(dest, count * sizeof(*dest));
}

template <>
inline void construct_copies(wchar_t* dest, const wchar_t* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void destruct(wchar_t*, size_t)
{
}

template <>
inline void assign_value(wchar_t* dest, size_t count, const wchar_t& value)
{
	while (count-- > 0)
		*dest++ = value;
}

template <>
inline void assign_pairwise(wchar_t* dest, const wchar_t* source, size_t count)
{
	memory::copy(dest, source, count * sizeof(*dest));
}

template <>
inline void assign_pairwise_backwards(wchar_t* dest,
	const wchar_t* source, size_t count)
{
	memory::move(dest, source, count * sizeof(*dest));
}

size_t base64url_encode(const void* src_buf, size_t src_size,
	void* dst_buf, size_t dst_size);


size_t base64url_decode(const void* src_buf, size_t src_size,
	void* dst_buf, size_t dst_size);

B_END_NAMESPACE

#endif /* !defined(B_MISC_H) */
