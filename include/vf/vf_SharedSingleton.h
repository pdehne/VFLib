// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SHAREDSINGLETON_VFHEADER__
#define __VF_SHAREDSINGLETON_VFHEADER__

#include "vf/vf_SharedObject.h"
#include "vf/vf_StaticMutex.h"

// Thread-safe singleton which comes into existence on first use, persists
// while referenced, and gets automatically destroyed when the last reference
// is removed. The object is re-created as needed.
//
template <class Object>
class SharedSingleton : public SharedObject
{
public:
  typedef SharedObjectPtr <Object> Ptr;

  SharedSingleton ()
  {
    vfassert (s_instance == 0);
  }

  ~SharedSingleton ()
  {
    s_instance = 0;
  }

  static Ptr getInstance ()
  {
    Ptr instance;

    StaticMutex <Object>::ScopedLockType lock;

    if (!s_instance)
      s_instance = Object::createInstance ();

    instance = s_instance;

    return instance;
  }

private:
  void destroySharedObject ()
  {
    StaticMutex <Object>::ScopedLockType lock;

    // See if someone snuck in a reference via getInstance ().
    if (!isBeingReferenced ())
      delete this;
  }

private:
  static Object* s_instance;
  static StaticMutex <Object> s_mutex;
};

template <class Object>
Object* SharedSingleton <Object>::s_instance;

template <class Object>
StaticMutex <Object> SharedSingleton <Object>::s_mutex;

#endif
