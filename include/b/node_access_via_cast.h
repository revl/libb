// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_NODE_ACCESS_VIA_CAST_H
#define B_NODE_ACCESS_VIA_CAST_H

#include "host.h"

B_BEGIN_NAMESPACE

// Utility class for accessing "implantable" nodes of various data
// structures and finding the node that corresponds to the
// specified element of the data structure.
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
