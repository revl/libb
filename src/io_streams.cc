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
	class std_input : public input_stream
	{
		virtual void delete_this() const;

		virtual size_t read(void* buffer, size_t buffer_size);

		virtual bool eof();
	};

	void std_input::delete_this() const
	{
		// No-op: the only instance of this class is a singleton.
		// It is created in the data segment and therefore
		// cannot be deleted.
	}

	B_STATIC_CONST_STRING(stdin_stream_name, "stdin");

	size_t std_input::read(void* buffer, size_t buffer_size)
	{
		size_t bytes_read = fread(buffer, 1, buffer_size, stdin);

		if (bytes_read < buffer_size && ferror(stdin))
			throw system_exception(stdin_stream_name, errno);

		return bytes_read;
	}

	bool std_input::eof()
	{
		return feof(stdin) != 0;
	}

	class std_output : public output_stream
	{
	public:
		std_output(FILE* s, const string& sn) :
			stream(s), stream_name(sn)
		{
		}

	private:
		virtual void delete_this() const;

		virtual size_t write(const void* buffer, size_t buffer_size);

		FILE* stream;
		const string stream_name;
	};

	void std_output::delete_this() const
	{
		// No-op: all instances of this class are singletons
		// created in the data segment.
	}

	size_t std_output::write(const void* buffer, size_t buffer_size)
	{
		size_t bytes_written = fwrite(buffer, 1, buffer_size, stream);

		if (bytes_written < buffer_size)
			throw system_exception(stream_name, errno);

		return bytes_written;
	}
}

ref<input_stream> standard_input_stream()
{
	// Cannot be 'const' because of the reference counter.
	static std_input si;

	return &si;
}

ref<output_stream> standard_output_stream()
{
	B_STATIC_CONST_STRING(stdout_stream_name, "stdout");

	// Cannot be 'const' because of the reference counter.
	static std_output so(stdout, stdout_stream_name);

	return &so;
}

ref<output_stream> standard_error_stream()
{
	B_STATIC_CONST_STRING(stderr_stream_name, "stderr");

	// Cannot be 'const' because of the reference counter.
	static std_output se(stderr, stderr_stream_name);

	return &se;
}

B_END_NAMESPACE
