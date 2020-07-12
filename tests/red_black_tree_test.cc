// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/red_black_tree.h>

#include "test_case.h"

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
