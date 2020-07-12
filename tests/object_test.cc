// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#include <b/object.h>

#include "test_case.h"

class base : public b::object
{
public:
	typedef b::ref<base> ref;

	static ref create(int new_object_id);

	int id() const;

	static int instance_count();

protected:
	base(int new_object_id);

	virtual ~base();

private:
	const int object_id;

	static int instance_counter;
};

base::base(int new_object_id) : object_id(new_object_id)
{
	++instance_counter;
}

base::ref base::create(int new_object_id)
{
	return new base(new_object_id);
}

int base::id() const
{
	return object_id;
}

int base::instance_count()
{
	return instance_counter;
}

base::~base()
{
	--instance_counter;
}

int base::instance_counter = 0;

class derived : public base
{
public:
	typedef b::ref<derived> ref;

	static ref create(int new_object_id);

	derived(int new_object_id);
};

inline derived::derived(int new_object_id) : base(new_object_id)
{
}

inline derived::ref derived::create(int new_object_id)
{
	return new derived(new_object_id);
}

B_TEST_CASE(object_count)
{
	{
		base::ref b1 = base::create(1);
		base::ref b2 = base::create(2);

		derived::ref d = derived::create(3);

		B_CHECK(base::instance_count() == 3);

		b1 = b2;
		b2 = d;

		B_CHECK(base::instance_count() == 2);

		b1.swap(b2);

		B_CHECK(b1->id() == 3);
		B_CHECK(b2->id() == 2);
	}

	B_CHECK(base::instance_count() == 0);
}

class movable : public virtual b::object
{
public:
	struct position
	{
		int x, y;
	};

	virtual position current_position() const = 0;

	virtual void move_to(const position& new_pos) = 0;
};

class pawn : public movable
{
public:
	virtual unsigned color() const = 0;
};

class controlled_by_computer : public virtual b::object
{
public:
	virtual void make_next_move() = 0;
};

class computer_controlled_pawn : public pawn, public controlled_by_computer
{
public:
	computer_controlled_pawn(unsigned pc) : pawn_color(pc)
	{
		pos.y = pos.x = 0;

		++number_of_pawns;
	}

	virtual ~computer_controlled_pawn()
	{
		--number_of_pawns;
	}

	virtual position current_position() const
	{
		return pos;
	}

	virtual void move_to(const position& new_pos)
	{
		pos = new_pos;
	}

	virtual unsigned color() const
	{
		return pawn_color;
	}

	virtual void make_next_move()
	{
		position new_pos;

		new_pos.x = pos.x + 1;
		new_pos.y = pos.y;

		move_to(new_pos);
	}

private:
	unsigned pawn_color;
	position pos;

public:
	static unsigned number_of_pawns;
};

unsigned computer_controlled_pawn::number_of_pawns = 0U;

B_TEST_CASE(multiple_inheritance)
{
	{
		b::ref<movable> m;

		{
			b::ref<computer_controlled_pawn> ccp =
					new computer_controlled_pawn(0xFFFF00U);

			movable::position pos =
			{
				10,
				10
			};

			ccp->move_to(pos);

			B_CHECK(computer_controlled_pawn::number_of_pawns ==
					1U);

			ccp->make_next_move();

			m = ccp;

			b::ref<pawn> p = m.cast<pawn>();

			B_CHECK(p->color() == 0xFFFF00U);
		}

		B_CHECK(m->current_position().x == 11);
	}

	B_CHECK(computer_controlled_pawn::number_of_pawns == 0U);
}
