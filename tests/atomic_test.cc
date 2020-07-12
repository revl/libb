// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/atomic.h>

#include "test_case.h"

B_TEST_CASE(atomic)
{
	b::atomic refs = B_ATOMIC_INIT(1);

	++refs;

	B_CHECK((int) refs == 2);

	B_CHECK(--refs);
	B_CHECK(!--refs);

	refs = 1;

	B_CHECK(!--refs);
}
