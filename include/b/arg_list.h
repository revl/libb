/*
 * B: Common C++ library
 * Copyright (C) 2002-2007, 2016-2020 Damon Revoe
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

// A technique to pass a variable-length list of named parameters to
// a function or a method

#ifndef B_ARG_LIST_H
#define B_ARG_LIST_H

#include "host.h"

B_BEGIN_NAMESPACE

// The base for all 'arg_val' structures. Contains a unique
// tag that identifies the argument.
struct arg_list
{
	// Converts this list to a reference.
	// Required for proper syntax.
	operator const arg_list*() const
	{
		return this;
	}

	// Links two argument lists together.
	const arg_list& operator ,(const arg_list& next_arg) const
	{
		next_arg.prev_arg = this;
		return next_arg;
	}

	// The tag defines the type and the semantics
	// of the argument.
	const int tag;

	// A pointer to the next argument in the list.
	mutable const arg_list* prev_arg;

protected:
	// For use by 'arg_val'.
	arg_list(int t) : tag(t), prev_arg(NULL)
	{
	}
};

// A wrapper around the argument value.
template <typename T>
struct arg_val : public arg_list
{
	// Saves the tag and the value.
	arg_val(int t, const T& v) : arg_list(t), value(v)
	{
	}

	const T value;
};

// Instances of this class represent named argument keywords and
// assign tags to argument values.
template <typename T, int Tag>
struct arg_name
{
	// Creates an 'arg_val' object on the stack.
	arg_val<T> operator =(const T& v) const
	{
		return arg_val<T>(Tag, v);
	}

	// Checks if the provided argument value
	// has the same tag as this argument name.
	static bool is_name_for(const arg_list* arg)
	{
		return arg->tag == Tag;
	}

	// Returns the value of the specified argument.
	static const T& value(const arg_list* arg)
	{
		B_ASSERT(arg->tag == Tag);

		return static_cast<const arg_val<T>*>(arg)->value;
	}
};

// An alternative way of defining an argument name.
template <typename T>
struct arg_name_obj
{
	// Initialize this argument name with a tag.
	explicit arg_name_obj(int t) : tag(t)
	{
	}

	// Creates an 'arg_val' object on the stack.
	arg_val<T> operator =(const T& v) const
	{
		return arg_val<T>(tag, v);
	}

	// Checks if the provided argument value
	// has the same tag as this argument name.
	bool is_name_for(const arg_list* arg) const
	{
		return arg->tag == tag;
	}

	// Returns the value of the specified argument.
	const T& value(const arg_list* arg) const
	{
		B_ASSERT(arg->tag == tag);

		return static_cast<const arg_val<T>*>(arg)->value;
	}

	const int tag;
};

B_END_NAMESPACE

#endif /* !defined(B_ARG_LIST_H) */
