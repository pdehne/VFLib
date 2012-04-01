// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_WORKER_VFHEADER__
#define __VF_WORKER_VFHEADER__

#include "../memory/vf_AllocatedBy.h"
#include "../memory/vf_Allocator.h"
#include "../lockfree/vf_LockFreeQueue.h"

/**
  Provides a functor FIFO, with these invariants:

	#1 Functors are added to any open queue from any thread,
	   using call() and/or queue()

	#2 When process() is called the functors are executed and disposed.

	#3 Functors queued by the same thread always execute in the
	   order they were queued.

	#4 When call() is used and the calling thread is the same one
	   used in the last call to process(), the call is added and the
	   queue is immediately processed. This behavior can be avoided by
	   using queue() instead of call().

	#5 Once queued, a functor must eventually execute. A functor is
	   never lost or unprocessed. It is an error if the CallQueue
	   is deleted while there are pending calls.

	#6 Adding a call to a closed queue throws an exception.

	#7 The overridable functions signal() and reset() are called
	   when the queue transitions from empty to non-empty, and
	   vice versa.

  For performance considerations, this implementation is wait-free
  for producers and mostly wait-free for consumers. It also uses a lock-free
  and wait-free (in the fast path) custom memory allocator.
*/
class CallQueue
{
public:
  typedef Allocator AllocatorType;

  class Call;

private:
  typedef LockFree::Queue <Call> Calls;

public:
  class Call : public Calls::Node,
               public AllocatedBy <AllocatorType>
  {
  public:
    virtual ~Call () { }
    virtual void operator() () = 0;
  };

  template <class Functor>
  class CallType : public Call
  {
  public:
    explicit CallType (Functor const& f) : m_f (f) { }
    ~CallType () { }
    void operator() () { m_f (); }

  private:
    Functor m_f;
  };

public:
  explicit CallQueue (String name);
  ~CallQueue ();

  inline AllocatorType& getAllocator ()
  {
    return m_allocator;
  }

  // returns true if the current thread of execution is
  // the last one used to process this queue
  bool isAssociatedWithCurrentThread () const;

  // used for diagnostics in Listener
  bool in_process () const { return m_in_process.isSet(); }

  // Add the Call without executing immediately.
  // Calls MUST NOT cause thread interruptions.
  void queuep (Call* call);

  // Add the Call, and process the queue if we're
  // on the associated thread.
  //
  void callp (Call* call);

  // Convenience for caller generated functors.

  template <class Functor>
  void queuef (Functor const& f)
  {
    queuep (new (m_allocator) CallType <Functor> (f));
  }

  template <class Functor>
  void callf (Functor const& f)
  {
    callp (new (m_allocator) CallType <Functor> (f));
  }

  // Sugar for queuef() with automatic binding.

  template <class Fn>
  void queue (Fn f)
  { queuef (vf::bind (f)); }

  template <class Fn, typename  T1>
  void queue (Fn f,   const T1& t1)
  { queuef (vf::bind (f, t1)); }

  template <class Fn, typename  T1, typename  T2>
  void queue (Fn f,   const T1& t1, const T2& t2)
  { queuef (vf::bind (f, t1, t2)); }

  template <class Fn, typename  T1, typename  T2, typename  T3>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3)
  { queuef (vf::bind (f, t1, t2, t3)); }

  template <class Fn, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void queue (Fn f,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queuef (vf::bind (f, t1, t2, t3, t4)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5, t6)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5, t6, t7, t8)); }

  // Sugar for callf() with automatic binding.

  template <class Fn>
  void call (Fn f)
  { callf (vf::bind (f)); }

  template <class Fn, typename  T1>
  void call (Fn f,    const T1& t1)
  { callf (vf::bind (f, t1)); }

  template <class Fn, typename  T1, typename  T2>
  void call (Fn f,    const T1& t1, const T2& t2)
  { callf (vf::bind (f, t1, t2)); }

  template <class Fn, typename  T1, typename  T2, typename  T3>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3)
  { callf (vf::bind (f, t1, t2, t3)); }

  template <class Fn, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void call (Fn f,    const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { callf (vf::bind (f, t1, t2, t3, t4)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { callf (vf::bind (f, t1, t2, t3, t4, t5)); }

  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { callf (vf::bind (f, t1, t2, t3, t4, t5, t6)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { callf (vf::bind (f, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { callf (vf::bind (f, t1, t2, t3, t4, t5, t6, t7, t8)); }

protected:
  // Used by GuiWorker to synchronize calls
  void associateWithCurrentThread ();

  // Derived class calls this when the queue is signaled,
  // or whenever it wants to. It is disallowed to call
  // process() from more than one thread simultaneously.
  // 
  bool process ();

  // This function should be called as early as possible.
  // Closing the CallQueue is done for diagnostics, it allows
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
  bool do_process ();

private:
  String const m_name;
  VF_JUCE::Thread::ThreadID m_id;
  Calls m_list;
  AtomicFlag m_closed;
  AtomicFlag m_in_process;
  AllocatorType m_allocator;
};

#endif
