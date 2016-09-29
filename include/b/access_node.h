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

#ifndef B_ACCESS_NODE_H
#define B_ACCESS_NODE_H

#include "host.h"

B_BEGIN_NAMESPACE

// Returns the node that corresponds to the specified element of
// an abstract data structure.
// This default implementation assumes that either the node is a
// base class of the element or the element defines a conversion
// to Node&.
template <class Node, class Element>
Node* access_node(Element* element)
{
	return &static_cast<Node&>(*element);
}

B_END_NAMESPACE

#endif /* !defined(B_ACCESS_NODE_H) */
