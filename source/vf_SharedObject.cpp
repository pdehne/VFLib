// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_BoostThread.h"
#include "vf/vf_SharedObject.h"
#include "vf/vf_SharedSingleton.h"
#include "vf/vf_ThreadWorker.h"

//------------------------------------------------------------------------------

class SharedObject::Deleter : LeakChecked <Deleter>
{
private:
  typedef SpinLock LockType;

  Deleter () : m_worker ("Deleter")
  {
  }

  ~Deleter ()
  {
    m_worker.stop_and_wait ();
  }

  void construct ()
  {
    m_worker.start ();
  }

private:
  static void doDelete (SharedObject* sharedObject)
  {
    delete sharedObject;
  }

public:
  typedef vf::SharedObjectPtr <Deleter> Ptr;

  Worker& getWorker ()
  {
    return m_worker;
  }

  void incReferenceCount ()
  {
    m_refs.addref ();
  }

  void decReferenceCount ()
  {
    if (m_refs.release ())
    {
      bool destroy;

      {
        LockType::ScopedLockType lock (*s_mutex);
      
        if (m_refs.is_signaled ())
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
        delete this;
    }
  }

  void Delete (SharedObject* sharedObject)
  {
    //m_worker.call (&Deleter::doDelete, sharedObject);
    delete sharedObject;
  }

  static Ptr getInstance ()
  {
    Ptr instance;

    instance = s_instance;

    if (instance == nullptr)
    {
      LockType::ScopedLockType lock (*s_mutex);

      instance = s_instance;

      if (instance == nullptr)
      {
        s_instance = new Deleter;

        instance = s_instance;

        instance->construct ();
      }
    }

    return instance;
  }

private:
  Atomic::Counter m_refs;
  ThreadWorkerType <BoostThreadType <BoostThread::PollingBased> > m_worker;

  static Deleter* s_instance;
  static Static::Storage <SpinLock, Deleter> s_mutex;
};

SharedObject::Deleter* SharedObject::Deleter::s_instance;
Static::Storage <SpinLock, SharedObject::Deleter> SharedObject::Deleter::s_mutex;

//------------------------------------------------------------------------------

SharedObject::SharedObject ()
{
  //Deleter::getInstance()->incReferenceCount();
}

SharedObject::~SharedObject ()
{
  vfassert (m_refs.is_reset ());
  //Deleter::getInstance()->decReferenceCount();
}

void SharedObject::destroySharedObject ()
{
  Deleter::getInstance()->Delete (this);
}

END_VF_NAMESPACE
