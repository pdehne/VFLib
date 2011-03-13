// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_CatchAny.h"
#include "vf/vf_ThreadWorker.h"

ThreadWorker::ThreadWorker (const char* szName)
  : Worker (szName)
  , m_thread (szName)
  , m_stopped (true)
{
}

ThreadWorker::~ThreadWorker ()
{
  stop_and_wait ();
}

void ThreadWorker::start (Function worker_idle,
                          Function worker_init,
                          Function worker_exit)
{
  m_mutex.enter ();

  // May not start unless stopped.
  jassert (m_stopped);
  m_stopped = false;

  m_worker_idle = worker_idle;
  m_worker_exit = worker_exit;

  open ();

  m_stop = false;

  // Call the init function so it happens on the thread.
  // NOTE vf::Thread wraps the thread in a CatchAny
  call (worker_init);

  m_thread.start (Bind (&ThreadWorker::run, this));

  m_mutex.exit ();
}

void ThreadWorker::stop (bool wait)
{
  m_mutex.enter ();

  // Check if we started.
  if (!m_stopped)
  {
    // can't call stop(true) from within a thread function
    jassert (!wait || !m_thread.isTheCurrentThread ());
      //m_thread.getId() != Boost::CurrentThread::getId());

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

bool ThreadWorker::interruptionPoint ()
{
  return m_thread.interruptionPoint();
}

void ThreadWorker::do_idle ()
{
  m_worker_idle ();
}

void ThreadWorker::signal ()
{
  m_thread.interrupt ();
}

void ThreadWorker::reset ()
{
}

void ThreadWorker::do_stop ()
{
  m_stop = true;
}

void ThreadWorker::run ()
{
  // Not needed because vf::Thread does it
  //CatchAny (Bind (&ThreadWorker::do_run, this));
  for (;;)
  {
    Worker::process ();

    if (m_stop)
      break;

    try
    {
      // WRONG!!!
      // This is here so we can immediately take care
      // of anything put into the ThreadQueue while
      // the ThreadQueue was being processed.
      //
      // WRONG! This could cause the idle function
      // to get starved, and also the way that the
      // thread queue works with synchronous calls
      // it should not be needed!!
      //boost::this_thread::interruption_point();

      //do_idle ();
      m_worker_idle ();

      // sleep() is a a boost interruption point
      /*
      boost::this_thread::sleep (
        boost::posix_time::ptime (
          boost::date_time::max_date_time)); // forever
      */
      m_thread.wait ();
    }
    /* SHOULD NEVER SEE THIS HERE
    catch (boost::thread_interrupted&)
    {
    }
    */
    catch (vf::Thread::Interruption&)
    {
      // loop again
    }
  }

  m_worker_exit ();

  m_stopped = true;
}

//------------------------------------------------------------------------------

END_VF_NAMESPACE

