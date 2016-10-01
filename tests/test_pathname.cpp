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

#include <b/pathname.h>

#include <unistd.h>

int main()
{
	b::string current_dir;

	current_dir.reserve(1024);

	getcwd(current_dir.lock(), current_dir.capacity());

	current_dir.unlock(b::calc_length(current_dir.data()));

	printf("[%s]$ cd ", current_dir.c_str());

	char buffer[1024];

	//while (fgets(buffer, sizeof(buffer), stdin))
	while (0)
	{
		b::pathname path(current_dir);

		size_t buffer_length = b::calc_length(buffer);

		path.ChDir(b::string_view(buffer, buffer_length > 0 &&
			buffer[buffer_length - 1] == '\n' ?
			buffer_length - 1: buffer_length));

		b::string new_dir;
		path.AppendPathnameTo(new_dir);

		current_dir = new_dir;

		printf("[%s]$ cd ", current_dir.c_str());
	}

	printf("\n");

	return 0;
}
