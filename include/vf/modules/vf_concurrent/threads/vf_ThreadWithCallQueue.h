// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_THREADWITHCALLQUEUE_VFHEADER
#define VF_THREADWITHCALLQUEUE_VFHEADER

#include "vf_CallQueue.h"

//==============================================================================
/** An InterruptibleThread with a CallQueue.

    This combines an InterruptibleThread with a CallQueue, allowing functors to
    be queued for asynchronous execution on the thread.

    The thread runs an optional user-defined idle function, which must regularly
    check for an interruption using the InterruptibleThread interface. When an
    interruption is signaled, the idle function returns and the CallQueue is
    synchronized. Then, the idle function is resumed.

    When the ThreadWithCallQueue first starts up, an optional user-defined
    initialization function is executed on the thread. When the thread exits,
    a user-defined exit function may be executed on the thread.
*/
class ThreadWithCallQueue : public CallQueue
{
public:
  typedef Function <const InterruptibleThread::Interrupted (void)> idle_t;
  typedef Function <void (void)> init_t;
  typedef Function <void (void)> exit_t;

  /** Create a ThreadWithCallQueue.

      @param name The name of the InterruptibleThread and CallQueue, used
                  for diagnostics when debugging.
  */
  explicit ThreadWithCallQueue (String name);

  /** Destroy a ThreadWithCallQueue.

      If the thread is still running it is stopped. The destructor blocks
      until the thread exits cleanly.
  */
  ~ThreadWithCallQueue ();

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
  // During a call to stop() the CallQueue is closed, and attempts to
  // queue new calls will throw a runtime exception.
  //
  // Any listeners registered on the worker need to be removed
  // before stop is called
  //
  void stop (bool const wait);

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
  const InterruptibleThread::Interrupted interruptionPoint ();

  // Interrupts the idle function by queueing a call that does nothing.
  void interrupt ();

private:
  void signal ();
  void reset ();

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
