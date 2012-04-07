// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CALLQUEUE_VFHEADER
#define VF_CALLQUEUE_VFHEADER

#include "../memory/vf_AllocatedBy.h"
#include "../memory/vf_FifoFreeStore.h"
#include "../containers/vf_LockFreeQueue.h"

//==============================================================================
/** 
    A FIFO for calling functors asynchronously.

    @todo Discussion of synchronization concept and time evolution of mutable state

    This object is an alternative to traditional locking techniques used to
    implement concurrent systems. Instead of acquiring a mutex to change shared
    data, a functor is queued for execution on another thread. The functor
    typically performs the operation that was previously executed within a lock
    (i.e. CriticalSection).

    For reading operations on shared data, instead of acquiring a mutex and
    accessing the data directly, copies are made (one for each thread), and the
    thread accesses its copy without taking a lock. One thread owns the master
    data. It is to this thread that requests to change the data are posted to
    the CallQueue. When the owning thread updates the shared data, it then in
    turn posts calls to other threads (using the CallQueue again) with functors
    that incrementally update portions of the copy of the data.

    Setting up a system to use this method of shared data access requires more
    work up front and a small increase in complexity of objects used in the
    system. For example, when dynamically allocated objects of class type are
    part of the shared data (which is expected, for all but the most trivial
    systems), it becomes necessary to make objects reference counted.

    The benefits of this model are significant, however. Since functors only
    execute at well defined times (via the call to synchronize()), under control
    of the associated thread, the programmer can reason and make strong
    statements about the correctness of the concurrent system. For example, if
    the audioDeviceIOCallback calls process() on its CallQueue at the beginning
    of its execution and nowhere else, we can be assured that shared data will
    only be changed at the defined point.

    Avoiding locking by making copies of shared data requires additional set up
    and maintenance, but is rewarded by an increase in performance. Readers of
    the shared data have carte blanch to access their copy in any manner
    desired, with the invariant that the data will not change except across a
    call to synchronize().

    Since a CallQueue is almost always used to invoke parameterized member
    functions of objects, the call() function comes in a variety of convenient
    forms to make usage easy:

    @code

    void func1 (int);

    struct Object
    {
      void func2 (void);
      void func3 (String name);

      static void func4 ();
    };

    CallQueue fifo ("Example");

    void example ()
    {
      fifo.call (func1, 42);               // same as: func1 (42)

      Object* object = new Object;

      fifo.call (&Object::func2, object);  // same as: object->func2 ()

      fifo.call (&Object::func3,           // same as: object->funcf ("Label")
                 object,
                 "Label");

      fifo.call (&Object::func4);          // even static members can be called.

      fifo.callf (bind (&Object::func2,    // same as: object->func2 ()
                        object));
    }

    @endcode

    Functors should execute quickly, ideally in constant time. This queue is
    not a replacement for a thread pool or job queue type system. The purpose
    of a functor is to trigger the time evolution of mutable data towards a
    consensus among multiple threads. Objects of class type passed as
    parameters should, in general, be reference counted.

    Invariants:

    - Functors can be added from any thread at any time,
      to any queue which is not closed.

    - When synchronize() is called, functors are called and deleted.

    - The thread from which synchronize() is called is considered the thread
      associated with the CallQueue.

    - Functors queued by the same thread always execute in the same order
      they were queued.

    - Functors are guaranteed to execute. It is an error if the CallQueue
      is deleted while there are functors in it.

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
  //============================================================================
  /** Create a CallQueue.

      @param  name  A string identifying the associated thread for debugging.
  */
  explicit CallQueue (String name);

  /** Destroy a CallQueue.

      It is an error to destroy a CallQueue that still contains functors.
  */
  ~CallQueue ();

  //============================================================================
  /** Add a function with no parameters and synchronize if possible.

      When a functor is added in a call made from the associated thread, this
      routine will automatically call synchronize(). This behavior can be
      avoided by using queue() instead.

      @see queue
  */
  template <class Fn>
  void call (Fn f)
  {
    callf (vf::bind (f));
  }

  /** Add a function with no parameters, without synchronizing.

      This function can be useful when the caller is holding a lock, and the
      functors to be added would cause contention or undefined behavior due
      to the locking model, if a synchronization took place.

      @see call
  */
  template <class Fn>
  void queue (Fn f)
  {
    queuef (vf::bind (f));
  }

protected:
  //============================================================================
  /** Synchronize by calling all functors.

      Derived class call this when the queue is signaled, and optionally at any
      additional time. Calling this function from more than one thread
      simultaneously results in undefined behavior.

      @return  `true` if any functors were executed.
  */
  bool synchronize ();

  /** Close the queue.

      Functors may not be added after this routine is called. This is used for
      diagnostics, to track down spurious calls during application shutdown
      or exit. Derived classes may call this if the appropriate time is known.
  */
  void close ();

  /** Notification when a queue is signaled.

      A queue is signaled on the transition from empty to non-empty. Derived
      classes implement this function to perform a notification so that
      synchronize() will be called. For example, by triggering a WaitableEvent.

      Due to the implementation the queue can remain signaled for one extra
      cycle. This does not happen under load and is not an issue in practice.
  */

  virtual void signal () = 0;

  /** Notification when a queue becomes empty */
  virtual void reset () = 0;

public:
#ifndef DOXYGEN
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
#endif

#ifndef DOXYGEN
public:
  typedef FifoFreeStoreType AllocatorType;

  class Call;

private:
  typedef LockFreeQueue <Call> Calls;

public:
  /** Used internally to create custom work items.
  */
  class Call : public Calls::Node,
               public AllocatedBy <AllocatorType>
  {
  public:
    virtual ~Call () { }
    virtual void operator() () = 0;
  };

  /** Used internally to share the allocator with other objects.
  */
  inline AllocatorType& getAllocator ()
  {
    return m_allocator;
  }

  /** Used internally to determine if the current thread of execution is the
      last one used to synchronize this queue.
  */
  bool isAssociatedWithCurrentThread () const;

  /** Used internally for diagnostics to determine if the call queue is currently
      inside synchronize ().
  */
  bool isBeingSynchronized () const { return m_isBeingSynchronized.isSet(); }

  /** Add the Call without executing immediately. */
  void queuep (Call* call);

  /** Add the Call, and process the queue if we're on the associated thread. */
  void callp (Call* call);
#endif

private:
#ifndef DOXYGEN
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
#endif

  bool do_process ();

private:
  String const m_name;
  VF_JUCE::Thread::ThreadID m_id;
  Calls m_list;
  AtomicFlag m_closed;
  AtomicFlag m_isBeingSynchronized;
  AllocatorType m_allocator;
};

#endif
