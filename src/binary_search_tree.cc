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

void binary_search_tree_base::insert(binary_tree_node* node,
	binary_tree_node* parent, int cmp_result)
{
	++number_of_nodes;

	node->left = node->right = NULL;

	if (parent == NULL)
	{
		node->parent = NULL;
		root = node;
	}
	else
		if (cmp_result < 0)
			parent->left = node;
		else
			if (cmp_result > 0 || parent->right == NULL)
				parent->right = node;
			else
			{
				parent = parent->right;

				while (parent->left != NULL)
					parent = parent->left;

				parent->left = node;
			}

	node->parent = parent;
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

	if (node->right == NULL)
	{
		if (node->left != NULL)
			node->left->parent = node->parent;

		update_parent(&root, node->parent, node, node->left);

		return;
	}

	if (node->left == NULL)
	{
		node->right->parent = node->parent;

		update_parent(&root, node->parent, node, node->right);

		return;
	}

	if (node->left->right == NULL)
	{
		node->left->right = node->right;
		node->right->parent = node->left;
		node->left->parent = node->parent;

		update_parent(&root, node->parent, node, node->left);

		return;
	}

	if (node->right->left == NULL)
	{
		node->right->left = node->left;
		node->left->parent = node->right;
		node->right->parent = node->parent;

		update_parent(&root, node->parent, node, node->right);

		return;
	}

	binary_tree_node* next_node;
	binary_tree_node* next_node_parent = node->right;

	for (;;)
	{
		next_node = next_node_parent->left;

		if (next_node->left == NULL)
			break;

		next_node_parent = next_node;
	}

	if ((next_node_parent->left = next_node->right) != NULL)
		next_node->right->parent = next_node_parent;

	(next_node->left = node->left)->parent = next_node;
	(next_node->right = node->right)->parent = next_node;
	next_node->parent = node->parent;

	update_parent(&root, node->parent, node, next_node);
}

B_END_NAMESPACE
