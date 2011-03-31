// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SHAREDSINGLETON_VFHEADER__
#define __VF_SHAREDSINGLETON_VFHEADER__

#include "vf/vf_BoostThread.h"
#include "vf/vf_SharedObject.h"
#include "vf/vf_StaticMutex.h"
#include "vf/vf_ThreadWorker.h"

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
  typedef SharedObjectPtr <Object> Ptr;

  explicit SharedSingleton (const bool persistAfterCreation = true)
    : m_persistAfterCreation (persistAfterCreation)
  {
    vfassert (s_instance == 0);
  }

  ~SharedSingleton ()
  {
    s_instance = 0;
  }

public:
  static Ptr getInstance ()
  {
    Ptr instance;

    StaticMutex <Object>::ScopedLockType lock;

    if (!s_instance)
    {
      s_instance = Object::createInstance ();

      if (s_instance->m_persistAfterCreation)
        s_persistantReference.setInstance (s_instance);
    }

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
  const bool m_persistAfterCreation;

  class PersistantReference
  {
  public:
    // Intentionally lacking a constructor.
    // Inited to zero from static storage duration.
    ~PersistantReference ()
    {
      if (m_instance != 0)
        m_instance->decReferenceCount ();
    }

    void setInstance (Object* object)
    {
      m_instance = object;
      m_instance->incReferenceCount ();
    }

  private:
    Object* m_instance;
  };

  static Object* s_instance;
  static StaticMutex <Object> s_mutex;
  static PersistantReference s_persistantReference;
};

template <class Object>
Object* SharedSingleton <Object>::s_instance;

template <class Object>
StaticMutex <Object> SharedSingleton <Object>::s_mutex;

template <class Object>
typename SharedSingleton <Object>::PersistantReference
  SharedSingleton <Object>::s_persistantReference;

//------------------------------------------------------------------------------

// This is here to break a cyclic #include.

class SharedObject::Singleton : public SharedSingleton <Singleton>
{
private:
  Singleton ();
  ~Singleton ();

private:
  friend class SharedSingleton <Singleton>;

  static Singleton* createInstance ();
  static void doDelete (SharedObject* sharedObject);

public:
  inline Worker& getWorker () { return m_worker; }

  void Delete (SharedObject* sharedObject);

private:
  ThreadWorkerType <BoostThread> m_worker;
};

#endif
