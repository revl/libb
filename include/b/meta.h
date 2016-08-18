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

#ifndef B_META_H
#define B_META_H

#include "host.h"

B_BEGIN_NAMESPACE

template <class NODE>
class InheritedNodeSelector
{
// Types
public:
	typedef typename NODE::Element Element;

	typedef NODE Node;

// Operations
public:
	static Node* GetNode(Element* element)
	{
		return static_cast<Node*>(element);
	}

	static const Node* GetNode(const Element* element)
	{
		return static_cast<const Node*>(element);
	}
};

B_END_NAMESPACE

#endif // B_META_H
