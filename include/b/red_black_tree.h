// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_RED_BLACK_TREE_H
#define B_RED_BLACK_TREE_H

#include "binary_tree.h"

B_BEGIN_NAMESPACE

// Low-level structure that implements a self-balancing binary search tree.
struct red_black_tree_node : public binary_tree_node
{
	enum
	{
		red,
		black
	} color;

	const red_black_tree_node* next() const;

	red_black_tree_node* next();

	const red_black_tree_node* prev() const;

	red_black_tree_node* prev();
};

inline const red_black_tree_node* red_black_tree_node::next() const
{
	return static_cast<const red_black_tree_node*>(
		binary_tree_node::next());
}

inline red_black_tree_node* red_black_tree_node::next()
{
	return static_cast<red_black_tree_node*>(binary_tree_node::next());
}

inline const red_black_tree_node* red_black_tree_node::prev() const
{
	return static_cast<const red_black_tree_node*>(
		binary_tree_node::prev());
}

inline red_black_tree_node* red_black_tree_node::prev()
{
	return static_cast<red_black_tree_node*>(binary_tree_node::prev());
}

// Red-black tree implementation. This is a low-level structure
// that exposes its internals. It is not meant for routine use.
// The 'set' and 'map' containers must be used instead.
template <class Key_op>
struct red_black_tree : public binary_search_tree<Key_op>
{
	red_black_tree(const Key_op& key_op) :
		binary_search_tree<Key_op>(key_op)
	{
	}
};

B_END_NAMESPACE

#endif /* !defined(B_RED_BLACK_TREE_H) */
