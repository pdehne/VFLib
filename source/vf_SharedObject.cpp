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

class SharedObject::Singleton : public SharedSingleton <Singleton>
{
private:
  Singleton ()
    : m_worker (__FILE__)
    , SharedSingleton (persistAfterCreation)
  {
    m_worker.start ();
  }

  ~Singleton ()
  {
    // is this needed?
    m_worker.stop_and_wait ();
  }

private:
  friend class SharedSingleton <Singleton>;

  static Singleton* createInstance ()
  {
    return new Singleton;
  }

  static void doDelete (SharedObject* sharedObject)
  {
    delete sharedObject;
  }

public:
  inline Worker& getWorker () { return m_worker; }

  void Delete (SharedObject* sharedObject)
  {
    m_worker.call (&Singleton::doDelete, sharedObject);
  }

private:
  ThreadWorkerType <BoostThreadType <BoostThread::PollingBased> > m_worker;
};

//------------------------------------------------------------------------------

void SharedObject::destroySharedObject ()
{
  Singleton::getInstance()->Delete (this);
}

END_VF_NAMESPACE
