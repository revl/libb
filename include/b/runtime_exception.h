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

#include "string.h"

#ifndef B_RUNTIME_EXCEPTION_H
#define B_RUNTIME_EXCEPTION_H

#include "exception.h"

B_BEGIN_NAMESPACE

// The superclass of exceptions resulting from errors that may
// occur during normal program execution. These errors are usually
// caused by external circumstances, such as: invalid input, lack
// of permissions, running out of disk space, and so on.
class runtime_exception : public exception
{
public:
	// Returns the detailed description of this error.
	virtual string message() const = 0;
};

B_END_NAMESPACE

#endif /* !defined(B_RUNTIME_EXCEPTION_H) */
