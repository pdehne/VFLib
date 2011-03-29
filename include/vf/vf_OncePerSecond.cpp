// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Atomic.h"
#include "vf/vf_Bind.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_OncePerSecond.h"
#include "vf/vf_StaticMutex.h"
#include "vf/vf_Thread.h"

//------------------------------------------------------------------------------

class OncePerSecond::TimerSingleton : public SharedObject
{
private:
  TimerSingleton () : m_thread (__FILE__)
  {
    m_started = false;

    vfassert (s_instance == 0);
    s_instance = this;
  }

  ~TimerSingleton ()
  {
    if (m_started)
      m_thread.join ();

    vfassert (m_list.empty ());
    vfassert (s_instance == this);
  }

  void start ()
  {
    m_started = true;
    m_thread.start (bind (&TimerSingleton::run, this));
  }

  void run ()
  {
    for(;;)
    {
      const bool interrupted = m_thread.wait (1000);

      if (interrupted)
        break;

      notify();
    }
  }

  void notify ()
  {
    ScopedLock lock (m_mutex);

    for (List::iterator iter = m_list.begin(); iter != m_list.end(); ++iter)
      iter->object->doOncePerSecond ();
  }

  void destroySharedObject ()
  {
    StaticMutex::ScopedLock lock (s_mutex);

    if (!isBeingReferenced ())
    {
      delete this;
    }
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

    m_list.remove (elem);
  }

  static TimerPtr getInstance ()
  {
    TimerPtr instance;

    StaticMutex::ScopedLock lock (s_mutex);

    instance = const_cast <TimerSingleton*> (s_instance);

    if (!instance)
    {
      instance = new TimerSingleton;
      instance->start ();
    }

    return instance;
  }

private:
  bool m_started;
  BoostThread m_thread;
  Mutex m_mutex;
  List m_list;

  static StaticMutex s_mutex;
  static volatile TimerSingleton* s_instance;
};

StaticMutex OncePerSecond::TimerSingleton::s_mutex;
volatile OncePerSecond::TimerSingleton* OncePerSecond::TimerSingleton::s_instance;

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
