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

static void update_parent(binary_tree_node** root, binary_tree_node* parent,
	binary_tree_node* old_child, binary_tree_node* new_child)
{
	if (parent == NULL)
		*root = new_child;
	else
		if (parent->left == old_child)
			parent->left = new_child;
		else
		{
			B_ASSERT(parent->right == old_child);

			parent->right = new_child;
		}
}

void binary_search_tree_base::remove(binary_tree_node* node)
{
	B_ASSERT(!is_empty());

	--number_of_nodes;

	binary_tree_node* const parent_node = node->parent;
	binary_tree_node* const left_node = node->left;
	binary_tree_node* const right_node = node->right;

	if (left_node == NULL)
	{
		if (right_node != NULL)
			right_node->parent = parent_node;

		update_parent(&root, parent_node, node, right_node);

		return;
	}

	if (right_node == NULL)
	{
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

	if (left_node->right == NULL)
	{
		left_node->right = right_node;
		right_node->parent = left_node;
		left_node->parent = parent_node;

		update_parent(&root, parent_node, node, left_node);

		return;
	}

	binary_tree_node* successor = right_node->left;

	while (successor->left != NULL)
		successor = successor->left;

	successor->left = left_node;
	left_node->parent = successor;
	right_node->parent = parent_node;

	update_parent(&root, parent_node, node, right_node);
}

B_END_NAMESPACE
