// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_PRIORITY_QUEUE_H
#define B_PRIORITY_QUEUE_H

#include "array.h"
#include "heap.h"

B_BEGIN_NAMESPACE

// Priority queue container.
template <class T>
class priority_queue
{
public:
	// Adds a value to this priority queue.
	void push(const T& element);

	// Removes the element with the greatest value from the priority queue
	// and returns it.
	T pop();

	// Returns the number of elements in this priority queue.
	size_t length() const;

	// Returns true if the priority queue is empty.
	bool is_empty() const;

protected:
	array<T> data;
};

template <class T>
void priority_queue<T>::push(const T& element)
{
	data.append(element);
	push_into_heap(data.lock(), data.length());
	data.unlock();
}

template <class T>
T priority_queue<T>::pop()
{
	size_t data_size = data.length();

	pop_from_heap(data.lock(), data_size);
	data.unlock();

	T result(data[--data_size]);

	data.remove(data_size);

	return result;
}

template <class T>
size_t priority_queue<T>::length() const
{
	return data.length();
}

template <class T>
bool priority_queue<T>::is_empty() const
{
	return data.is_empty();
}

B_END_NAMESPACE

#endif /* !defined(B_PRIORITY_QUEUE_H) */
