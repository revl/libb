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

#ifndef B_EXCEPTION_H
#define B_EXCEPTION_H

#ifdef __GNUG__
#pragma interface "B/Exception.h"
#endif // __GNUG__

#include "Host.h"

B_BEGIN_NAMESPACE

// The base class for all exceptions.
class B_API Exception
#ifdef B_USE_STL
	: public std::exception
#endif // B_USE_STL
{
#ifndef B_USE_STL
// Implementation
public:
	virtual ~Exception()
		throw ()
	{
	}
#endif // !defined(B_USE_STL)
};

B_END_NAMESPACE

#endif // !defined(B_EXCEPTION_H)
