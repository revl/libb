// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/priority_queue.h>

#include "test_case.h"

B_TEST_CASE(push_pop)
{
	b::priority_queue<int> pq;

	pq.push(20);
	pq.push(10);

	B_CHECK(pq.pop() == 20);

	pq.push(30);

	B_CHECK(pq.pop() == 30);

	pq.push(60);
	pq.push(50);

	B_CHECK(pq.length() == 3);

	pq.push(40);
	pq.push(70);

	B_CHECK(pq.pop() == 70);
	B_CHECK(pq.pop() == 60);
	B_CHECK(pq.pop() == 50);
	B_CHECK(pq.pop() == 40);
	B_CHECK(pq.pop() == 10);

	B_CHECK(pq.is_empty());
}
