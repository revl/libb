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
};

class binary_search_tree_base
{
public:
	binary_search_tree_base() : root(NULL), size(0)
	{
	}

protected:
	binary_tree_node* root;
	size_t size;
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
		{
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
					cmp_result = 0;
					return node;
				}
		}

		return parent;
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
