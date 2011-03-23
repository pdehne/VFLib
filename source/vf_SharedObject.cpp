// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_ThreadWorker.h"
#include "vf/vf_SharedObject.h"

class SharedObject::Deleter
{
public:
  static void Delete (SharedObject* sharedObject)
  {
    delete sharedObject;
  }
};

namespace {

#if VF_HAVE_JUCE

class SharedObjectDestroyer : DeletedAtShutdown
{
public:
  SharedObjectDestroyer () : m_worker (__FILE__)
  {
    m_worker.start ();
  }

  ~SharedObjectDestroyer ()
  {
    // is this needed?
    //m_worker.stop_and_wait ();
  }

  void Delete (SharedObject* sharedObject)
  {
    m_worker.call (&SharedObject::Deleter::Delete, sharedObject);
  }

  juce_DeclareSingleton (SharedObjectDestroyer, false)

private:
  ThreadWorkerType <Juce::ThreadType <Juce::Thread::PollingBased> >
    m_worker;
};

juce_ImplementSingleton (SharedObjectDestroyer)

#else

#error "Missing Singleton"

#endif

}

void SharedObject::destroySharedObject ()
{
  SharedObjectDestroyer::getInstance()->Delete (this);
}

END_VF_NAMESPACE
