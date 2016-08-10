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

#include <B/Host.h>

#ifdef B_USE_PRECOMPILED_HEADER
#pragma hdrstop
#endif // B_USE_PRECOMPILED_HEADER

#ifdef __GNUG__
#pragma implementation "B/Exception.h"
#pragma implementation "B/RuntimeException.h"
#pragma implementation "B/SystemException.h"
#pragma implementation "B/CustomException.h"
#endif // __GNUG__

#include <B/SystemException.h>
#include <B/CustomException.h>

B_BEGIN_NAMESPACE

void SystemException::GetMessage(String& target) const
	throw ()
{
#ifdef B_USE_WIN32_API
	PTSTR message_buffer;
	DWORD length;

	try
	{
		if ((length = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM, NULL, code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(PTSTR) &message_buffer, 0, NULL)) > 0)
		{
			const B_CHAR* end_of_message = message_buffer + length;

			while (*--end_of_message == '\r' ||
				*end_of_message == '\n')
				--length;

			target.Assign(message_buffer, (int) length);
		}
	}
	catch (Memory::Exception&)
	{
		::LocalFree((HLOCAL) message_buffer);
	}
	::LocalFree((HLOCAL) message_buffer);

#else

	try
	{
#ifdef B_UNICODE
		const wchar_t* message_buffer = wcserror(code);
#else
		const char* message_buffer = strerror(code);
#endif // B_UNICODE
		target.Assign(message_buffer, B::CalcLength(message_buffer));
	}
	catch (Memory::Exception&)
	{
	}
#endif // B_USE_WIN32_API
}

#ifdef B_USE_STL
const char* SystemException::what() const
	throw ()
{
#ifdef B_USE_WIN32_API
	PTSTR message_buffer;

	// FIXME: Memory allocated for message is not freed
	return ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM, NULL, code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(PTSTR) &message_buffer, 0, NULL) > 0 ? message_buffer :
		"Unknown system error";
#else

#ifdef B_UNICODE
	return wcserror(code);
#else
	return strerror(code);
#endif // B_UNICODE

#endif // B_USE_WIN32_API
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
