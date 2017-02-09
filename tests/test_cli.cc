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

#include <b/cli.h>

#include "unit_test.h"

#include <b/string_stream.h>

B_STATIC_CONST_STRING(app_name, "console_app");
B_STATIC_CONST_STRING(app_version, "1.2");
B_STATIC_CONST_STRING(app_summary, "Test the b::cli class.");
B_STATIC_CONST_STRING(app_description,
		"Here goes a description of things the app does. "
		"The description can be as long as needed to "
		"convey the purpose of this application. However, "
		"it does not substitute a proper manual page.");

B_TEST_CASE(cli)
{
	b::cli cli_parser(app_name, app_version, app_summary, app_description);

	const char* help_command[] =
	{
		"test_cli",
		"--help"
	};

	b::ref<b::string_stream> help_str = new b::string_stream;

	cli_parser.parse(2, help_command,
		b::cli::help_output_stream = help_str);

	B_CHECK(match_pattern(help_str->str(),
		"console_app: Test the b::cli class.\n\n"
		"Usage: console_app\n\n"
		"Here goes *\n*\n* manual page.\n\n"));
}
