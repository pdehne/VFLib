// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_THREADWORKER_VFHEADER__
#define __VF_THREADWORKER_VFHEADER__

#include "vf/modules/vf_core/functor/vf_Bind.h"
#include "vf/modules/vf_core/diagnostic/vf_CatchAny.h"
#include "vf/modules/vf_core/functor/vf_Function.h"
#include "vf/modules/vf_core/threads/vf_Mutex.h"
#include "vf/modules/vf_concurrent/queue/vf_Worker.h"

#include "vf/modules/vf_core/threads/vf_Thread.h" // is this needed?

//
// Worker that comes with its own thread of execution used
// to process calls. When there are no calls to process,
// an idle function will run. The idle function must either
// return quickly, or periodically return the result of the
// interruptionPoint() function in order to stop what it is
// doing in order to process new calls.
//

// Factored out the types so that callers don't need to
// supply template arguments when they want to use a type,
// for example, idle_t::None()
//
class ThreadWorker : public Worker
{
public:
  typedef Function <const Thread::Interrupted (void)> idle_t;
  typedef Function <void (void)> init_t;
  typedef Function <void (void)> exit_t;

  explicit ThreadWorker (String name);
};

template <class ThreadType>
class ThreadWorkerType : public ThreadWorker
{
public:
  explicit ThreadWorkerType (String name)
    : ThreadWorker (name)
    , m_thread (name)
    , m_calledStart (false)
    , m_calledStop (false)
    , m_shouldStop (false)
  {
  }

  ~ThreadWorkerType ()
  {
    stop_and_wait ();
  }

  //
  // Starts the worker.
  //
  void start (idle_t worker_idle = idle_t::None(),
              init_t worker_init = init_t::None(),
              exit_t worker_exit = exit_t::None())
  {
    {
      // TODO: Atomic for this
      VF_NAMESPACE::Mutex::ScopedLockType lock (m_mutex);
      // start() MUST be called.
      vfassert (!m_calledStart);
      m_calledStart = true;
    }

    m_init = worker_init;
    m_idle = worker_idle;
    m_exit = worker_exit;

    m_thread.start (boost::bind (&ThreadWorkerType::run, this));
  }

  //
  // Stop the thread and optionally wait until it exits.
  // It is safe to call this function at any time and as many times as desired.
  // It is an error to call stop(true) from inside any of the worker functions.
  //
  // During a call to stop() the Worker is closed, and attempts to
  // queue new calls will throw a runtime exception.
  //
  // Any listeners registered on the worker need to be removed
  // before stop is called
  //
  void stop (bool const wait)
  {
    // can't call stop(true) from within a thread function
    vfassert (!wait || !m_thread.isTheCurrentThread ());

    {
      VF_NAMESPACE::Mutex::ScopedLockType lock (m_mutex);

      // start() MUST be called.
      vfassert (m_calledStart);

      // TODO: Atomic for this
      if (!m_calledStop)
      {
        m_calledStop = true;

        {
          Mutex::ScopedUnlockType unlock (m_mutex); // getting fancy

          call (&ThreadWorkerType::do_stop, this);

          // in theory something could slip in here

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
  // If interruptionPoint returns true or throws, it must
  // not be called again before the thread has the opportunity to reset.
  //
  const Thread::Interrupted interruptionPoint ()
  {
    return m_thread.interruptionPoint ();
  }

  // Interrupts the idle function by queueing a call that does nothing.
  void interrupt ()
  {
    call (Function <void (void)>::None ());
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
    m_init ();

    for (;;)
    {
      Worker::process ();

      if (m_shouldStop)
        break;

      try
      {
        // idle_t::None() must return a non signaled Thread::Interrupted.
        Thread::Interrupted interrupted = m_idle ();

        if (!interrupted)
          interrupted = interruptionPoint ();

        if (!interrupted)
          m_thread.wait ();
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
  Mutex m_mutex;
  ThreadType m_thread;
  idle_t m_idle;
  init_t m_init;
  exit_t m_exit;
};

//------------------------------------------------------------------------------

#if 0
typedef ThreadWorkerType <BoostThreadType <BoostThread::ExceptionBased> > ExceptionWorker;
typedef ThreadWorkerType <BoostThreadType <BoostThread::PollingBased> > PollingWorker;
#else
typedef ThreadWorkerType <JuceThreadType <JuceThread::ExceptionBased> > ExceptionWorker;
typedef ThreadWorkerType <JuceThreadType <JuceThread::PollingBased> > PollingWorker;
#endif

#endif