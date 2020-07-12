// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/binary_tree.h>

#include <b/array.h>
#include <b/pseudorandom.h>

#include "test_case.h"

struct element : public b::binary_tree_node
{
	element(int val) : value(val)
	{
	}

	const int value;
};

static int value_for_node(const b::binary_tree_node* node)
{
	return static_cast<const element*>(node)->value;
}

B_TEST_CASE(construction)
{
	b::binary_search_tree<int (*)(const b::binary_tree_node*)> bst =
		value_for_node;

	int cmp_result;

	B_CHECK(bst.find(0) == NULL);
	B_CHECK(bst.search(0, &cmp_result) == NULL);
	B_CHECK(cmp_result == 0);

	element el10(10);

	bst.insert(&el10);

	B_REQUIRE(bst.number_of_nodes == 1);

	b::binary_tree_node* found_el10 = bst.find(10);

	B_CHECK(found_el10 == &el10);

	found_el10 = bst.search(10, &cmp_result);

	B_CHECK(found_el10 == &el10);
	B_CHECK(cmp_result == 0);

	B_CHECK(bst.find(20) == NULL);
}

struct key_op
{
	int operator ()(const b::binary_tree_node* node) const
	{
		return value_for_node(node);
	}
};

B_TEST_CASE(inorder_walk)
{
	b::binary_search_tree<key_op> bst = key_op();

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

	B_REQUIRE(bst.number_of_nodes == 6);

	int prev_value = -1;

	const b::binary_tree_node* node = bst.leftmost;

	size_t actual_number_of_elements = 0;

	while (node != NULL)
	{
		++actual_number_of_elements;

		int value = value_for_node(node);

		B_CHECK(prev_value <= value);

		prev_value = value;

		node = node->next();
	}

	B_CHECK(actual_number_of_elements == 6);

	++prev_value;

	node = bst.rightmost;

	while (node != NULL)
	{
		--actual_number_of_elements;

		int value = value_for_node(node);

		B_CHECK(prev_value >= value);

		prev_value = value;

		node = node->prev();
	}

	B_CHECK(actual_number_of_elements == 0);
}

#define NUMBER_OF_ELEMENTS 100

B_TEST_CASE(random_elements)
{
	b::binary_search_tree<key_op> bst = key_op();

	b::array<int> numbers;

	for (int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
		numbers.append(i);

	b::pseudorandom prg(10);

	for (int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
	{
		size_t random_index = prg.next(numbers.length());

		bst.insert(new element(numbers[random_index]));

		numbers.remove(random_index);
	}

	B_REQUIRE(numbers.is_empty());

	for (int i = 0; i < NUMBER_OF_ELEMENTS; ++i)
		numbers.append(i);

	size_t expected_size = NUMBER_OF_ELEMENTS;

	b::binary_tree_node* node;

	do
	{
		{
			B_REQUIRE(bst.number_of_nodes == expected_size);

			node = bst.leftmost;

			size_t i = 0;

			do
			{
				B_REQUIRE(i < numbers.length());

				B_CHECK(numbers[i++] == value_for_node(node));
			}
			while ((node = node->next()) != NULL);

			B_REQUIRE(i == expected_size);

			node = bst.rightmost;

			do
				B_CHECK(numbers[--i] == value_for_node(node));
			while ((node = node->prev()) != NULL);

			B_REQUIRE(i == 0);
		}

		size_t random_index = prg.next(numbers.length());
		int number = numbers[random_index];

		int cmp_result;

		node = bst.search(number + NUMBER_OF_ELEMENTS, &cmp_result);
		B_CHECK(node != NULL && cmp_result > 0);
		B_CHECK(bst.find(number + NUMBER_OF_ELEMENTS) == NULL);

		node = bst.search(number - NUMBER_OF_ELEMENTS, &cmp_result);
		B_CHECK(node != NULL && cmp_result < 0);
		B_CHECK(bst.find(number - NUMBER_OF_ELEMENTS) == NULL);

		node = bst.search(number, &cmp_result);

		B_REQUIRE(node != NULL && cmp_result == 0);
		B_REQUIRE(value_for_node(node) == number);

		bst.remove(node);
		delete static_cast<element*>(node);

		numbers.remove(random_index);
	}
	while (--expected_size != 0);
}
