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

#include "SystemException.h"

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
	// Insufficient memory condition. An exception of this type
	// is thrown as a result of any memory allocation failure
	// in the library.
	class Exception : public SystemException
	{
#ifdef B_DUMB_MALLOC
	// Construction
	public:
		Exception();
#endif // B_DUMB_MALLOC
	};

	// Allocates <size> number of bytes and returns a pointer to
	// the allocated memory block.
	// Throws Memory::Exception if an out-of-memory condition occurs.
	static void* Alloc(int size)
		throw (Exception);

	// Frees a memory block allocated by a previous call to
	// the Alloc() method.
	static void Free(void* block);

	// Allocates a chunk of memory of the appropriate size
	// from the internal list of free chunks. If no such chunk
	// is found, the method allocates a new one from the heap.
	// Throws Memory::Exception if an out-of-memory condition occurs.
	static void* FixedAlloc(int size)
		throw (Exception);

	// Places a chunk of memory allocated by a previous call
	// to the FixedAlloc() method back to the list of free chunks.
	static void FixedFree(void* chunk, int size);

	// Fills <length> bytes of memory pointed to by <block> with the
	// specified byte value.
	static void Fill(void* block, int length, char filler);

	// Fills a block of memory with zeros.
	static void Zero(void* block, int length);

	// Copies a block of memory from one location to another.
	// If the source and destination blocks overlap, the result is
	// undefined. For overlapped blocks, use Move().
	static void Copy(void* destination, const void* source, int length);

	// Moves a block of memory from one location to another.
	// The source and destination blocks may overlap.
	static void Move(void* destination, const void* source, int length);

	// Compares two same-sized memory blocks.
	static int Compare(const void* block1, const void* block2, int length);

	// Aligns an integer value on the <alignment> boundary.
	// The boundary must be given as a power of two.
	static size_t Align(size_t value, size_t alignment);

	// Aligns a pointer on the <alignment> boundary.
	// The boundary must be given as a power of two.
	static void* Align(void* value, size_t alignment);
};

#ifdef B_SET_MALLOC_ERROR
inline Memory::Exception::Exception() : SystemException(ENOMEM)
{
}
#endif // B_SET_MALLOC_ERROR

inline void Memory::Free(void* block)
{
#ifdef B_USE_WIN32_API
	::HeapFree(::GetProcessHeap(), 0, block);
#else
	free(block);
#endif // B_USE_WIN32_API
}

inline void Memory::Fill(void* block, int length, char filler)
{
#ifdef B_USE_WIN32_API
	::FillMemory(block, length, (BYTE) filler);
#else
	memset(block, filler, length);
#endif // B_USE_WIN32_API
}

inline void Memory::Zero(void* block, int length)
{
#ifdef B_USE_WIN32_API
	::ZeroMemory(block, length);
#else
	memset(block, 0, length);
#endif // B_USE_WIN32_API
}

inline void Memory::Copy(void* destination, const void* source, int length)
{
#ifdef B_USE_WIN32_API
	::CopyMemory(destination, source, length);
#else
	memcpy(destination, source, length);
#endif // B_USE_WIN32_API
}

inline void Memory::Move(void* destination, const void* source, int length)
{
#ifdef B_USE_WIN32_API
	::MoveMemory(destination, source, length);
#else
	memmove(destination, source, length);
#endif // B_USE_WIN32_API
}

inline int Memory::Compare(const void* block1,
	const void* block2, int length)
{
	return memcmp(block1, block2, length);
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

#include "Helpers.h"

#endif // !defined(B_MEMORY_H)
