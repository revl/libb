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

#ifndef B_COMMAND_LINE_PARSER_H
#define B_COMMAND_LINE_PARSER_H

#include "opaque.h"
#include "string.h"
#include "arg_list.h"
#include "io_streams.h"

B_BEGIN_NAMESPACE

// Optional arguments that can be passed to the cli::parse() method.
//
// Example:
//
// int cmd_id = cl_parser.parse(argc, argv, b::cli_args::help_text_width = 80);
//
namespace cli_args
{
	enum
	{
		program_name_tag,
		version_info_tag,
		program_description_tag,
		help_text_width_tag,
		cmd_descr_indent_tag,
		arg_descr_indent_tag,
		help_output_stream_tag,
		error_stream_tag
	};

	// The name of the program binary, which is used in the help
	// and error messages.
	extern arg_name<string, program_name_tag> program_name;

	// Human-readable program version. This text is printed verbatim
	// in response to the '--version' option. If this parameter
	// is not specified, the '--version' option is not recognized.
	extern arg_name<string, version_info_tag> version_info;

	// A longer program description, which can be as long as needed,
	// but it is not meant to be a substitute for a proper man page.
	extern arg_name<string, program_description_tag> program_description;

	// The maximum width of the help text, 72 columns by default.
	extern arg_name<int, help_text_width_tag> help_text_width;

	// The column number where command descriptions start
	// in the list of all commands, 24 by default.
	extern arg_name<int, cmd_descr_indent_tag> cmd_descr_indent;

	// The column number where option descriptions start,
	// 32 by default.
	extern arg_name<int, arg_descr_indent_tag> arg_descr_indent;

	// Parameter to override the stream that the 'help'
	// command prints to.
	extern arg_name<ref<output_stream>, help_output_stream_tag>
		help_output_stream;

	// Parameter to override the stream for error reporting.
	extern arg_name<ref<output_stream>, error_stream_tag> error_stream;
}

// POSIX-compatible command line parser and help screen generator.
class cli
{
B_OPAQUE:
	// Initialize this object with a one-line program description.
	cli(const string& program_summary);

	// Command line argument types.
	enum arg_type {
		// An optional switch that enables a non-default
		// mode of operation.
		option,

		// An optional switch that requires a value.
		option_with_parameter,

		// A required positional argument. Positional
		// arguments are reported by the next_arg() method
		// in the order they were associated with the
		// command.
		positional_argument,

		// Similar to 'positional_argument', but optional.
		optional_positional,

		// An optional sequence of arguments.
		zero_or_more_positional,

		// An non-empty sequence of arguments.
		one_or_more_positional
	};

	// Registers an argument of the specified type with the
	// specified identifier and description. For options, long
	// and short variants must be separated by vertical bars.
	// Positional arguments cannot have name variants.
	void register_arg(arg_type type,
			int arg_id,
			const string& name_variants,
			const string& description);

	// Registers a command category with the specified title.
	void register_command_category(int cat_id, const string& title);

	// Registers a command with the specified identifier,
	// name, and short and long descriptions. Commands can
	// have aliases separated by vertical bars. Command
	// identifier cannot be -1.
	void register_command(int cmd_id,
			const string& name_variants,
			const string& synopsis,
			const string& usage,
			int cat_id = -1);

	// Registers an association between a command and an
	// argument. The order in which associations are
	// registered defines the order of positional arguments as
	// well as the order of options in help screens.
	void register_association(int cmd_id, int arg_id);

	// Parses the command line.
	//
	// If at least one command has been registered, a command
	// must be present on the command line. If no command is
	// given or there is a parsing error, the method throws a
	// 'runtime_exception'. Otherwise, the method returns the
	// identifier of the command.
	//
	// If the command specified by the user is either 'help'
	// or 'version', the method prints the help screen or the
	// program version respectively and returns -1, in which
	// case the caller must exit the program with a zero
	// exit status.
	//
	// If no commands have been registered, the return value
	// of the method can be ignored unless it is -1.
	int parse(int argc, const char* const *argv,
			const arg_list* arg = NULL);

	// Consecutively retrieves command line argument values in
	// the order they appear on the command line.
	//
	// If the method returns true, an argument is found on the
	// command line. Its identifier is stored in 'arg_id' and
	// the value is stored in 'arg_value'. If there are no
	// more arguments, the method returns false.
	bool next_arg(int* arg_id, const char** arg_value);
};

B_END_NAMESPACE

#endif /* !defined(B_COMMAND_LINE_PARSER_H) */
