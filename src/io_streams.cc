// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/io_streams.h>

B_BEGIN_NAMESPACE

seekable::~seekable()
{
}

seekable_input_stream::~seekable_input_stream()
{
}

input_output_stream::input_output_stream()
{
}

input_output_stream::~input_output_stream()
{
}

namespace
{
	class file_input_stream : public input_stream
	{
	public:
		file_input_stream(FILE* s, const string& sn) :
			stream(s), stream_name(sn)
		{
		}

	private:
		virtual size_t read(void* buffer, size_t buffer_size);

		virtual bool eof();

		FILE* stream;
		const string stream_name;
	};

	size_t file_input_stream::read(void* buffer, size_t buffer_size)
	{
		size_t bytes_read = fread(buffer, 1, buffer_size, stream);

		if (bytes_read < buffer_size && ferror(stream) != 0)
			throw system_exception(stream_name, errno);

		return bytes_read;
	}

	bool file_input_stream::eof()
	{
		return feof(stream) != 0;
	}

	B_STRING_LITERAL(stdin_stream_name, "stdin");

	class std_input : public file_input_stream
	{
	public:
		std_input() : file_input_stream(stdin, stdin_stream_name)
		{
		}

		virtual void delete_this() const;
		virtual ~std_input();
	};

	void std_input::delete_this() const
	{
		// No-op: the only instance of this class is a singleton.
		// It is created in the data segment and therefore
		// cannot be deleted.
	}

	std_input::~std_input()
	{
		// Reset the reference counter to avoid triggering
		// an assertion in ~object().
		refs = 0;
	}

	class file_output_stream : public output_stream
	{
	public:
		file_output_stream(FILE* s, const string& sn) :
			stream(s), stream_name(sn)
		{
		}

	private:
		virtual size_t write(const void* buffer, size_t buffer_size);

		FILE* stream;
		const string stream_name;
	};

	size_t file_output_stream::write(const void* buffer, size_t buffer_size)
	{
		size_t bytes_written = fwrite(buffer, 1, buffer_size, stream);

		if (bytes_written < buffer_size)
			throw system_exception(stream_name, errno);

		return bytes_written;
	}

	class std_output : public file_output_stream
	{
	public:
		std_output(FILE* s, const string& sn) :
			file_output_stream(s, sn)
		{
		}

		virtual void delete_this() const;
		virtual ~std_output();
	};

	void std_output::delete_this() const
	{
		// No-op: all instances of this class are singletons
		// created in the data segment.
	}

	std_output::~std_output()
	{
		// Reset the reference counter to avoid triggering
		// an assertion in ~object().
		refs = 0;
	}
}

ref<input_stream> open_file_for_reading(const string& pathname)
{
	FILE* stream = fopen(pathname.data(), "rb");

	if (stream == NULL)
		throw system_exception(pathname, errno);

	return new file_input_stream(stream, pathname);
}

ref<output_stream> open_file_for_writing(const string& pathname)
{
	FILE* stream = fopen(pathname.data(), "wb");

	if (stream == NULL)
		throw system_exception(pathname, errno);

	return new file_output_stream(stream, pathname);
}

ref<input_stream> standard_input_stream()
{
	// Cannot be 'const' because of the reference counter.
	static std_input si;

	return &si;
}

ref<output_stream> standard_output_stream()
{
	B_STRING_LITERAL(stdout_stream_name, "stdout");

	// Cannot be 'const' because of the reference counter.
	static std_output so(stdout, stdout_stream_name);

	return &so;
}

ref<output_stream> standard_error_stream()
{
	B_STRING_LITERAL(stderr_stream_name, "stderr");

	// Cannot be 'const' because of the reference counter.
	static std_output se(stderr, stderr_stream_name);

	return &se;
}

B_END_NAMESPACE
