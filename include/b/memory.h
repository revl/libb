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

#ifndef B_MEMORY_H
#define B_MEMORY_H

#include "host.h"

B_BEGIN_NAMESPACE

// The minimal size of a memory block that Memory::FixedAlloc()
// can allocate.
#define B_MIN_FIXED_ALLOC (sizeof(void*) * 2)

// Utility class for memory management. This class comprises
// system-independent wrappers around common memory handling
// routines, and also provides an effective technique for
// allocating small objects from the linked list of reusable
// fixed-sized chunks, which offers increase in allocation
// speed, as well as reduces heap fragmentation.
class Memory
{
public:
	// Allocates <size> number of bytes and returns a pointer to
	// the allocated memory block.
	// Throws 'system_exception' if an out-of-memory condition occurs.
	static void* Alloc(size_t size);

	// Frees a memory block allocated by a previous call to
	// the Alloc() method.
	static void Free(void* block);

	// Allocates a chunk of memory of the appropriate size
	// from the internal list of free chunks. If no such chunk
	// is found, the method allocates a new one from the heap.
	// Throws 'system_exception' if an out-of-memory condition occurs.
	static void* FixedAlloc(size_t size);

	// Places a chunk of memory allocated by a previous call
	// to the FixedAlloc() method back to the list of free chunks.
	static void FixedFree(void* chunk, size_t size);

	// Fills <length> bytes of memory pointed to by <block> with the
	// specified byte value.
	static void Fill(void* block, size_t length, char filler);

	// Fills a block of memory with zeros.
	static void Zero(void* block, size_t length);

	// Copies a block of memory from one location to another.
	// If the source and destination blocks overlap, the result is
	// undefined. For overlapped blocks, use Move().
	static void Copy(void* destination, const void* source, size_t length);

	// Moves a block of memory from one location to another.
	// The source and destination blocks may overlap.
	static void Move(void* destination, const void* source, size_t length);

	// Compares two same-sized memory blocks.
	static int Compare(const void* p1, const void* p2, size_t length);

	// Aligns an integer value on the <alignment> boundary.
	// The boundary must be given as a power of two.
	static size_t Align(size_t value, size_t alignment);

	// Aligns a pointer on the <alignment> boundary.
	// The boundary must be given as a power of two.
	static void* Align(void* value, size_t alignment);
};

inline void Memory::Free(void* block)
{
	free(block);
}

inline void Memory::Fill(void* block, size_t length, char filler)
{
	memset(block, filler, length);
}

inline void Memory::Zero(void* block, size_t length)
{
	memset(block, 0, length);
}

inline void Memory::Copy(void* destination, const void* source, size_t length)
{
	memcpy(destination, source, length);
}

inline void Memory::Move(void* destination, const void* source, size_t length)
{
	memmove(destination, source, length);
}

inline int Memory::Compare(const void* p1, const void* p2, size_t length)
{
	return memcmp(p1, p2, length);
}

inline size_t Memory::Align(size_t value, size_t alignment)
{
	--alignment;

	return (value + alignment) & ~alignment;
}

inline void* Memory::Align(void* value, size_t alignment)
{
	return (void*) Align((size_t) value, alignment);
}

B_END_NAMESPACE

#include "system_exception.h"

#endif /* !defined(B_MEMORY_H) */
