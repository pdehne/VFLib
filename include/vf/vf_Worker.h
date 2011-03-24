// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
  // This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_WORKER_VFHEADER__
#define __VF_WORKER_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_Bind.h"
#include "vf/vf_Function.h"
#include "vf/vf_List.h"
#include "vf/vf_LockFreeAllocator.h"
#include "vf/vf_LockFreeQueue.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Thread.h"

//
// Queue that executes functors on another thread, with these invariants:
//
// #1 Functors can be appended to queues from any thread, using call().
//
// #2 Functors execute and get destroyed during process().
//
// #3 Functors queued by the same thread always execute in order.
//
// #4 Functors queued from the thread that last called process() will
//    execute immediately before call() returns. This will cause pending
//    functors to execute first, to preserve the order of calls. 
//
// #5 It is an error to queue a functor into a closed queue.
//
// Derived classes are responsible for implementing signal() and reset()
// in order to guarantee that the queue's process() function is eventually
// called.
//
class Worker
{
public:
  typedef Function <void (void)> func_t;

  explicit Worker (const char* szName = "");
  ~Worker ();

  // used for diagnostics in Listener
  bool in_process () const { return m_in_process.isSet(); }

  //
  // Add the functor without executing immediately.
  //
  void queuef (const func_t& f);

  // Add a functor to the queue. It may be executed immediately.
  //
  // Functors MUST NOT cause thread interruptions.
  //
  void callf (const func_t& f);

  // Sugar for calling functions with arguments.

  template <class Fn>
  void queue (Fn const& f)
  { queuef (bind (f)); }

  template <>
  void queue (const func_t& f)
  { queuef (f); }

  template <class Fn, typename  T1>
  void queue (Fn f,    const T1& t1)
  { queuef (bind (f, t1)); }

  template <class Fn, typename  T1, typename  T2>
  void queue (Fn f,   const T1& t1, const T2& t2)
  { queuef (bind (f, t1, t2)); }

  template <class Fn, typename  T1, typename  T2, typename  T3>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3)
  { queuef (bind (f, t1, t2, t3)); }

  template <class Fn, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void queue (Fn f,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queuef (bind (f, t1, t2, t3, t4)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queuef (bind (f, t1, t2, t3, t4, t5)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queuef (bind (f, t1, t2, t3, t4, t5, t6)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queuef (bind (f, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queuef (bind (f, t1, t2, t3, t4, t5, t6, t7, t8)); }

  template <class Fn>
  void call (Fn const& f)
  { callf (bind (f)); }

  template <>
  void call (const func_t& f)
  { callf (f); }

  template <class Fn, typename  T1>
  void call (Fn f,    const T1& t1)
  { callf (bind (f, t1)); }

  template <class Fn, typename  T1, typename  T2>
  void call (Fn f,    const T1& t1, const T2& t2)
  { callf (bind (f, t1, t2)); }

  template <class Fn, typename  T1, typename  T2, typename  T3>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3)
  { callf (bind (f, t1, t2, t3)); }

  template <class Fn, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void call (Fn f,    const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { callf (bind (f, t1, t2, t3, t4)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { callf (bind (f, t1, t2, t3, t4, t5)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { callf (bind (f, t1, t2, t3, t4, t5, t6)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { callf (bind (f, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { callf (bind (f, t1, t2, t3, t4, t5, t6, t7, t8)); }

protected:
  // Derived class calls this when the queue is signaled,
  // or whenever it wants to. It is disallowed to call
  // process() from more than one thread simultaneously.
  // 
  bool process ();

  // This function should be called as early as possible.
  // Closing the Worker is done for diagnostics, it allows
  // detection of when new calls are being made when they
  // shouldn't be.
  //
  void close ();

  // The worker is in the signaled state when there are unprocessed
  // work items, and in the reset state when the list of work items
  // transitions to empty.
  //
  // Derived classes provide these functions to know when to call process().
  //
  // Due to the lock-free implementation, and also because of the
  // causing sychronous calls from the process thread, it is possible
  // for the worker to become signaled and then see an empty queue.
  //
  virtual void reset () = 0;
  virtual void signal () = 0;

private:
  // List of fixed-size functors
  struct Call;
  typedef LockFree::Queue <Call> Calls;
  struct Call : Calls::Node, func_t
    { Call (func_t const& c) : Function (c) { } };

private:
  void do_reset ();
  void do_signal ();
  bool do_process ();
  void do_queue (Call* c);
  void do_call (Call* c);

private:
  const char* m_szName; // for debugging
  volatile Thread::id m_id;
  Calls m_list;
  Atomic::Flag m_signal;
  Atomic::Flag m_closed;
  Atomic::Flag m_in_process;
};

#endif
