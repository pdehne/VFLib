// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SHAREDSINGLETON_VFHEADER__
#define __VF_SHAREDSINGLETON_VFHEADER__

#include "vf/vf_PerformedAtExit.h"
#include "vf/vf_SharedObject.h"
#include "vf/vf_SpinLock.h"
#include "vf/vf_Static.h"

// Thread-safe singleton which comes into existence on first use.
// All objects with static storage duration should, in general, be singletons,
// or else they may not be leak-checked correctly.
//
// class Object must provide this function:
//
//  Object* Object::createInstance()
//

//------------------------------------------------------------------------------

template <class Object>
class SharedSingleton : private PerformedAtExit
{
protected:
  enum Lifetime
  {
    // Singleton is created on first use and destroyed when
    // the last reference is removed.
    //
    createOnDemand,

    // Like createOnDemand, but after the Singleton is destroyed an
    // exception will be thrown if an attempt is made to create it again.
    //
    createOnDemandOnce,

    // The singleton is created on first use and persists until program exit.
    //
    persistAfterCreation
  };

  explicit SharedSingleton (Lifetime const lifetime)
    : PerformedAtExit (lifetime == persistAfterCreation)
    , m_lifetime (lifetime)
  {
    vfassert (s_instance == nullptr);

    if (m_lifetime == persistAfterCreation)
    {
      incReferenceCount ();
    }
    else if (m_lifetime == createOnDemandOnce && *s_created)
    {
      Throw (Error().fail (__FILE__, __LINE__));
    }

    *s_created = true;
  }

  virtual ~SharedSingleton ()
  {
    vfassert (s_instance == nullptr);
  }

public:
  typedef SharedObjectPtr <Object> Ptr;

  static Ptr getInstance ()
  {
    Ptr instance;

    instance = s_instance;

    if (instance == nullptr)
    {
      SpinLock::ScopedLockType lock (*s_mutex);

      instance = s_instance;
  
      if (instance == nullptr)
      {
        s_instance = Object::createInstance ();

        instance = s_instance;
      }
    }

    return instance;
  }

  inline void incReferenceCount() noexcept
  {
    m_refs.addref ();
  }

  inline void decReferenceCount() noexcept
  {
    vfassert (m_refs.is_signaled ());

    if (m_refs.release ())
      destroySingleton ();
  }

  // Caller must synchronize.
  inline bool isBeingReferenced () const
  {
    return m_refs.is_signaled ();
  }

private:
  void performAtExit ()
  {
    decReferenceCount ();
  }

  void destroySingleton ()
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
  Lifetime const m_lifetime;
  Atomic::Counter m_refs;

private:
  static Object* s_instance;
  static Static::Storage <SpinLock, SharedSingleton <Object> > s_mutex;
  static Static::Storage <bool, SharedSingleton <Object> > s_created;
};

template <class Object>
Object* SharedSingleton <Object>::s_instance;

template <class Object>
Static::Storage <SpinLock, SharedSingleton <Object> >
  SharedSingleton <Object>::s_mutex;

template <class Object>
Static::Storage <bool, SharedSingleton <Object> >
  SharedSingleton <Object>::s_created;

#endif
