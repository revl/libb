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

#include <B/Pathname.h>

B_BEGIN_NAMESPACE

void Pathname::AppendPathnameTo(String& pathname) const
{
	switch (up_dir_level)
	{
	case INT_MAX:
		pathname.Append(B_PATH_SEPARATOR);

		if (!components.IsEmpty())
			GetComponents(pathname);
		break;

	case 0:
		if (!components.IsEmpty())
			GetComponents(pathname);
		else
			pathname.Append(B_TEXT('.'));
		break;

	default:
		int i = up_dir_level;

		if (!components.IsEmpty())
		{
			while (--i >= 0)
				pathname.Append(B_TEXT("..")
					B_PATH_SEPARATOR_SZ, 3);

			GetComponents(pathname);
		}
		else
		{
			while (--i > 0)
				pathname.Append(B_TEXT("..")
					B_PATH_SEPARATOR_SZ, 3);

			pathname.Append(B_TEXT(".."), 2);
		}
	}
}

void Pathname::ChDir(const Pathname& right_side)
{
	switch (right_side.up_dir_level)
	{
	case INT_MAX:
		*this = right_side;
		break;

	default:
		if (right_side.up_dir_level <= components.GetSize())
			components.RemoveAt(components.GetSize() -
				right_side.up_dir_level,
				right_side.up_dir_level);
		else
		{
			up_dir_level += right_side.up_dir_level -
				components.GetSize();

			components.RemoveAll();
		}

	case 0:
		components.Append(right_side.components);

		can_be_filename = right_side.can_be_filename;
	}
}

void Pathname::ChDir(const B_CHAR* pathname, int count)
{
	const B_CHAR* component;
	const B_CHAR* suffix;

	can_be_filename = false;

	if (count == 0)
		return;

	switch (*pathname)
	{
	case B_PATH_SEPARATOR:
		components.RemoveAll();
		up_dir_level = INT_MAX;

		goto Slash;

	case B_TEXT('.'):
		goto DotFirst;
	}

NextComponent:
	component = pathname;
	suffix = NULL;

Continue:
	for (;;)
	{
		++pathname;

		if (--count == 0)
		{
			AddComponent(component, suffix, pathname);

			can_be_filename = true;

			return;
		}

		switch (*pathname)
		{
		case B_PATH_SEPARATOR:
			AddComponent(component, suffix, pathname);

			goto Slash;

		case B_TEXT('.'):
			suffix = pathname;
		}
	}

DotFirst:
	if (--count == 0)
		return;

	component = pathname;
	suffix = NULL;

	switch (*++pathname)
	{
	case B_PATH_SEPARATOR:
		goto Slash;

	case B_TEXT('.'):
		suffix = pathname;

		if (--count == 0)
		{
			GoUpDir();

			return;
		}

		switch (*++pathname)
		{
		case B_PATH_SEPARATOR:
			GoUpDir();

			goto Slash;

		case B_TEXT('.'):
			suffix = pathname;
		}
	}

	goto Continue;

Slash:
	if (--count == 0)
		return;

	switch (*++pathname)
	{
	case B_PATH_SEPARATOR:
		goto Slash;

	case B_TEXT('.'):
		goto DotFirst;
	}

	goto NextComponent;
}

void Pathname::GetComponents(String& pathname) const
{
	int count = components.GetSize();

	if (count > 0)
	{
		const Component* component = components.GetBuffer();

		component->AppendNameTo(pathname);

		while (--count > 0)
		{
			pathname.Append(B_PATH_SEPARATOR);

			(++component)->AppendNameTo(pathname);
		}
	}
}

B_END_NAMESPACE
