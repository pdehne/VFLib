// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_ThreadWorker.h"

#if 0

ThreadWorker::ThreadWorker (const char* szName)
  : Worker (szName)
  , m_thread (szName)
  , m_calledStart (false)
  , m_calledStop (false)
  , m_shouldStop (false)
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
  {
    ScopedLock lock (m_mutex);

    fatal_vfassert (!m_calledStart);

    m_calledStart = true;

    m_idle = worker_idle;
    m_exit = worker_exit;

    open ();

    call (worker_init);

    m_thread.start (Bind (&ThreadWorker::run, this));
  }
}

void ThreadWorker::stop (const bool wait)
{
  {
    ScopedLock lock (m_mutex);

    fatal_vfassert (m_calledStart);

    // can't call stop(true) from within a thread function
    vfassert (!wait || !m_thread.isTheCurrentThread ());

    if (!m_calledStop)
    {
      m_calledStop = true;

      // Atomically queue a stop and close the worker.
      {
        ScopedLock lock (ThreadWorker::getMutex ());
      
        call (&ThreadWorker::do_stop, this);

        close ();
      }
    }
  }

  if (wait)
    m_thread.join ();
}

bool ThreadWorker::interruptionPoint ()
{
  return m_thread.interruptionPoint ();
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
  m_shouldStop = true;
}

void ThreadWorker::run ()
{
  for (;;)
  {
    Worker::process ();

    if (m_shouldStop)
      break;

    try
    {
      m_idle ();

      if (!interruptionPoint ())
      {
        m_thread.wait ();
      }
    }
    catch (vf::Thread::Interruption&)
    {
      // loop
    }
  }

  m_exit ();
}

#endif

//------------------------------------------------------------------------------

END_VF_NAMESPACE

