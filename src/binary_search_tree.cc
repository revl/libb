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

B_BEGIN_NAMESPACE

const binary_tree_node* binary_tree_node::next() const
{
	if (right != NULL)
	{
		const binary_tree_node* next_node = right;

		// Find the leftmost child in the right branch.
		while (next_node->left != NULL)
			next_node = next_node->left;

		return next_node;
	}

	// Find the first parent that has the current
	// subtree as its left branch.
	const binary_tree_node* next_node = parent;
	const binary_tree_node* child = this;

	while (next_node != NULL && next_node->right == child)
	{
		child = next_node;
		next_node = next_node->parent;
	}

	return next_node;
}

static void update_parent(binary_tree_node** root, binary_tree_node* parent,
	binary_tree_node* old_child, binary_tree_node* new_child)
{
	if (parent == NULL)
		*root = new_child;
	else
		if (parent->left == old_child)
			parent->left = new_child;
		else
			parent->right = new_child;
}

void binary_search_tree_base::remove(binary_tree_node* node)
{
	B_ASSERT(!is_empty());

	--number_of_nodes;

	binary_tree_node* const parent_node = node->parent;
	binary_tree_node* const left_node = node->left;
	binary_tree_node* const right_node = node->right;

	if (right_node == NULL)
	{
		if (left_node != NULL)
			left_node->parent = parent_node;

		update_parent(&root, parent_node, node, left_node);

		return;
	}

	if (left_node == NULL)
	{
		right_node->parent = parent_node;

		update_parent(&root, parent_node, node, right_node);

		return;
	}

	if (left_node->right == NULL)
	{
		left_node->right = right_node;
		right_node->parent = left_node;
		left_node->parent = parent_node;

		update_parent(&root, parent_node, node, left_node);

		return;
	}

	if (right_node->left == NULL)
	{
		right_node->left = left_node;
		left_node->parent = right_node;
		right_node->parent = parent_node;

		update_parent(&root, parent_node, node, right_node);

		return;
	}

	binary_tree_node* successor_parent = right_node;
	binary_tree_node* successor;

	for (;;)
	{
		successor = successor_parent->left;

		if (successor->left == NULL)
			break;

		successor_parent = successor;
	}

	if ((successor_parent->left = successor->right) != NULL)
		successor->right->parent = successor_parent;

	(successor->left = left_node)->parent = successor;
	(successor->right = right_node)->parent = successor;
	successor->parent = parent_node;

	update_parent(&root, parent_node, node, successor);
}

B_END_NAMESPACE
