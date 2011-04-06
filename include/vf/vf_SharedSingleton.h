// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SHAREDSINGLETON_VFHEADER__
#define __VF_SHAREDSINGLETON_VFHEADER__

#include "vf/vf_SharedObject.h"
#include "vf/vf_SpinLock.h"
#include "vf/vf_Static.h"

// Thread-safe singleton which comes into existence on first use.
// An option controls whether the singleton persists after creation,
// or if it is created and destroyed as needed based on reference counts.
//
// Object must provide this function:
//
//  Object* Object::createInstance()
//
template <class Object>
class SharedSingleton : public SharedObject
{
protected:
  enum Lifetime
  {
    createOnDemand,
    persistAfterCreation
  };

  explicit SharedSingleton (const Lifetime lifetime)
    : m_lifetime (lifetime)
  {
    vfassert (s_instance == 0);
  }

  ~SharedSingleton ()
  {
    //s_instance = 0;
    vfassert (s_instance == 0);
  }

public:
  typedef SharedObjectPtr <Object> Ptr;

  static Ptr getInstance ()
  {
    Ptr instance;

    {
      SpinLock::ScopedLockType lock (*s_mutex);

      instance = s_instance;
    }

    if (!instance)
    {
      SpinLock::ScopedLockType lock (*s_mutex);

      instance = s_instance;

      if (!instance)
      {
        s_instance = Object::createInstance ();

        if (s_instance->m_lifetime == persistAfterCreation)
        {
          *s_ref = s_instance;
        }

        instance = s_instance;
      }
    }

    return instance;
  }

private:
  void destroySharedObject ()
  {
    bool destroy;

    {
      SpinLock::ScopedLockType lock (*s_mutex);

      if (isBeingReferenced ())
      {
        destroy = false;
      }
      else
      {
        destroy = true;
        s_instance = 0;
      }
    }

    if (destroy)
    {
      delete this;
    }
  }

private:
  const Lifetime m_lifetime;

private:
  static Object* s_instance;
  static Static::Object <Ptr, SharedSingleton <Object>> s_ref;
  static Static::Storage <SpinLock, SharedSingleton <Object>> s_mutex;
};

template <class Object>
Object* SharedSingleton <Object>::s_instance;

template <class Object>
Static::Object <typename SharedSingleton <Object>::Ptr, SharedSingleton <Object> >
  SharedSingleton <Object>::s_ref;

template <class Object>
Static::Storage <SpinLock, SharedSingleton <Object> >
  SharedSingleton <Object>::s_mutex;

#endif
