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
// abnormal termination of a system call.
class SystemException : public RuntimeException
{
// Types
public:
	// System-specific typedef for the system error number.
	typedef int Code;

	// Enumeration of system-specific error codes.
	enum
	{
		FileNotFound = ENOENT,
		PathNotFound = ENOENT
	};

// Utility Method
public:
	// Returns the error code of the last system error occured
	// in the calling thread.
	static Code GetLastErrorCode();

// Construction
public:
	// Constructs an object with the number of the last error.
	SystemException();

	// Constructs an object with the specified error number.
	SystemException(Code code);

// Attributes
public:
	// Retrieves the stored code of the last system error.
	Code GetErrorCode() const;

	// Sets the error number to the specified value.
	void SetErrorCode(Code code);

// Overridden
public:
	virtual void GetMessage(String& target) const
		throw ();

#if defined(B_USE_STL)
	virtual const char* what() const
		throw ();
#endif // B_USE_STL

// Implementation
private:
	Code code;
};

inline SystemException::Code SystemException::GetLastErrorCode()
{
	return errno;
}

inline SystemException::SystemException() : code(GetLastErrorCode())
{
}

inline SystemException::SystemException(Code saved_code) : code(saved_code)
{
}

inline SystemException::Code SystemException::GetErrorCode() const
{
	return code;
}

inline void SystemException::SetErrorCode(Code new_code)
{
	code = new_code;
}

B_END_NAMESPACE

#include "memory.h"

#endif // !defined(B_SYSTEMEXCEPTION_H)
