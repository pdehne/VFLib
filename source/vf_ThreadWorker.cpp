// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_ThreadWorker.h"
#include "vf/vf_TryCatch.h"

ThreadWorker::ThreadWorker (const char* szName)
  : Worker (szName)
  , m_stopped (true)
{
}

ThreadWorker::~ThreadWorker ()
{
  // can't call stop here because any derived
  // class portion will be deleted by now.
}

void ThreadWorker::start (Function thread_idle,
                          Function thread_init,
                          Function thread_exit)
{
  m_mutex.enter ();

  // May not start unless stopped.
  jassert (m_stopped);
  m_stopped = false;

  m_thread_idle = thread_idle;
  m_thread_exit = thread_exit;

  open ();

  m_stop = false;

  // Call the init function so it happens on the thread.
  call (thread_init);

  m_thread = boost::thread (Bind (&ThreadWorker::run, this));

  m_mutex.exit ();
}

void ThreadWorker::stop (bool wait)
{
  m_mutex.enter ();

  // Check if we started.
  if (!m_stopped)
  {
    // can't call stop(true) from within a thread function
    jassert (!wait || m_thread.get_id() != boost::this_thread::get_id());

    // Use the thread queue open status as a flag
    // to know if we already queued a call to do_stop.
    lock ();
    if (!closed ())
    {
      // Queue a stop call, and close the thread queue in one atomic operation.
      call (&ThreadWorker::do_stop, this);
      close ();
    }
    unlock ();

    m_mutex.exit ();

    if (wait)
      m_thread.join ();
  }
  else
  {
    // needed to prevent asserts if the thread is never started
    // or if the thread is already stopped by the time we get here.
    lock ();
    if (!closed ())
      close ();
    unlock ();

    m_mutex.exit ();
  }
}

namespace { void none () { } }

void ThreadWorker::interrupt ()
{
  call (&none);
}

void ThreadWorker::do_idle ()
{
  m_thread_idle ();
}

void ThreadWorker::interrupt_boost_thread ()
{
  m_thread.interrupt ();
}

void ThreadWorker::do_stop ()
{
  m_stop = true;
}

void ThreadWorker::do_run ()
{
  for (;;)
  {
    Worker::process ();

    if (m_stop)
      break;

    idle_and_wait ();
  }

  m_thread_exit ();

  m_stopped = true;
}

void ThreadWorker::run ()
{
  TryCatch (Bind (&ThreadWorker::do_run, this));
}

//------------------------------------------------------------------------------

BoostWorker::BoostWorker (const char* szName) : ThreadWorker (szName)
{
}

BoostWorker::~BoostWorker ()
{
  // required
  stop_and_wait ();
}

void BoostWorker::signal ()
{
  interrupt_boost_thread ();
}

void BoostWorker::reset ()
{
}

void BoostWorker::idle_and_wait ()
{
  try
  {
    // This is here so we can immediately take care
    // of anything put into the ThreadQueue while
    // the ThreadQueue was being processed.
    boost::this_thread::interruption_point();

    do_idle ();

    // sleep() is a a boost interruption point
    boost::this_thread::sleep (
      boost::posix_time::ptime (
        boost::date_time::max_date_time)); // forever
  }
  catch (boost::thread_interrupted&)
  {
  }
  catch (vf::Thread::Interruption&)
  {
  }
}

//------------------------------------------------------------------------------

SoftWorker::SoftWorker (const char* szName)
  : ThreadWorker (szName)
  , m_event (true)
  , m_interrupt_requested (false)
{
}

SoftWorker::~SoftWorker ()
{
  // required
  stop_and_wait ();
}

void SoftWorker::signal ()
{
  m_event.signal ();
  m_interrupt_requested = true;
}

void SoftWorker::reset ()
{
  m_event.reset ();
  m_interrupt_requested = false;
}

void SoftWorker::idle_and_wait ()
{
  // This is here so we can immediately take care
  // of anything put into the ThreadQueue while
  // the ThreadQueue was being processed.
  if (!interrupt_requested ())
  {
    do_idle();

    m_event.wait ();
  }
}

END_VF_NAMESPACE

