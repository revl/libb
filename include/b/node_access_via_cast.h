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

#ifndef B_NODE_ACCESS_VIA_CAST_H
#define B_NODE_ACCESS_VIA_CAST_H

#include "host.h"

B_BEGIN_NAMESPACE

// Class that helps find the node that corresponds to the
// specified element of an abstract data structure.
// This class assumes that either the node is a superclass
// of the element or the element defines a conversion to 'Node&'.
template <class Node>
class node_access_via_cast
{
public:
	typedef Node node_type;

	typedef typename Node::element_type element_type;

	// Returns a pointer to the node that is associated
	// with 'element'.
	static node_type* node_for(element_type* element)
	{
		return &static_cast<node_type&>(*element);
	}

	static const node_type* node_for(const element_type* element)
	{
		return &static_cast<node_type&>(*element);
	}
};

B_END_NAMESPACE

#endif /* !defined(B_NODE_ACCESS_VIA_CAST_H) */
