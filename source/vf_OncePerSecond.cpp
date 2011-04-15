// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Bind.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_OncePerSecond.h"
#include "vf/vf_SharedSingleton.h"
#include "vf/vf_Thread.h"

//------------------------------------------------------------------------------

class OncePerSecond::TimerSingleton
  : public SharedSingleton <OncePerSecond::TimerSingleton>
{
private:
  TimerSingleton ()
    : SharedSingleton (persistAfterCreation)
    , m_thread (__FILE__)
  {
    m_thread.start (bind (&TimerSingleton::run, this));
  }

  ~TimerSingleton ()
  {
    m_thread.join ();

    vfassert (m_list.empty ());
  }

  void run ()
  {
    try
    {
      for(;;)
      {
        const bool interrupted = m_thread.wait (1000);

        if (interrupted)
          break;

        notify();
      }
    }
    catch (Thread::Interrupted&)
    {
    }
  }

  void notify ()
  {
    ScopedLock lock (m_mutex);

    for (List::iterator iter = m_list.begin(); iter != m_list.end(); ++iter)
      iter->object->doOncePerSecond ();
  }

public:
  void insert (Elem* elem)
  {
    ScopedLock lock (m_mutex);

    m_list.push_back (elem);
  }

  void remove (Elem* elem)
  {
    ScopedLock lock (m_mutex);

    m_list.erase (elem);
  }

  static TimerSingleton* createInstance ()
  {
    return new TimerSingleton;
  }

private:
  BoostThreadType <BoostThread::PollingBased> m_thread;
  Mutex m_mutex;
  List m_list;
};

//------------------------------------------------------------------------------

OncePerSecond::OncePerSecond ()
{
  m_elem.instance = TimerSingleton::getInstance ();
  m_elem.object = this;
}

OncePerSecond::~OncePerSecond ()
{
}

void OncePerSecond::startOncePerSecond ()
{
  m_elem.instance->insert (&m_elem);
}

void OncePerSecond::endOncePerSecond ()
{
  m_elem.instance->remove (&m_elem);
}

//------------------------------------------------------------------------------

END_VF_NAMESPACE
