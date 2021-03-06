// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_MEMORY_H
#define B_MEMORY_H

#include "host.h"

B_BEGIN_NAMESPACE

// The minimal size of a memory block that memory::fixed_alloc()
// can allocate.
#define B_MIN_FIXED_ALLOC (sizeof(void*) * 2)

// Utility class for memory management. This class comprises
// system-independent wrappers around common memory handling
// routines, and also provides an effective technique for
// allocating small objects from the linked list of reusable
// fixed-sized chunks, which offers increase in allocation
// speed, as well as reduces heap fragmentation.
class memory
{
public:
	// Allocates 'size' number of bytes and returns a pointer to
	// the allocated memory block.
	// Throws 'system_exception' if an out-of-memory condition occurs.
	static void* alloc(size_t size);

	// Frees a memory block allocated by a previous call to
	// the alloc() method.
	static void free(void* block);

	// Allocates a chunk of memory of the appropriate size
	// from the internal list of free chunks. If no such chunk
	// is found, the method allocates a new one from the heap.
	// Throws 'system_exception' if an out-of-memory condition occurs.
	static void* fixed_alloc(size_t size);

	// Places a chunk of memory allocated by a previous call
	// to the fixed_alloc() method back to the list of free chunks.
	static void fixed_free(void* chunk, size_t size);

	// Fills 'length' bytes of memory pointed to by 'block' with the
	// specified byte value.
	static void fill(void* block, size_t length, char filler);

	// Fills 'length' wide characters with the specified value.
	static void fill_wchar_t(wchar_t* wcs, size_t length, wchar_t value);

	// Fills a block of memory with zeros.
	static void zero(void* block, size_t length);

	// Copies a block of memory from one location to another.
	// If the source and destination blocks overlap, the result is
	// undefined. For overlapped blocks, use move().
	static void copy(void* destination, const void* source, size_t length);

	// Moves a block of memory from one location to another.
	// The source and destination blocks may overlap.
	static void move(void* destination, const void* source, size_t length);

	// Compares two same-sized memory blocks.
	static int compare(const void* p1, const void* p2, size_t length);

	// Aligns an integer value on the 'alignment' boundary.
	// The boundary must be given as a power of two.
	static size_t align(size_t value, size_t alignment);

	// Aligns a pointer on the 'alignment' boundary.
	// The boundary must be given as a power of two.
	static void* align(void* value, size_t alignment);
};

inline void memory::free(void* block)
{
	::free(block);
}

inline void memory::fill(void* block, size_t length, char filler)
{
	::memset(block, filler, length);
}

inline void memory::fill_wchar_t(wchar_t* wcs, size_t length, wchar_t value)
{
	::wmemset(wcs, value, length);
}

inline void memory::zero(void* block, size_t length)
{
	::memset(block, 0, length);
}

inline void memory::copy(void* destination, const void* source, size_t length)
{
	::memcpy(destination, source, length);
}

inline void memory::move(void* destination, const void* source, size_t length)
{
	::memmove(destination, source, length);
}

inline int memory::compare(const void* p1, const void* p2, size_t length)
{
	return ::memcmp(p1, p2, length);
}

inline size_t memory::align(size_t value, size_t alignment)
{
	--alignment;

	return (value + alignment) & ~alignment;
}

inline void* memory::align(void* value, size_t alignment)
{
	return (void*) align((size_t) value, alignment);
}

B_END_NAMESPACE

#include "system_exception.h"

#endif /* !defined(B_MEMORY_H) */
