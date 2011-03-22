// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_BOOST

#include "vf/vf_BoostThread.h"
#include "vf/vf_CatchAny.h"

namespace Boost {

Thread::Thread (const VF_NAMESPACE::String& name)
{
}

Thread::~Thread ()
{
  join ();
}

void Thread::join ()
{
  m_thread.join ();
}

Thread::id Thread::getId ()
{
  return m_thread.get_id ();
}

bool Thread::isTheCurrentThread () const
{
  return m_thread.get_id () == boost::this_thread::get_id ();
}

void Thread::wait ()
{
  try
  {
    // sleep until interrupted
    boost::this_thread::sleep (
      boost::posix_time::ptime (
        boost::date_time::max_date_time));
  }
  catch (boost::thread_interrupted&)
  {
    // wake up
  }
}

void Thread::interrupt ()
{
  m_thread.interrupt ();
}

Thread::Interrupted Thread::interruptionPoint ()
{
  vfassert (isTheCurrentThread ());

  try
  {
    boost::this_thread::interruption_point ();
  }
  catch (boost::thread_interrupted&)
  {
    // re-throw it as a boost-independent object
    throw detail::Thread::Interruption();
  }

  return Thread::Interrupted (false);
}

namespace CurrentThread {

Boost::Thread::id getId ()
{
  return boost::this_thread::get_id ();
}

Thread::Interrupted interruptionPoint ()
{
  try
  {
    boost::this_thread::interruption_point ();
  }
  catch (boost::thread_interrupted&)
  {
    // re-throw it as a boost-independent object
    throw detail::Thread::Interruption();
  }

  return Thread::Interrupted (false);
}

}

}

#endif

END_VF_NAMESPACE

