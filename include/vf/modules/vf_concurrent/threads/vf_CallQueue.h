// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CALLQUEUE_VFHEADER
#define VF_CALLQUEUE_VFHEADER

#include "../memory/vf_AllocatedBy.h"
#include "../memory/vf_FifoFreeStore.h"
#include "../containers/vf_LockFreeQueue.h"

//==============================================================================
/**
  A queue / fifo of functors.

  This object is an alternative to traditional locking techniques used to
  implement concurrent systems. Instead of acquiring a mutex to change shared
  data, a functor is queued for execution on another thread. The functor
  typically performs the operation that was previously executed within a lock
  (i.e. CriticalSection).

  For reading operations on shared data, instead of acquiring a mutex and
  accessing the data directly, copies are made (one for each thread), and the
  thread accesses its copy without taking a lock. One thread owns the master
  data. It is to this thread that requests to change the data are posted to the
  CallQueue. When the owning thread updates the shared data, it then in turn
  posts calls to other threads (using the CallQueue again) with functors that
  incrementally update portions of the copy of the data.

  Setting up a system to use this method of shared data access requires more
  work up front and a small increase in complexity of objects used in the
  system. For example, when dynamically allocated objects of class type are
  part of the shared data (which is expected, for all but the most trivial
  systems), it becomes necessary to make objects reference counted.

  The benefits of this model are significant, however. Since functors only
  execute at well defined times (via the call to process()), under control of
  the associated thread, the programmer can reason and make strong statements
  about the correctness of the concurrent system. For example, if the
  audioDeviceIOCallback calls process() on its CallQueue at the beginning of
  its execution and nowhere else, we can be assured that shared data will only
  be changed at the defined point.

  Avoiding locking by making copies of shared data requires additional set up
  and maintenance, but is rewarded by an increase in performance. Readers of the
  shared data have carte blanch to access their copy in any manner desired, with
  the invariant that the data will not change except across a call to
  process().

  Since a CallQueue is almost always used to invoke parameterized member
  functions of objects, the call() function comes in a variety of convenient
  forms to make usage easy.

  Example:

      void func1 (int);

      struct Object
      {
        void func2 (void);
        void func3 (String name);

        static void func4 ();
      };

      CallQueue queue ("Example");

      void example ()
      {
        queue.call (func1, 42);               // same as: func1 (42)

        Object* object = new Object;

        queue.call (&Object::func2, object);  // same as: object->func2 ()

        queue.call (&Object::func3,           // same as: object->funcf ("Label")
                    object,
                    "Label");

        queue.call (&Object::func4);          // even static members can be called.

        queue.callf (bind (&Object::func2,    // same as: object->func2 ()
                           object));
      }

  Invariants:

  1. Functors can be added from any thread at any time,
	 to any queue which is not closed.
  2. When process() is called, functors in the queue are
	 executed and deleted.
  3. The thread from which process() is called is considered
	 the thread associated with the CallQueue.
  4. Functors queued by the same thread always execute in
	 the same order they were queued.
  5. Functors are guaranteed to execute. It is an error if
	 the CallQueue is deleted while there are functors in it.

  Normally, you will not use CallQueue directly, but one of its subclasses
  instead. The CallQueue is one of a handful of objects that work together to
  implement this system of concurrent data access.

  For performance considerations, this implementation is wait-free for
  producers and mostly wait-free for consumers. It also uses a lock-free
  and wait-free (in the fast path) custom memory allocator.

  @see GuiCallQueue, ManualCallQueue
*/
class CallQueue
{
public:
  /** Create a CallQueue.

      @param  name  A string used to identify the associated thread for debugging.
  */
  explicit CallQueue (String name);

  /** Destroy a CallQueue.

      It is an error to destroy a CallQueue that still contains functors.
  */
  ~CallQueue ();

  /** Add a function with no parameters and synchronize the queue if possible.

      When a functor is added in a call made from the associated thread, this
      routine will automatically call process(). This behavior can be avoided
      by using queue() instead.

      @see queue
  */
  template <class Fn>
  void call (Fn f)
  { callf (vf::bind (f)); }

  /** Add a function with no parameters, without synchronizing the queue.

      @see call
  */
  template <class Fn>
  void queue (Fn f)
  { queuef (vf::bind (f)); }

  /** call() with automatic binding for a function with 1 parameter. */
  template <class Fn, typename  T1>
  void call (Fn f,    const T1& t1)
  { callf (vf::bind (f, t1)); }

  /** call() with automatic binding for a function with 2 parameters. */
  template <class Fn, typename  T1, typename  T2>
  void call (Fn f,    const T1& t1, const T2& t2)
  { callf (vf::bind (f, t1, t2)); }

