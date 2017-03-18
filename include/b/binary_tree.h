/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016, 2017 Damon Revoe
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

	binary_tree_node* next();

	const binary_tree_node* prev() const;

	binary_tree_node* prev();
};

inline binary_tree_node* binary_tree_node::next()
{
	return const_cast<binary_tree_node*>(
		((const binary_tree_node*) this)->next());
}

inline binary_tree_node* binary_tree_node::prev()
{
	return const_cast<binary_tree_node*>(
		((const binary_tree_node*) this)->prev());
}

// Non-template base class for the 'binary_search_tree' template.
struct binary_search_tree_base
{
	binary_tree_node* root;
	binary_tree_node* leftmost;
	binary_tree_node* rightmost;
	size_t number_of_nodes;

	binary_search_tree_base() :
		root(NULL), leftmost(NULL), rightmost(NULL), number_of_nodes(0)
	{
	}

	bool is_empty() const
	{
		return root == NULL;
	}

	void insert_after_search(binary_tree_node* node,
		binary_tree_node* parent, int cmp_result);

	void remove(binary_tree_node* node);
};

// Binary search tree implementation. This is a low-level structure
// that exposes its internals. It is not meant for routine use.
// The 'set' and 'map' containers must be used instead.
template <class Key_op>
struct binary_search_tree : public binary_search_tree_base
{
	Key_op key_for_node;

	binary_search_tree(const Key_op& key_op) : key_for_node(key_op)
	{
	}

	template <class Key>
	binary_tree_node* find(const Key& key) const
	{
		for (binary_tree_node* node = root; node != NULL; )
			if (key < key_for_node(node))
				node = node->left;
			else
				if (key_for_node(node) < key)
					node = node->right;
				else
					return node;

		return NULL;
	}

	template <class Key>
	binary_tree_node* search(const Key& key, int* cmp_result) const
	{
		*cmp_result = 0;

		if (root == NULL)
			return NULL;

		for (binary_tree_node* node = root; ; )
			if (key < key_for_node(node))
				if (node->left != NULL)
					node = node->left;
				else
				{
					*cmp_result = -1;
					return node;
				}
			else
				if (key_for_node(node) < key)
					if (node->right != NULL)
						node = node->right;
					else
					{
						*cmp_result = 1;
						return node;
					}
				else
					return node;
	}

	void insert(binary_tree_node* node)
	{
		int cmp_result;

		binary_tree_node* parent =
			search(key_for_node(node), &cmp_result);

		insert_after_search(node, parent, cmp_result);
	}
};

B_END_NAMESPACE

#endif /* !defined(B_BINARY_TREE_H) */
