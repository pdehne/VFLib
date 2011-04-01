// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_BOOST

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

bool BoostThread::PollingBased::wait (int milliseconds, BoostThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted = false;

  return interrupted;
}

void BoostThread::PollingBased::interrupt (BoostThread& thread)
{
}

ThreadBase::Interrupted BoostThread::PollingBased::interruptionPoint (BoostThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted = false;

  return ThreadBase::Interrupted (interrupted);
}

//------------------------------------------------------------------------------

BoostThread::BoostThread (String const& name)
  : m_name (name)
{
}

void BoostThread::start (const Function <void (void)>& f)
{
  m_thread = boost::thread (f);
}

void BoostThread::join ()
{
  interrupt ();

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
  try
  {
    boost::this_thread::interruption_point ();
  }
  catch (boost::thread_interrupted&)
  {
    // re-throw it as a boost-independent object
    throw ThreadBase::Interruption();
  }

  return BoostThread::Interrupted (false);
}

}

#endif

END_VF_NAMESPACE

