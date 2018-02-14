/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2018 Damon Revoe
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

#include <b/red_black_tree.h>

#include "unit_test.h"

struct element : public b::red_black_tree_node
{
	element(int val) : value(val)
	{
	}

	const int value;
};

static int value_for_node(const b::red_black_tree_node* node)
{
	return static_cast<const element*>(node)->value;
}

B_TEST_CASE(construction)
{
	b::red_black_tree<int (*)(const b::red_black_tree_node*)> rbt =
		value_for_node;

	B_REQUIRE(rbt.number_of_nodes == 0);
}
