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

#ifndef B_BINARY_TREE_H
#define B_BINARY_TREE_H

#include "host.h"

B_BEGIN_NAMESPACE

struct binary_tree_node
{
	binary_tree_node* parent;
	binary_tree_node* left;
	binary_tree_node* right;

	void replace_child(binary_tree_node* child, binary_tree_node* new_child)
	{
		if (left == child)
			left = new_child;
		else
		{
			B_ASSERT(right == child);

			right = new_child;
		}
	}
};

class binary_search_tree_base
{
public:
	binary_search_tree_base() : root(NULL), number_of_nodes(0)
	{
	}

	size_t size() const
	{
		return number_of_nodes;
	}

	void remove(binary_tree_node* node)
	{
		binary_tree_node* const parent_node = node->parent;
		binary_tree_node* const left_node = node->left;
		binary_tree_node* const right_node = node->right;

		if (left_node == NULL)
		{
			if (right_node != NULL)
				right_node->parent = parent_node;

			update_parent(parent_node, node, right_node);

			return;
		}

		if (right_node == NULL)
		{
			left_node->parent = parent_node;

			update_parent(parent_node, node, left_node);

			return;
		}

		if (right_node->left == NULL)
		{
			right_node->left = left_node;
			left_node->parent = right_node;
			right_node->parent = parent_node;

			update_parent(parent_node, node, right_node);

			return;
		}

		if (left_node->right == NULL)
		{
			left_node->right = right_node;
			right_node->parent = left_node;
			left_node->parent = parent_node;

			update_parent(parent_node, node, left_node);

			return;
		}

		binary_tree_node* successor_parent;
		binary_tree_node* successor = right_node;

		do
		{
			successor_parent = successor;
			successor = successor->left;
		}
		while (successor->left != NULL);

		if (successor->right != NULL)
			successor->right->parent = successor_parent;

		successor_parent->replace_child(successor, successor->right);

		successor->parent = parent_node;
		successor->left = left_node;
		successor->right = right_node;

		update_parent(parent_node, node, successor);
	}

protected:
	void update_parent(binary_tree_node* parent,
		binary_tree_node* old_child, binary_tree_node* new_child)
	{
		if (parent != NULL)
			parent->replace_child(old_child, new_child);
		else
			root = new_child;
	}

	binary_tree_node* root;
	size_t number_of_nodes;
};

template <class Less>
class binary_search_tree : public Less, public binary_search_tree_base
{
public:
	binary_search_tree(const Less& less_op) : Less(less_op)
	{
	}

	template <class Key>
	binary_tree_node* search(const Key& key, int* cmp_result) const
	{
		binary_tree_node* parent = NULL;
		binary_tree_node* node = root;

		while (node != NULL)
			if (less(key, *node))
			{
				parent = node;
				node = node->left;
				*cmp_result = -1;
			}
			else
				if (less(*node, key))
				{
					parent = node;
					node = node->right;
					*cmp_result = 1;
				}
				else
				{
					*cmp_result = 0;
					return node;
				}

		return parent;
	}

	void insert(binary_tree_node* node)
	{
		int cmp_result;

		binary_tree_node* parent = search(*node, &cmp_result);

		if (parent != NULL && cmp_result == 0)
		{
			binary_tree_node* new_parent = parent->right;

			while (new_parent != NULL)
			{
				parent = new_parent;

				if (less(*node, *new_parent))
				{
					new_parent = new_parent->left;
					cmp_result = -1;
				}
				else
				{
					new_parent = new_parent->right;
					cmp_result = 1;
				}
			}
		}

		node->left = node->right = NULL;

		if ((node->parent = parent) == NULL)
			root = node;
		else
			if (cmp_result < 0)
				parent->left = node;
			else
				parent->right = node;

		++number_of_nodes;
	}

	// TODO maintain a member pointer to the minimum element
	const binary_tree_node* minimum() const
	{
		const binary_tree_node* node = root;
		while (node->left != NULL)
			node = node->left;
		return node;
	}

	const binary_tree_node* maximum() const
	{
		const binary_tree_node* node = root;
		while (node->right != NULL)
			node = node->right;
		return node;
	}

	const binary_tree_node* inorder_next(const binary_tree_node* node) const
	{
		B_ASSERT(node != NULL);

		if (node->right != NULL)
		{
			node = node->right;
			while (node->left != NULL)
				node = node->left;
			return node;
		}

		const binary_tree_node* parent = node->parent;

		while (parent != NULL && parent->right == node)
		{
			node = parent;
			parent = parent->parent;
		}

		return parent;
	}

	bool less(const binary_tree_node& lhs,
		const binary_tree_node& rhs) const
	{
		return this->operator ()(lhs, rhs);
	}

	template <class Key>
	bool less(const Key& key, const binary_tree_node& node) const
	{
		return this->operator ()(key, node);
	}

	template <class Key>
	bool less(const binary_tree_node& node, const Key& key) const
	{
		return this->operator ()(node, key);
	}
};

B_END_NAMESPACE

#endif /* !defined(B_BINARY_TREE_H) */
