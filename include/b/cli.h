// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

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
	// The name of the program binary, which is used in the help
	// and error messages.
	extern arg_name<string, 'p'> program_name;

	// Human-readable program version. This text is printed verbatim
	// in response to the '--version' option. If this parameter
	// is not specified, the '--version' option is not recognized.
	extern arg_name<string, 'v'> version_info;

	// A longer program description, which can be as long as needed,
	// but it is not meant to be a substitute for a proper man page.
	extern arg_name<string, 'd'> program_description;

	// The maximum width of the help text, 72 columns by default.
	extern arg_name<int, 'w'> help_text_width;

	// The column number where command descriptions start
	// in the list of all commands, 24 by default.
	extern arg_name<int, 'i' + 'c'> cmd_descr_indent;

	// The column number where option descriptions start,
	// 32 by default.
	extern arg_name<int, 'i' + 'a'> arg_descr_indent;

	// Parameter to override the stream that the 'help'
	// command prints to.
	extern arg_name<ref<output_stream>, 'o'> help_output_stream;
}

// POSIX-compatible command line parser and help screen generator.
class cli
{
B_OPAQUE:
	// Initializes this object with a one-line program description.
	cli(const string& program_summary);

	// Registers an optional switch meant to enable a non-default
	// mode of operation. Name variants must be separated by
	// vertical bars.
	void register_option(int unique_arg_id,
			const string& name_variants,
			const string& description);

	// Registers an option that requires a value. Option name
	// variants, both long and short, must be separated by
	// vertical bars.
	void register_option_with_parameter(int unique_arg_id,
			const string& name_variants,
			const string& param_name,
			const string& description);

	// Registers a required positional argument. Positional
	// arguments cannot have name variants. For a particular
	// command, the order of its positional arguments is
	// defined by the 'register_association()' method.
	void register_positional_argument(int unique_arg_id,
			const string& arg_name);

	// Registers a positional argument that can be omitted.
	void register_optional_positional(int unique_arg_id,
			const string& arg_name);

	// Registers an optional sequence of arguments.
	void register_zero_or_more_positional(int unique_arg_id,
			const string& arg_name);

	// Registers a non-empty sequence of arguments.
	void register_one_or_more_positional(int unique_arg_id,
			const string& arg_name);

	// Registers a command category with the specified title.
	void register_command_category(int unique_cat_id, const string& title);

	// Registers a command with the specified identifier,
	// name, and short and long descriptions. Commands can
	// have aliases separated by vertical bars. Command
	// identifier cannot be -1.
	void register_command(int unique_cmd_id,
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
