// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_WORKER_VFHEADER__
#define __VF_WORKER_VFHEADER__

#include "vf/vf_Bind.h"
#include "vf/vf_List.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Threads.h"

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
// TODO: CHANGE jassert TO THROW EXCEPTIONS?
//
class Worker
{
private:
  class Call;
  typedef List <Call> Calls;

  // Abstract nullary functor which can be a list element.
  class Call : public Calls::Node
  { 
  public:
    virtual ~Call () { }
    virtual void operator ()() = 0;
  };

  // Stores the Functor. It must be copy constructible.
  template <class Functor>
  class StoredCall : public Call
  {
  public:
    explicit StoredCall (Functor const& f) : m_f (f) { }
    void operator()() { m_f.operator()(); }
  private:
    Functor m_f;
  };

private:
  const char* m_szName; // for debugging
  bool m_open;
  bool m_in_process;
  Calls m_calls;
  Mutex m_mutex;
  VF_NAMESPACE::Thread::id m_id;

private:
  // Call all the functors in the queue.
  bool do_process (const bool from_call);

  // Queues a call for execution and possibly processes the queue.
  void do_call (Call* element);

protected:
  // Derived classes call these at appropriate times.
  // TODO: Think about always starting out open,
  // remove this function, allow only closing once.
  void open ();
  // This function should be called as early as possible.
  // Closing the Worker is done for diagnostics, it allows
  // detection of when new calls are being made when they
  // shouldn't be.
  // TODO: Consider making this virtual and having the
  // derived class cause the thread to stop ?
  void close ();
  bool closed () const { return !m_open; }

  // Derived class uses this to perform additional synchronization
  void lock () { m_mutex.enter (); }
  void unlock () { m_mutex.exit (); }

  // Derived class calls this when the queue is signaled,
  // or whenever it wants to. It is disallowed to call
  // process() from more than one thread simultaneously.
  // 
public:
  bool process ();

protected:
  // Derived classes implement these functions to know the
  // signaled state:
  //
  // #1 The queue is signaled when there functors to process
  //
  // #2 A call to signal() never happens twice without a call
  //    to reset() in between.
  //
  // #3 Calling process() on a signaled queue will always trigger
  //    a call to reset().
  //
  // #4 The mutex is held during the calls.
  //
  virtual void reset () = 0;
  virtual void signal () = 0;

public:
  explicit Worker (const char* szName = "");
  ~Worker ();

  // used for diagnostics
  bool in_process ();

  // Custom allocator optimized for this pattern of use.
  // This is also used by the listeners system.
private:
  static void* global_alloc (size_t bytes);
  static void  global_free (void* p);

public:
  //
  // callf()
  //
  // Add a functor to the queue. It may be executed immediately.
  //
  template <class Functor>
  void callf (const Functor& f)
  {
    do_call (new (global_alloc (sizeof (StoredCall <Functor>)))
                 StoredCall <Functor> (f));
  }

#if VF_HAVE_BIND
  //
  // Sugar for calling functions with arguments.
  //

  template <class Fn>
  void call (Fn const& f)
  { callf (bind (f)); }

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
#endif
};

#endif
