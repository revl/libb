// This file is part of the B library, which is released under the MIT license.
// Copyright (C) 2002-2007, 2016-2020 Damon Revoe <him@revl.org>
// See the file LICENSE for the license terms.

#ifndef B_REF_H
#define B_REF_H

#include "host.h"

B_BEGIN_NAMESPACE

// Smart pointer to instances of classes derived from the 'object' class.
template <class T>
class ref
{
public:
	// Constructs a null pointer.
	ref();

	// Increases the reference count of the object pointed to by 'that'.
	ref(const ref& that);

	// Initializes this instance with a pointer to an existing object.
	ref(T* obj_ptr);

	// Initializes this reference using a reference to a subclass.
	template <class C>
	ref(const ref<C>& that);

	// Tests if this is a null pointer.
	bool is_null() const;

	// Returns a C++ pointer to the controlled object.
	T* ptr() const;

	// The same as ptr().
	operator T*() const;

	// Member selection operator.
	T* operator ->() const;

	// Dereference operator.
	T& operator *() const;

	// Casts the object pointed to by this smart pointer to the derived
	// class 'C' and returns a smart pointer to the resulting object.
	//
	// Because this method uses 'static_cast' and not 'dynamic_cast',
	// it cannot be used if 'T' is a virtual base class of 'C'.
	template <class C>
	ref<C> cast();

	// Various comparison operators.
	bool operator ==(const ref& rhs) const;
	bool operator ==(T* rhs) const;

	bool operator !=(const ref& rhs) const;
	bool operator !=(T* rhs) const;

	bool operator <(const ref& rhs) const;
	bool operator <(T* rhs) const;

	bool operator >(const ref& rhs) const;
	bool operator >(T* rhs) const;

	bool operator <=(const ref& rhs) const;
	bool operator <=(T* rhs) const;

	bool operator >=(const ref& rhs) const;
	bool operator >=(T* rhs) const;

	// Switches to a new object. Releases the previously
	// controlled object and grabs a reference to the new one.
	void assign(T* new_obj);

	// Assignment operators.
	ref& operator =(const ref& rhs);
	ref& operator =(T* new_obj);
	template <class C>
	ref& operator =(const ref<C>& rhs);

	// Switches to a new object without incrementing its
	// reference count. Releases the previously controlled object.
	void attach(T* new_obj);

	// Detaches this smart pointer from the object it points to
	// without decrementing the reference count of the object,
	// then resets this smart pointer to zero. Returns a C++
	// pointer to the detached object.
	T* detach();

	// Makes this smart pointer instance point to the object
	// pointed to by 'that' and vice versa.
	void swap(ref& that);

	// Releases the controlled object.
	~ref();

protected:
	T* obj;
};

template <class T>
inline ref<T>::ref() : obj(NULL)
{
}

template <class T>
inline ref<T>::ref(const ref<T>& that)
{
	if ((obj = that.ptr()) != NULL)
		obj->add_ref();
}

template <class T>
template <class C>
inline ref<T>::ref(const ref<C>& that)
{
	if ((obj = that.ptr()) != NULL)
		obj->add_ref();
}

template <class T>
inline ref<T>::ref(T* obj_ptr)
{
	if ((obj = obj_ptr) != NULL)
		obj_ptr->add_ref();
}

template <class T>
inline bool ref<T>::is_null() const
{
	return obj == NULL;
}

template <class T>
inline T* ref<T>::ptr() const
{
	return obj;
}

template <class T>
inline ref<T>::operator T*() const
{
	return obj;
}

template <class T>
inline T* ref<T>::operator ->() const
{
	B_ASSERT(obj != NULL);

	return obj;
}

template <class T>
inline T& ref<T>::operator *() const
{
	return *obj;
}

template <class T>
template <class C>
inline ref<C> ref<T>::cast()
{
	return ref<C>(static_cast<C*>(obj));
}

template <class T>
inline bool ref<T>::operator ==(const ref<T>& rhs) const
{
	return obj == rhs.obj;
}

template <class T>
inline bool ref<T>::operator ==(T* rhs) const
{
	return obj == rhs;
}

template <class T>
inline bool ref<T>::operator !=(const ref<T>& rhs) const
{
	return obj != rhs.obj;
}

template <class T>
inline bool ref<T>::operator !=(T* rhs) const
{
	return obj != rhs;
}

template <class T>
inline bool ref<T>::operator <(const ref<T>& rhs) const
{
	return obj < rhs.obj;
}

template <class T>
inline bool ref<T>::operator <(T* rhs) const
{
	return obj < rhs;
}

template <class T>
inline bool ref<T>::operator >(const ref<T>& rhs) const
{
	return obj > rhs.obj;
}

template <class T>
inline bool ref<T>::operator >(T* rhs) const
{
	return obj > rhs;
}

template <class T>
inline bool ref<T>::operator <=(const ref<T>& rhs) const
{
	return obj <= rhs.obj;
}

template <class T>
inline bool ref<T>::operator <=(T* rhs) const
{
	return obj <= rhs;
}

template <class T>
inline bool ref<T>::operator >=(const ref<T>& rhs) const
{
	return obj >= rhs.obj;
}

template <class T>
inline bool ref<T>::operator >=(T* rhs) const
{
	return obj >= rhs;
}

template <class T>
inline void ref<T>::assign(T* new_obj)
{
	if (new_obj != NULL)
		new_obj->add_ref();

	T* old_obj = obj;

	obj = new_obj;

	if (old_obj != NULL)
		old_obj->release();
}

template <class T>
inline ref<T>& ref<T>::operator =(const ref<T>& rhs)
{
	assign(rhs.obj);

	return *this;
}

template <class T>
inline ref<T>& ref<T>::operator =(T* new_obj)
{
	assign(new_obj);

	return *this;
}

template <class T>
template <class C>
inline ref<T>& ref<T>::operator =(const ref<C>& rhs)
{
	assign(rhs.ptr());

	return *this;
}

template <class T>
inline void ref<T>::attach(T* new_obj)
{
	if (obj != NULL)
		obj->release();

	obj = new_obj;
}

template <class T>
inline T* ref<T>::detach()
{
	T* this_obj = obj;

	obj = NULL;
	return this_obj;
}

template <class T>
inline void ref<T>::swap(ref<T>& that)
{
	T* this_obj = obj;

	obj = that.obj;
	that.obj = this_obj;
}

template <class T>
ref<T>::~ref()
{
	if (obj != NULL)
		obj->release();
}

B_END_NAMESPACE

#endif /* !defined(B_REF_H) */
