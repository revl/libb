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

#ifndef B_RED_BLACK_TREE_H
#define B_RED_BLACK_TREE_H

#include "binary_tree.h"

B_BEGIN_NAMESPACE

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
