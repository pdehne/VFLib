// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADWORKER_VFHEADER__
#define __VF_THREADWORKER_VFHEADER__

#include "vf/vf_Bind.h"
#include "vf/vf_CatchAny.h"
#include "vf/vf_Function.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Worker.h"

#include "vf/vf_Thread.h" // is this needed?

//
// Worker that comes with its own thread of execution used
// to process calls. When there are no calls to process,
// an idle function will run. The idle function must either
// return quickly, or periodically call the interruptionPoint()
// function in order to stop what it is doing in order to
// process new calls.
//

namespace detail {

template <class ThreadType>
class ThreadWorker : public Worker
{
public:
  explicit ThreadWorker (const char* szName)
    : Worker (szName)
    , m_thread (szName)
    , m_calledStart (false)
    , m_calledStop (false)
    , m_shouldStop (false)
  {
  }

  ~ThreadWorker ()
  {
    stop_and_wait ();
  }

  //
  // Starts the worker.
  //
  // #1 Worker is opened and init function queued as call.
  //
  // #2 Worker calls are processed on the thread.
  //
  // #3 The idle function is called on the thread after
  //    all current Worker calls finish processing.
  //
  // #4 The exit function is called on the thread when
  //    the Worker is stopped and there are no more calls.
  //
  // #5 start() MUST be called once.
  //
  void start (Function worker_idle,
              Function worker_init,
              Function worker_exit)
  {
    {
      VF_NAMESPACE::ScopedLock lock (m_mutex);
      // start() MUST be called.
      fatal_vfassert (!m_calledStart);
      m_calledStart = true;
    }

    m_idle = worker_idle;
    m_exit = worker_exit;
    open ();
    call (worker_init);
    m_thread.start (Bind (&ThreadWorker::run, this));
  }

  //
  // Stop the thread and optionally wait until it exits.
  // It is safe to call this function at any time and as many times as desired.
  // It is an error to call stop(true) from inside any of the worker functions.
  //
  // During a call to stop() the Worker is closed, and attempts to
  // queue new calls will throw a runtime exception.
  //
  void stop (const bool wait)
  {
    {
      VF_NAMESPACE::ScopedLock lock (m_mutex);

      // start() MUST be called.
      fatal_vfassert (m_calledStart);

      // can't call stop(true) from within a thread function
      fatal_vfassert (!wait || !m_thread.isTheCurrentThread ());

      if (!m_calledStop)
      {
        m_calledStop = true;

        {
          VF_NAMESPACE::ScopedUnlock unlock (m_mutex); // getting fancy

          // Atomically queue a stop and close the worker.
          VF_NAMESPACE::ScopedLock lock (ThreadWorker::getMutex ());
        
          call (&ThreadWorker::do_stop, this);

          close ();
        }
      }
    }

    if (wait)
      m_thread.join ();
  }

  // Sugar
  void stop_request () { stop (false); }
  void stop_and_wait () { stop (true); }

  // Should be called periodically by the idle function.
  // There are three possible results:
  //
  // #1 Returns false. The idle function may continue or return.
  // #2 Returns true. The idle function should return as soon as possible.
  // #3 Throws a Thread::Interruption exception.
  //
  bool interruptionPoint ()
  {
    return m_thread.interruptionPoint ();
  }

  // Interrupts the idle function by queueing a call that does nothing.
  void interrupt ()
  {
    call (Function::None ());
  }

private:
  void reset ()
  {
  }

  void signal ()
  {
    m_thread.interrupt ();
  }

  void do_stop ()
  {
    m_shouldStop = true;
  }

  void run ()
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
      catch (VF_NAMESPACE::Thread::Interruption&)
      {
        // loop
      }
    }

    m_exit ();
  }

private:
  bool m_calledStart;
  bool m_calledStop;
  bool m_shouldStop;
  VF_NAMESPACE::Mutex m_mutex;
  ThreadType m_thread;
  Function m_idle;
  Function m_exit;
};

}

#if 0
class ThreadWorker : public Worker
{
public:
  explicit ThreadWorker (const char* szName = "");

  ~ThreadWorker ();

  //
  // Starts the worker.
  //
  // #1 Worker is opened and init function queued as call.
  //
  // #2 Worker calls are processed on the thread.
  //
  // #3 The idle function is called on the thread after
  //    all current Worker calls finish processing.
  //
  // #4 The exit function is called on the thread when
  //    the Worker is stopped and there are no more calls.
  //
  // #5 start() may only be called once.
  //
  void start (Function worker_idle,
              Function worker_init = Function::None(),
              Function worker_exit = Function::None());

  // Stop the thread and optionally wait until it exits.
  // All thread objects must eventually be called with stop(true).
  // It is an error to call stop(true) from inside any thread_ function.
  // It is ok to call stop() if start() was never called.
  void stop (const bool wait = true);

  // Sugar
  void stop_request () { stop (false); }
  void stop_and_wait () { stop (true); }

  // Interrupts the idle function by queueing a call that does nothing.
  // TODO: SEE IF THIS IS NEEDED?!
  void interrupt ()
  {
    call (Function::None ());
  }

  // Should be called periodically by the idle function.
  // There are three possible results:
  //
  // #1 Returns false. The idle function may continue or return.
  // #2 Returns true. The idle function should return as soon as possible.
  // #3 Throws a Thread::Interruption exception.
  //
  bool interruptionPoint ();

private:
  void reset ();
  void signal ();
  void do_stop ();
  void run ();

private:
  bool m_calledStart;
  bool m_calledStop;
  bool m_shouldStop;
  Mutex m_mutex;
  Thread m_thread;
  Function m_idle;
  Function m_exit;
};
#endif

//------------------------------------------------------------------------------

#if VF_HAVE_JUCE
typedef detail::ThreadWorker <Juce::Thread> BoostWorker;
typedef detail::ThreadWorker <Juce::Thread> SoftWorker;

#elif VF_HAVE_BOOST
typedef detail::ThreadWorker <Boost::Thread> BoostWorker;
typedef detail::ThreadWorker <Boost::Thread> SoftWorker;

#endif

#endif
