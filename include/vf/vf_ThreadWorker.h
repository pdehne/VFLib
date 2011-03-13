// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADWORKER_VFHEADER__
#define __VF_THREADWORKER_VFHEADER__

#include "vf/vf_Function.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Thread.h"
#include "vf/vf_Worker.h"

// FIX THIS ASAP!
#if VF_HAVE_BOOST && VF_HAVE_JUCE

//
// Worker which uses a thread to process its queue.
// When there is no queue work, the idle function will run.
// The idle function must check for interrupts periodically.
//
// Thread objects with built in thread queue and
// unexpected exception handling (via JUCEApplication)
//
// Note that derived classes must call stop(true) from
// their destructor or else the result is undefined behavior.
class ThreadWorker : public Worker
{
public:
  explicit ThreadWorker (const char* szName = "");
  ~ThreadWorker ();

  // Start the worker.
  // A thread may only be started if it is stopped.
  void start (Function thread_idle,
              Function thread_init = Function(),
              Function thread_exit = Function());

  // Stop the thread and optionally wait until it exits.
  // All thread objects must eventually be called with stop(true).
  // It is an error to call stop(true) from inside any thread_ function.
  // It is ok to call stop() if start() was never called.
  void stop (bool wait = true);
  void stop_request () { stop (false); }
  void stop_and_wait () { stop (true); }

  // restarts the process function. equivalent to call() with no functor.
  void interrupt ();

  // These are here so code can switch back and forth between the
  // exception style and the polling style without change
  virtual bool interrupt_requested () const { return false; }
  virtual void interruption_point () { }

  // Thread idle functions should call this routine and stop
  // what they are doing if it returns true. Depending on the
  // choice of implementation of thread, this might throw
  // boost::thread_interrupted instead of returning, so callers
  // need to handle both cases.
  virtual bool wants_interrupt () { return false; }

protected:
  void do_idle ();

  virtual void idle_and_wait () = 0;

  void interrupt_boost_thread ();

private:
  void do_stop ();
  void do_run ();
  void run ();

private:
  bool m_stop;
  bool m_stopped;
  Mutex m_mutex;
  boost::thread m_thread;
  Function m_thread_idle;
  Function m_thread_exit;
};

//------------------------------------------------------------------------------

//
// Thread which uses boost::thread interruption points
//
class BoostWorker : public ThreadWorker
{
public:
  explicit BoostWorker (const char* szName = "");
  ~BoostWorker ();

  void interruption_point () { interrupt_boost_thread (); }

private:
  void signal ();
  void reset ();
  void idle_and_wait ();
};

//------------------------------------------------------------------------------

//
// Thread which has 'soft' interruption points (i.e. requires polling).
// However, it still uses a waitable event when it goes to sleep.
//
class SoftWorker : public ThreadWorker
{
public:
  explicit SoftWorker (const char* szName = "");
  ~SoftWorker ();

  // This has to be called periodically by the thread
  // idle function in order to know if it should return.
  bool interrupt_requested () const { return m_interrupt_requested; }

private:
  void signal ();
  void reset ();
  void idle_and_wait ();

  WaitableEvent m_event;
  volatile bool m_interrupt_requested;
};

#endif

#endif
