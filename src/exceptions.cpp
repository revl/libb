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

#ifdef __GNUG__
#pragma implementation "b/exception.h"
#pragma implementation "b/runtime_exception.h"
#pragma implementation "b/system_exception.h"
#pragma implementation "b/custom_exception.h"
#endif // __GNUG__

#include <b/system_exception.h>
#include <b/custom_exception.h>

B_BEGIN_NAMESPACE

void SystemException::GetMessage(String& target) const
	throw ()
{
	try
	{
#ifdef B_UNICODE
		const wchar_t* message_buffer = wcserror(code);
#else
		const char* message_buffer = strerror(code);
#endif // B_UNICODE
		target.Assign(message_buffer, b::CalcLength(message_buffer));
	}
	catch (Memory::Exception&)
	{
	}
}

#ifdef B_USE_STL
const char* SystemException::what() const
	throw ()
{
#ifdef B_UNICODE
	return wcserror(code);
#else
	return strerror(code);
#endif // B_UNICODE
}
#endif // B_USE_STL

CustomException::CustomException(const B_CHAR* message_format, ...) :
	message()
{
	va_list arguments;
	va_start(arguments, message_format);
	message.FormatV(message_format, arguments);
	va_end(arguments);
}

void CustomException::SetMessage(const B_CHAR* message_format, ...)
{
	va_list arguments;
	va_start(arguments, message_format);
	message.FormatV(message_format, arguments);
	va_end(arguments);
}

void CustomException::GetMessage(String& target) const
	throw ()
{
	target = message;
}

#ifdef B_USE_STL
const char* CustomException::what() const
	throw ()
{
	GetMessage(message);
	return message;
}
#endif // B_USE_STL

B_END_NAMESPACE