  /** call() with automatic binding for a function with 3 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3)
  { callf (vf::bind (f, t1, t2, t3)); }

  /** call() with automatic binding for a function with 4 parameters. */
  template <class Fn, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void call (Fn f,    const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { callf (vf::bind (f, t1, t2, t3, t4)); }

  /** call() with automatic binding for a function with 5 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { callf (vf::bind (f, t1, t2, t3, t4, t5)); }

  /** call() with automatic binding for a function with 6 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { callf (vf::bind (f, t1, t2, t3, t4, t5, t6)); }

  /** call() with automatic binding for a function with 7 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { callf (vf::bind (f, t1, t2, t3, t4, t5, t6, t7)); }

  /** call() with automatic binding for a function with 8 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void call (Fn f,    const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { callf (vf::bind (f, t1, t2, t3, t4, t5, t6, t7, t8)); }

  /** queue() with automatic binding for a function with 1 parameter. */
  template <class Fn, typename  T1>
  void queue (Fn f,   const T1& t1)
  { queuef (vf::bind (f, t1)); }

  /** queue() with automatic binding for a function with 2 parameters. */
  template <class Fn, typename  T1, typename  T2>
  void queue (Fn f,   const T1& t1, const T2& t2)
  { queuef (vf::bind (f, t1, t2)); }

  /** queue() with automatic binding for a function with 3 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3)
  { queuef (vf::bind (f, t1, t2, t3)); }

  /** queue() with automatic binding for a function with 4 parameters. */
  template <class Fn, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void queue (Fn f,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queuef (vf::bind (f, t1, t2, t3, t4)); }

  /** queue() with automatic binding for a function with 5 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5)); }

  /** queue() with automatic binding for a function with 6 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5, t6)); }

  /** queue() with automatic binding for a function with 7 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5, t6, t7)); }

  /** queue() with automatic binding for a function with 8 parameters. */
  template <class Fn, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void queue (Fn f,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queuef (vf::bind (f, t1, t2, t3, t4, t5, t6, t7, t8)); }

  /** call() for an arbitrary functor */
  template <class Functor>
  void callf (Functor const& f)
  {
    callp (new (m_allocator) CallType <Functor> (f));
  }

  /** queue() for an arbitrary functor */
  template <class Functor>
  void queuef (Functor const& f)
  {
    queuep (new (m_allocator) CallType <Functor> (f));
  }

public:
  typedef FifoFreeStoreType AllocatorType;

  class Call;

private:
  typedef LockFreeQueue <Call> Calls;

public:
  /** The abstract base that holds a functor.

      Objects built on top of the CallQueue derive from this class for their
      own custom work items to synergize with the allocator.

      @see Listeners
  */
  class Call : public Calls::Node,
               public AllocatedBy <AllocatorType>
  {
  public:
    virtual ~Call () { }
    virtual void operator() () = 0;
  };

  /** Retrieve the allocator associated with this CallQueue.

      Objects built on CallQueue use this.

      @see Listeners
  */
  inline AllocatorType& getAllocator ()
  {
    return m_allocator;
  }

  /** Returns true if the current thread of execution is the
      last one used to process this queue. */
  bool isAssociatedWithCurrentThread () const;

  /** Returns true of the call queue is currently in the call
      to process().
      
      Used for diagnostics in Listener.
  */
  bool isInProcess () const { return m_isInProcess.isSet(); }

  /** Add the Call without executing immediately. */
  void queuep (Call* call);

  /** Add the Call, and process the queue if we're on the associated thread. */
  void callp (Call* call);

protected:
  /** Process all functors in the queue.

      Derived class call this when the queue is signaled, or whenever it
      wants to. Calling this function from more than one thread simultaneously
      results in undefined behavior.
  */
  bool process ();

  /** Close the queue.

      Functors may not be placed into a closed queue. This is used for
      diagnostics, to track down spurious calls during application shutdown
      or exit.

      Derived classes may call this if the appropriate time is known.
  */
  void close ();

  /** Signaling notification.

      A queue is considered signaled when there are one or more functors.

      Due to the implementation the queue can remain signaled for one excess
      cycle. This does not happen under load and is not an issue in practice.
  */

  /** Called when the queue becomes empty */
  virtual void reset () = 0;

  /** Called when the queue has functors. Derived classes will override this
      and perform some type of signaling operation. For example, pulsing a
      WaitableEvent
  */
  virtual void signal () = 0;

private:
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

  bool do_process ();

private:
  String const m_name;
  VF_JUCE::Thread::ThreadID m_id;
  Calls m_list;
  AtomicFlag m_closed;
  AtomicFlag m_isInProcess;
  AllocatorType m_allocator;
};

#endif
