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

#include <B/Heap.h>

// My heap
static B::IntHeap my_heap;

// stl heap
static std::vector<int> stl_heap;

static void CompareHeaps()
{
	assert(my_heap.GetSize() == (int) stl_heap.size());
	if (!my_heap.IsEmpty())
		assert(my_heap[0] == stl_heap[0]);
}

int main(int /*argc*/, char* /*argv*/[])
{
	int i;
	int n;

	B::randomize();

	for (i = 0; i < 100; i++)
	{
		n = B::rand();

		my_heap.Push(n);

		stl_heap.push_back(n);
		std::push_heap(stl_heap.begin(), stl_heap.end());

		CompareHeaps();
	}

	for (i = 0; i < 100; i++)
	{
		my_heap.Pop(n);

		std::pop_heap(stl_heap.begin(), stl_heap.end());

		assert(stl_heap.back() == n);

		stl_heap.pop_back();

		CompareHeaps();
	}

	assert(my_heap.IsEmpty());

	int data[10];
#define COUNT (sizeof(data) / sizeof(*data))

	for (i = 0; i < COUNT; i++)
		printf("%d\t", data[i] = B::rand());
	printf("\n");

	printf("Testing my HeapSort...\n");
	B::IntHeap::Sort(data, COUNT);

	for (i = 0; i < COUNT; i++)
		printf("%d\t", data[i]);
	printf("\n");

	return 0;
}
