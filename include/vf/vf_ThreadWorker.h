// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADWORKER_VFHEADER__
#define __VF_THREADWORKER_VFHEADER__

#include "vf/vf_Function.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Thread.h"
#include "vf/vf_Worker.h"

//
// Worker that comes with its own thread of execution used
// to process calls. When there are no calls to process,
// an idle function will run. The idle function must either
// return quickly, or periodically call the interruptionPoint()
// function in order to stop what it is doing in order to
// process new calls.
//
class ThreadWorker : public Worker
{
public:
  explicit ThreadWorker (const char* szName = "");
  ~ThreadWorker ();

  // Start the worker. This should only hapen once
  // A thread may only be started if it is stopped.
  // TODO: Do this right at construction.
  void start (Function worker_idle,
              Function worker_init = Function::None(),
              Function worker_exit = Function::None());

  // Stop the thread and optionally wait until it exits.
  // All thread objects must eventually be called with stop(true).
  // It is an error to call stop(true) from inside any thread_ function.
  // It is ok to call stop() if start() was never called.
  void stop (bool wait = true);
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

  // These are here so code can switch back and forth between the
  // exception style and the polling style without change
protected:
  virtual bool interrupt_requested () const { return false; }
  virtual void interruption_point () { }

protected:
  void do_idle ();

  virtual void idle_and_wait () { }

private:
  void signal ();
  void reset ();
  void do_stop ();
  void do_run ();
  void run ();

private:
  bool m_stop;
  bool m_stopped;
  Mutex m_mutex;
  Thread m_thread;
  Function m_worker_idle;
  Function m_worker_exit;
};

//------------------------------------------------------------------------------

typedef ThreadWorker BoostWorker;
typedef ThreadWorker SoftWorker;

#endif
