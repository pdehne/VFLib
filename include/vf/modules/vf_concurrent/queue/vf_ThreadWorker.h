// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_THREADWORKER_VFHEADER__
#define __VF_THREADWORKER_VFHEADER__

#include "vf/modules/vf_concurrent/queue/vf_Worker.h"

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

  ~ThreadWorker ();

  //
  // Starts the worker.
  //
  void start (idle_t worker_idle = idle_t::None(),
              init_t worker_init = init_t::None(),
              exit_t worker_exit = exit_t::None());

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
  void stop (bool const wait);

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
  const Thread::Interrupted interruptionPoint ();

  // Interrupts the idle function by queueing a call that does nothing.
  void interrupt ();

private:
  void reset ();
  void signal ();

  void do_stop ();
  void run ();

private:
  bool m_calledStart;
  bool m_calledStop;
  bool m_shouldStop;
  VF_JUCE::CriticalSection m_mutex;
  InterruptibleThread m_thread;
  idle_t m_idle;
  init_t m_init;
  exit_t m_exit;
};

#endif
