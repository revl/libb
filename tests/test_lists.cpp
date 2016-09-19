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

#include <b/doubly_linked_list_container.h>
#include <b/meta.h>

#include <b/object.h>

B_BEGIN_NAMESPACE

class TestElement;

typedef LinkedListNode<TestElement> TestElementListNode;

class AscListNode : public TestElementListNode
{
};

class DescListNode : public TestElementListNode
{
};

static int object_count = 0;

class TestElement : public Object, public AscListNode, public DescListNode
{
// Construction
public:
	TestElement(int initial);

// Attributes
public:
	int GetValue() const;

// Implementation
protected:
	virtual ~TestElement();

private:
	int value;
};

inline TestElement::TestElement(int initial_value) : value(initial_value)
{
	++object_count;
}

inline int TestElement::GetValue() const
{
	return value;
}

TestElement::~TestElement()
{
	--object_count;
}

typedef InheritedNodeSelector<AscListNode> AscListNodeSelector;
typedef InheritedNodeSelector<DescListNode> DescListNodeSelector;

typedef LinkedList<AscListNodeSelector> AscList;
typedef LinkedList<DescListNodeSelector> DescList;

int TestObjectLists()
{
	AscList asc_list;
	DescList desc_list;

	TestElement* element = new TestElement(2);

	asc_list.AddHead(element);
	desc_list.AddTail(element);
	element->AddRef();

	element = new TestElement(3);

	asc_list.AddTail(element);
	desc_list.AddHead(element);
	element->AddRef();

	element = new TestElement(1);

	asc_list.AddHead(element);
	desc_list.AddTail(element);
	element->AddRef();

	int i = 0;

	element = asc_list.GetHead();

	do
		if (element->GetValue() != ++i)
			return 4;
	while ((element = asc_list.GetNext(element)) != NULL);

	if ((element = desc_list.GetHead())->GetValue() != i)
		return 5;

	do
		if ((element = desc_list.GetNext(element))->GetValue() != --i)
			return 6;
	while (element != desc_list.GetTail());

	desc_list.MoveToTail(desc_list.GetHead(), NULL);
	desc_list.MoveToHead(desc_list.GetNext(desc_list.GetHead()),
		desc_list.GetHead());

	element = desc_list.GetHead();

	while (!asc_list.IsEmpty())
	{
		if (asc_list.GetHead()->GetValue() != element->GetValue())
			return 7;

		asc_list.RemoveHead();

		TestElement* next = desc_list.GetNext(element);
		element->Release();
		element = next;
	}

	desc_list.RemoveAll();

	return object_count;
}

typedef DoublyLinkedListContainer<int> IntegerList;

int TestIntegerList()
{
	IntegerList integer_list;

	int* element = integer_list.AddHead(2);
	integer_list.InsertAfter(element, 3);
	integer_list.AddHead(1);

	element = integer_list.GetHead();

	int i = 0;

	do
		if (*element != ++i)
			return 8;
	while ((element = integer_list.GetNext(element)) != NULL);

	return 0;
}

B_END_NAMESPACE

int main()
{
	int result = b::TestObjectLists();

	if (result != 0)
		return result;

	return b::TestIntegerList();
}
