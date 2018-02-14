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

const binary_tree_node* binary_tree_node::prev() const
{
	if (left != NULL)
	{
		const binary_tree_node* prev_node = left;

		// Find the rightmost child in the left branch.
		while (prev_node->right != NULL)
			prev_node = prev_node->right;

		return prev_node;
	}

	// Find the first parent that has the current
	// subtree as its right branch.
	const binary_tree_node* prev_node = parent;
	const binary_tree_node* child = this;

	while (prev_node != NULL && prev_node->left == child)
	{
		child = prev_node;
		prev_node = prev_node->parent;
	}

	return prev_node;
}

void binary_search_tree_base::insert_after_search(binary_tree_node* node,
	binary_tree_node* parent, int cmp_result)
{
	if (parent == NULL)
		rightmost = leftmost = root = node;
	else
		if (cmp_result < 0)
		{
			parent->left = node;

			if (leftmost == parent)
				leftmost = node;
		}
		else
			if (cmp_result > 0 || parent->right == NULL)
			{
				parent->right = node;

				if (rightmost == parent)
					rightmost = node;
			}
			else
			{
				parent = parent->right;

				while (parent->left != NULL)
					parent = parent->left;

				parent->left = node;
			}

	node->parent = parent;
	node->left = node->right = NULL;

	++number_of_nodes;
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
	B_ASSERT(root != NULL);

	--number_of_nodes;

	if (node->right == NULL)
	{
		if (node->left != NULL)
		{
			if (rightmost == node)
			{
				rightmost = node->left;

				while (rightmost->right != NULL)
					rightmost = rightmost->right;
			}

			node->left->parent = node->parent;

			update_parent(&root, node->parent, node, node->left);

			return;
		}

		if (node->parent == NULL)
			rightmost = leftmost = root = NULL;
		else
			if (node->parent->left == node)
			{
				if (leftmost == node)
					leftmost = node->parent;

				node->parent->left = NULL;
			}
			else
			{
				if (rightmost == node)
					rightmost = node->parent;

				node->parent->right = NULL;
			}

		return;
	}

	if (node->left == NULL)
	{
		if (leftmost == node)
		{
			leftmost = node->right;

			while (leftmost->left != NULL)
				leftmost = leftmost->left;
		}

		node->right->parent = node->parent;

		update_parent(&root, node->parent, node, node->right);

		return;
	}

	if (node->left->right == NULL)
	{
		if (rightmost == node)
			rightmost = node->left;

		node->left->right = node->right;
		node->right->parent = node->left;
		node->left->parent = node->parent;

		update_parent(&root, node->parent, node, node->left);

		return;
	}

	if (node->right->left == NULL)
	{
		if (leftmost == node)
			leftmost = node->right;

		node->right->left = node->left;
		node->left->parent = node->right;
		node->right->parent = node->parent;

		update_parent(&root, node->parent, node, node->right);

		return;
	}

	// Substitute the inorder successor for the deleted node.
	binary_tree_node* next_node = node->right->left;

	while (next_node->left != NULL)
		next_node = next_node->left;

	if ((next_node->parent->left = next_node->right) != NULL)
		next_node->right->parent = next_node->parent;

	(next_node->left = node->left)->parent = next_node;
	(next_node->right = node->right)->parent = next_node;
	next_node->parent = node->parent;

	update_parent(&root, node->parent, node, next_node);
}

B_END_NAMESPACE
