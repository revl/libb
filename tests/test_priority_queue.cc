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

#include <b/priority_queue.h>

#include "unit_test.h"

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

	B_CHECK(pq.size() == 3);

	pq.push(40);
	pq.push(70);

	B_CHECK(pq.pop() == 70);
	B_CHECK(pq.pop() == 60);
	B_CHECK(pq.pop() == 50);
	B_CHECK(pq.pop() == 40);
	B_CHECK(pq.pop() == 10);

	B_CHECK(pq.is_empty());
}
