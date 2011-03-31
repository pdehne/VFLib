// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STATICOBJECT_VFHEADER__
#define __VF_STATICOBJECT_VFHEADER__

// Wraps an object up so it can exist at file scope with static storage
// duration. The object is initialized with zeroes as per the C++ spec.
// Non-trivial constructors will silently fail with undefined behavior.
//
// Spec: N2914=09-0104
//
// [3.6.2] Initialization of non-local objects
//
//         Objects with static storage duration (3.7.1) or thread storage
//         duration (3.7.2) shall be zero-initialized (8.5) before any
//         other initialization takes place.
//
template <class Tag, class Object>
class StaticObject
{
public:
  Object* operator-> () { return getObject (); }
  Object& operator* () { return *getObject (); }

  Object const* operator-> () const { return getObject (); }
  Object const& operator* () const { return *getObject (); }

  static Object* getObject ()
  {
    return reinterpret_cast <Object*> (s_storage);
  }

private:
  static char s_storage [sizeof (Object)];
};

template <class Tag, class Object>
char StaticObject <Tag, Object>::s_storage[sizeof (Object)];

#endif
