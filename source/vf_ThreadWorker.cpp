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

void ThreadWorker::reset ()
{
}

void ThreadWorker::signal ()
{
  m_thread.interrupt ();
}

void ThreadWorker::do_stop ()
{
  m_stop = true;
}

void ThreadWorker::run ()
{
  for (;;)
  {
    Worker::process ();

    if (m_stop)
      break;

    try
    {
      m_worker_idle ();

      m_thread.wait ();
    }
    catch (vf::Thread::Interruption&)
    {
      // loop
    }
  }

  m_worker_exit ();

  m_stopped = true;
}

//------------------------------------------------------------------------------

END_VF_NAMESPACE

