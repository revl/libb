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

#ifndef B_CUSTOM_EXCEPTION_H
#define B_CUSTOM_EXCEPTION_H

#if defined(__GNUG__) && !defined(__clang__)
#pragma interface "b/custom_exception.h"
#endif /* defined(__GNUG__) && !defined(__clang__) */

#include "runtime_exception.h"

B_BEGIN_NAMESPACE

// Customizable error exception class.
class custom_exception : public runtime_exception
{
public:
	// Constructs a new object with an empty description.
	custom_exception();

	// Initializes this object with a preformatted error message.
	custom_exception(const string& msg);

	// Initializes the error message to be contained by this
	// object using a format string and a variable number
	// of arguments.
	custom_exception(const char* fmt, ...) B_PRINTF_STYLE(2, 3);

	// Returns the detailed description of this error.
	virtual string message() const;

#ifdef B_USE_STL
	virtual const char* what() const
		throw ();
#endif /* defined(B_USE_STL) */

	virtual ~custom_exception()
	{
	}

private:
	string error_message;
};

inline custom_exception::custom_exception()
{
}

inline custom_exception::custom_exception(const string& msg) :
	error_message(msg)
{
}

B_END_NAMESPACE

#endif /* !defined(B_CUSTOM_EXCEPTION_H) */
