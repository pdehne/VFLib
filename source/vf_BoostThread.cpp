// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_BoostThread.h"
#include "vf/vf_CatchAny.h"

//------------------------------------------------------------------------------

bool BoostThread::ExceptionBased::wait (int milliseconds, BoostThread& thread)
{
  CurrentBoostThread::sleep (milliseconds);

  return false;
}

void BoostThread::ExceptionBased::interrupt (BoostThread& thread)
{
  thread.m_thread.interrupt ();
}

ThreadBase::Interrupted BoostThread::ExceptionBased::interruptionPoint (BoostThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  boost::this_thread::interruption_point ();

  return ThreadBase::Interrupted (false);
}

//------------------------------------------------------------------------------

BoostThread::PollingBased::PollingBased ()
  : m_state (stateRun)
{
}

bool BoostThread::PollingBased::wait (int milliseconds, BoostThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted;

  {
    boost::unique_lock <boost::mutex> lock (m_mutex);

    if (m_state == stateRun)
    {
      m_state = stateWait;

      if (milliseconds >= 0)
      {
        boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(milliseconds);
        
        do
        {
          interrupted = m_cond.timed_wait (lock, timeout);
        } while (m_state == stateWait && interrupted);
      }
      else
      {
        while(m_state == stateWait)
          m_cond.wait (lock);
        interrupted = true;
      }

      if (!interrupted)
        m_state = stateRun;
    }
    else
    {
      vfassert (m_state == stateInterrupt);

      m_state = stateRun;

      interrupted = true;
    }
  }

  return interrupted;
}

void BoostThread::PollingBased::interrupt (BoostThread& thread)
{
  boost::unique_lock <boost::mutex> lock (m_mutex);
  
  if (m_state == stateWait)
  {
    m_state = stateRun;

    m_cond.notify_one ();
  }
  else
  {
    m_state = stateInterrupt;
  }
}

ThreadBase::Interrupted BoostThread::PollingBased::interruptionPoint (BoostThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted;

  boost::unique_lock <boost::mutex> lock (m_mutex);
  
  if (m_state == stateRun)
  {
    interrupted = false;
  }
  else
  {
    vfassert (m_state == stateInterrupt);

    m_state = stateRun;

    interrupted = true;
  }

  return ThreadBase::Interrupted (interrupted);
}

//------------------------------------------------------------------------------

BoostThread::BoostThread (String name)
  : m_name (name)
{
}

void BoostThread::start (Function <void (void)> const& f)
{
  m_thread = boost::thread (boost::bind (&BoostThread::run, this, f));

  // kick the thread into gear, AFTER we assign data members, to prevent a data race.
  m_runEvent.signal ();
}

void BoostThread::run (Function <void (void)> f)
{
  VF_JUCE::Thread::setCurrentThreadName (m_name);

  m_runEvent.wait ();

  f ();
}

void BoostThread::join ()
{
  interrupt ();

  // signal just in case
  m_runEvent.signal ();

  m_thread.join ();
}

BoostThread::id BoostThread::getId ()
{
  return m_thread.get_id ();
}

bool BoostThread::isTheCurrentThread () const
{
  return m_thread.get_id () == boost::this_thread::get_id ();
}

//------------------------------------------------------------------------------

namespace CurrentBoostThread {

BoostThread::id getId ()
{
  return boost::this_thread::get_id ();
}

BoostThread::Interrupted interruptionPoint ()
{
  boost::this_thread::interruption_point ();

  return BoostThread::Interrupted (false);
}

}

END_VF_NAMESPACE

