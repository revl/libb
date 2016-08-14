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

#include <B/Pathname.h>

int main()
{
	B::String current_dir;

	current_dir.AllocExactly(MAX_PATH);

	getcwd(current_dir.LockBuffer(), current_dir.GetCapacity());

	current_dir.UnlockBuffer(chars_written);

	printf("[%s]$ cd ", current_dir.GetBuffer());

	B_CHAR buffer[1024];

	while (fgets(buffer, sizeof(buffer), stdin))
	{
		B::Pathname path(current_dir, current_dir.GetLength());

		int buffer_length = B::CalcLength(buffer);

		path.ChDir(buffer, buffer_length > 0 &&
			buffer[buffer_length - 1] == '\n' ?
			buffer_length - 1: buffer_length);

		B::String new_dir;
		path.AppendPathnameTo(new_dir);

		current_dir = new_dir;

		printf("[%s]$ cd ", current_dir.GetBuffer());
	}

	printf("\n");

	return 0;
}
