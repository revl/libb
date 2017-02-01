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

#include <b/string_stream.h>

#include "unit_test.h"

B_TEST_CASE(string_stream)
{
	b::ref<b::input_output_stream> ios = new b::string_stream;

	ios->write("Hello", 5);
	ios->write(", ", 2);
	ios->write("World.", 6);

	ios->seek(-1, b::seekable::end);
	ios->write("!", 1);

	ios->seek(0U);

	char buffer[16];

	b::string result(buffer,
		ios->read(buffer, sizeof(buffer)));

	ios->seek(7U);

	b::ref<b::input_stream> is(ios);

	result.assign(buffer,
		ios->read(buffer, sizeof(buffer)));

	B_CHECK(result == "World!");
}
