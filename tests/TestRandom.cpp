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

#ifdef B_USE_PRECOMPILED_HEADER
#include <B/Host.h>

#pragma hdrstop
#endif // B_USE_PRECOMPILED_HEADER

#include <B/Random.h>

int main()
{
	B::Random r1(1), r2(1), r3;

	bool test = r1.GetNext() == r2.GetNext();

	test &= r2.GetNext() != r3.GetNext();

	test &= r1.GetNext() != r2.GetNext();

	return !test;
}
