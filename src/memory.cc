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

#include <b/memory.h>

B_BEGIN_NAMESPACE

void* Memory::Alloc(size_t size)
{
	B_STATIC_CONST_STRING(method_name, "b::memory::alloc()");

	register void* block;

	if ((block = malloc(size)) == NULL)
		throw system_exception(method_name, ENOMEM);

	return block;
}

void* Memory::FixedAlloc(size_t size)
{
	return Alloc(size);
}

void Memory::FixedFree(void* chunk, size_t /*size*/)
{
	Free(chunk);
}

B_END_NAMESPACE
