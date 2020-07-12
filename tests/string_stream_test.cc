// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/string_stream.h>

#include "test_case.h"

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
