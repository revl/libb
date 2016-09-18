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

#ifndef B_SYSTEMEXCEPTION_H
#define B_SYSTEMEXCEPTION_H

#ifdef __GNUG__
#pragma interface "b/system_exception.h"
#endif // __GNUG__

#include "runtime_exception.h"

B_BEGIN_NAMESPACE

// System error condition. An exception of this class indicates
// abnormal termination of a system or standard library call.
class system_exception : public runtime_exception
{
// Construction
public:
	// Initializes the object using the specified error context
	// (subject, resource, or method name) and the specified
	// error code.
	system_exception(const string& context_or_subject, int errno_value);

// Information for the receiving end
public:
	// Retrieves the context that this object was constructed with.
	string context() const;

	// Retrieves the system error code that this object
	// was constructed with.
	int error_code() const;

	// Returns the detailed description of this error.
	virtual string message() const
		throw ();

#if defined(B_USE_STL)
	virtual const char* what() const
		throw ();
#endif // B_USE_STL

// Implementation
private:
	const string exception_context;
	const int system_error_code;

#if defined(B_USE_STL)
	mutable string message_buffer;
#endif // B_USE_STL
};

inline system_exception::system_exception(const string& context_or_subject,
		int errno_value) :
	exception_context(context_or_subject),
	system_error_code(errno_value)
{
}

inline string system_exception::context() const
{
	return exception_context;
}

inline int system_exception::error_code() const
{
	return system_error_code;
}

B_END_NAMESPACE

#endif // !defined(B_SYSTEMEXCEPTION_H)
