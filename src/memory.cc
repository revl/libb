// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/memory.h>

B_BEGIN_NAMESPACE

void* memory::alloc(size_t size)
{
	B_STRING_LITERAL(method_name, "b::memory::alloc()");

	void* block;

	if ((block = malloc(size)) == NULL)
		throw system_exception(method_name, ENOMEM);

	return block;
}

void* memory::fixed_alloc(size_t size)
{
	return alloc(size);
}

void memory::fixed_free(void* chunk, size_t /*size*/)
{
	free(chunk);
}

B_END_NAMESPACE
