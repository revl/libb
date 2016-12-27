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

#include <b/array.h>
#include <b/pseudorandom.h>

#include "unit_test.h"

struct element : public b::binary_tree_node
{
	element(int val) : value(val)
	{
	}

	const int value;
};

static int value_for_node(const b::binary_tree_node& node)
{
	return static_cast<const element&>(node).value;
}

struct value_is_less
{
	bool operator ()(const b::binary_tree_node& lhs,
		const b::binary_tree_node& rhs) const
	{
		return value_for_node(lhs) < value_for_node(rhs);
	}

	bool operator ()(const b::binary_tree_node& node, int val) const
	{
		return value_for_node(node) < val;
	}

	bool operator ()(int val, const b::binary_tree_node& node) const
	{
		return val < value_for_node(node);
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

B_TEST_CASE(inorder_walk)
{
	b::binary_search_tree<value_is_less> bst = value_is_less();

	element el20(20);
	bst.insert(&el20);

	element el40(40);
	bst.insert(&el40);

	element el20_again(20);
	bst.insert(&el20_again);

	element el30(30);
	bst.insert(&el30);

	element el10(10);
	bst.insert(&el10);

	element el20_again_and_again(20);
	bst.insert(&el20_again_and_again);

	B_REQUIRE(bst.size() == 6);

	int prev_value = -1;

	const b::binary_tree_node* node = bst.first();

	size_t actual_number_of_elements = 0;

	while (node != NULL)
	{
		++actual_number_of_elements;

		int value = value_for_node(*node);

		B_CHECK(prev_value <= value);

		prev_value = value;

		node = node->next();
	}

	B_CHECK(actual_number_of_elements == 6);

	++prev_value;

	node = bst.last();

	while (node != NULL)
	{
		--actual_number_of_elements;

		int value = value_for_node(*node);

		B_CHECK(prev_value >= value);

		prev_value = value;

		node = node->prev();
	}

	B_CHECK(actual_number_of_elements == 0);
}

#define NUMBER_OF_ELEMENTS 100

B_TEST_CASE(deletion)
{
	b::binary_search_tree<value_is_less> bst = value_is_less();

	b::array<int> numbers;

	for (int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
		numbers.append(1, i);

	b::pseudorandom prg(10);

	for (int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
	{
		b::pseudorandom::value_type random_index =
			prg.next((b::pseudorandom::value_type) numbers.size());

		bst.insert(new element(numbers[random_index]));

		numbers.remove(random_index);
	}

	B_REQUIRE(numbers.is_empty());

	for (int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
		numbers.append(1, i);

	size_t expected_size = NUMBER_OF_ELEMENTS;

	do
	{
		{
			B_REQUIRE(bst.size() == expected_size);

			const b::binary_tree_node* node = bst.first();

			size_t i = 0;

			do
			{
				B_REQUIRE(i < numbers.size());

				B_CHECK(numbers[i++] == value_for_node(*node));
			}
			while ((node = node->next()) != NULL);

			B_REQUIRE(i == expected_size);

			node = bst.last();

			do
				B_CHECK(numbers[--i] == value_for_node(*node));
			while ((node = node->prev()) != NULL);

			B_REQUIRE(i == 0);
		}

		b::pseudorandom::value_type random_index =
			prg.next((b::pseudorandom::value_type) numbers.size());
		int number = numbers[random_index];

		int cmp_result;

		b::binary_tree_node* node = bst.search(number, &cmp_result);

		B_REQUIRE(node != NULL);
		B_REQUIRE(value_for_node(*node) == number);

		bst.remove(node);
		delete static_cast<element*>(node);

		numbers.remove(random_index);
	}
	while (--expected_size != 0);
}
