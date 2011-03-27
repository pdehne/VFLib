// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_BOOST

#include "vf/vf_BoostThread.h"
#include "vf/vf_CatchAny.h"

BoostThread::BoostThread (const VF_NAMESPACE::String& name)
{
}

BoostThread::~BoostThread ()
{
  join ();
}

void BoostThread::start (const Function <void (void)>& f)
{
  m_thread = boost::thread (f);
}

void BoostThread::join ()
{
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

void BoostThread::wait ()
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

void BoostThread::interrupt ()
{
  m_thread.interrupt ();
}

BoostThread::Interrupted BoostThread::interruptionPoint ()
{
  vfassert (isTheCurrentThread ());

  try
  {
    boost::this_thread::interruption_point ();
  }
  catch (boost::thread_interrupted&)
  {
    // re-throw it as a boost-independent object
    throw Interruption();
  }

  return BoostThread::Interrupted (false);
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

