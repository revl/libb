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

#include <b/binary_tree.h>

#include "unit_test.h"

struct element : public b::binary_tree_node
{
	element(int val) : value(val)
	{
	}

	const int value;
};

struct value_is_less
{
	static const element& element_for_node(const b::binary_tree_node& node)
	{
		return static_cast<const element&>(node);
	}

	bool operator ()(const b::binary_tree_node& lhs,
		const b::binary_tree_node& rhs) const
	{
		return element_for_node(lhs).value <
			element_for_node(rhs).value;
	}

	bool operator ()(const b::binary_tree_node& node, int val) const
	{
		return element_for_node(node).value < val;
	}

	bool operator ()(int val, const b::binary_tree_node& node) const
	{
		return val < element_for_node(node).value;
	}
};

B_TEST_CASE(construction)
{
	b::binary_search_tree<value_is_less> bst = value_is_less();

	int cmp_result;

	B_CHECK(bst.search(0, &cmp_result) == NULL);

	element el10(10);

	bst.insert(&el10);

	B_REQUIRE(bst.size() == 1);

	b::binary_tree_node* found_el10 = bst.search(10, &cmp_result);

	B_CHECK(cmp_result == 0);
	B_CHECK(found_el10 == &el10);
}
