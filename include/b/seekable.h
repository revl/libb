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

#ifndef B_SEEKABLE_H
#define B_SEEKABLE_H

#include "object.h"

B_BEGIN_NAMESPACE

// Interface to retrieve and change the current read/write position
// within a stream.
class seekable : public virtual object
{
public:
	// Returns the current stream position value.
	virtual size_t position() const = 0;

	// Constants that define how the 'offset' argument is
	// interpreted by the 'seek()' method.
	enum relative_to
	{
		// The offset argument sets the new position in the stream.
		beg,

		// The offset is relative to the current position
		// in the stream.
		cur,

		// The offset is relative to the of the stream.
		end
	};

	// Change the current stream position.
	virtual void seek(off_t offset, relative_to whence = beg) = 0;

	// Returns the file size for the open stream.
	virtual size_t size() const = 0;
};

B_END_NAMESPACE

#endif /* !defined(B_SEEKABLE_H) */
