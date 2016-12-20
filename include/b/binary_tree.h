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

	const binary_tree_node* next() const;
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

	bool is_empty() const
	{
		return number_of_nodes == 0;
	}

	void remove(binary_tree_node* node);

protected:
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
	const binary_tree_node* first() const
	{
		const binary_tree_node* node = root;
		while (node->left != NULL)
			node = node->left;
		return node;
	}

	const binary_tree_node* last() const
	{
		const binary_tree_node* node = root;
		while (node->right != NULL)
			node = node->right;
		return node;
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
