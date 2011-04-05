// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SHAREDSINGLETON_VFHEADER__
#define __VF_SHAREDSINGLETON_VFHEADER__

#include "vf/vf_SharedObject.h"
#include "vf/vf_StaticMutex.h"

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

  typedef StaticMutex <SharedSingleton <Object> > MutexType;

  explicit SharedSingleton (const Lifetime lifetime)
    : m_lifetime (lifetime)
  {
    vfassert (s_instance == 0);
  }

  ~SharedSingleton ()
  {
    s_instance = 0;
  }

public:
  typedef SharedObjectPtr <Object> Ptr;

  static Ptr getInstance ()
  {
    MutexType::ScopedLockType lock;

    if (!s_instance)
    {
      s_instance = Object::createInstance ();

      if (s_instance->m_lifetime == persistAfterCreation)
        s_persistentReference = s_instance;
    }

    return s_instance;
  }

private:
  void destroySharedObject ()
  {
    MutexType::ScopedLockType lock;

    // See if someone snuck in a reference via getInstance ().
    if (!isBeingReferenced ())
      delete this;
  }

private:
  const Lifetime m_lifetime;

  class PersistentReference
  {
  public:
    // Intentionally lacking a constructor.
    // Inited to zero from static storage duration.
    ~PersistentReference ()
    {
      set (0);
    }

    inline void set (Object* object)
    {
      if (m_instance != object)
      {
        if (m_instance)
          m_instance->decReferenceCount ();

        m_instance = object;

        if (m_instance)
          m_instance->incReferenceCount ();
      }
    }

    inline void operator= (Object* object)
    {
      set (object);
    }

  private:
    Object* m_instance;
  };

  static Object* s_instance;
  static PersistentReference s_persistentReference;
};

template <class Object>
Object* SharedSingleton <Object>::s_instance;

template <class Object>
typename SharedSingleton <Object>::PersistentReference
  SharedSingleton <Object>::s_persistentReference;

#endif
