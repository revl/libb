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

#ifndef B_OPTION_LIST_H
#define B_OPTION_LIST_H

#include "host.h"

B_BEGIN_NAMESPACE

struct option_list
{
	option_list(int t) : tag(t), prev_opt(NULL)
	{
	}

	operator const option_list*() const
	{
		return this;
	}

	const option_list& operator ,(const option_list& next_opt) const
	{
		next_opt.prev_opt = this;
		return next_opt;
	}

	int tag;
	mutable const option_list* prev_opt;
};

template <typename T>
struct option_value : public option_list
{
	option_value(int t, const T& v) : option_list(t), value(v)
	{
	}

	T value;
};

template <typename T, int Tag>
struct option
{
	option_value<T> operator =(const T& v) const
	{
		return option_value<T>(Tag, v);
	}

	static bool is_tag_of(const option_list* opt)
	{
		return opt->tag == Tag;
	}

	static const T& value(const option_list* opt)
	{
		return static_cast<const option_value<T>*>(opt)->value;
	}
};

B_END_NAMESPACE

#endif /* !defined(B_OPTION_LIST_H) */
