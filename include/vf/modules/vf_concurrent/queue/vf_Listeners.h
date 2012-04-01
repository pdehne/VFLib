// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_LISTENERS_VFHEADER__
#define __VF_LISTENERS_VFHEADER__

#include "vf/modules/vf_concurrent/memory/vf_AllocatedBy.h"
#include "vf/modules/vf_concurrent/memory/vf_Allocator.h"
#include "vf/modules/vf_core/containers/vf_List.h"
#include "vf/modules/vf_concurrent/threads/vf_ReadWriteMutex.h"
#include "vf/modules/vf_core/threads/vf_SharedObject.h"
#include "vf/modules/vf_concurrent/queue/vf_Worker.h"

// List where each Listener registers with the desired Worker
// on which the call is made. Since the list traversal for an associated
// Worker is done on its thread, it is impossible for a Listener to
// get deleted while a callback is pending.

//
// TODO: CONST CORRECTNESS? List CONST-NESS?
//

class ListenersBase
{
public:
  //typedef StandardAllocator AllocatorType;
  struct ListenersStructureTag { };

  typedef GlobalAllocator <ListenersStructureTag> AllocatorType;
  //typedef StandardAllocator AllocatorType;

  typedef GlobalAllocator <ListenersBase> CallAllocatorType;
  //typedef StandardAllocator CallAllocatorType;

  // Reference counted polymorphic unary functor void (*)(void* listener).
  //
  class Call : public ReferenceCountedObject,
               public AllocatedBy <CallAllocatorType>
  {
  public:
    typedef ReferenceCountedObjectPtr <Call> Ptr;
    virtual void operator () (void* const listener) = 0;
  private:
    void destroySharedObject ();
  };

private:
  typedef unsigned long timestamp_t;

  class Group;
  typedef List <Group> Groups;

  class Proxy;
  typedef List <Proxy> Proxies;

  class CallWork;
  class GroupWork;
  class GroupWork1;

  // Maintains a list of listeners registered on the same Worker
  //
  class Group : public Groups::Node,
                public ReferenceCountedObject,
                public AllocatedBy <AllocatorType>
  {
  public:
    typedef ReferenceCountedObjectPtr <Group> Ptr;

    explicit Group    (Worker& worker);
    ~Group            ();
    void add          (void* listener, const timestamp_t timestamp,
                       AllocatorType& allocator);
    bool remove       (void* listener);
    bool contains     (void* const listener);
    void call         (Call* const c, const timestamp_t timestamp);
    void queue        (Call* const c, const timestamp_t timestamp);
    void call1        (Call* const c, const timestamp_t timestamp,
                       void* const listener);
    void queue1       (Call* const c, const timestamp_t timestamp,
                       void* const listener);
    void do_call      (Call* const c, const timestamp_t timestamp);
    void do_call1     (Call* const c, const timestamp_t timestamp,
                       void* const listener);

    bool empty        () const { return m_list.empty(); }
    Worker& getWorker () const { return m_worker; }

  private:
    void destroySharedObject() { delete this; }

  private:
    struct Entry;
    typedef List <Entry> List;

    Worker& m_worker;
    List m_list;
    void* m_listener;
    CacheLine::Aligned <ReadWriteMutex> m_mutex;
  };

  // A Proxy is keyed to a unique pointer-to-member of a
  // ListenerClass and is used to consolidate multiple unprocessed
  // Calls into a single call to prevent excess messaging. It is up
  // to the user of the class to decide when this behavior is appropriate.
  //
  class Proxy : public Proxies::Node,
                public AllocatedBy <AllocatorType>
  {
  public:
    enum
    {
      maxMemberBytes = 16
    };

    Proxy (void const* const member, const size_t bytes);
    ~Proxy ();

    void add    (Group* group, AllocatorType& allocator);
    void remove (Group* group);
    void update (Call* const c, const timestamp_t timestamp);

    bool match  (void const* const member, const size_t bytes) const;

  private:
    class Work;
    struct Entry;
    typedef List <Entry> Entries;
    char m_member [maxMemberBytes];
    const size_t m_bytes;
    Entries m_entries;
  };

public:
  ListenersBase ();
  ~ListenersBase ();

  inline CallAllocatorType& getCallAllocator ()
  {
    return *m_callAllocator;
  }

  void callp        (Call::Ptr c);
  void queuep       (Call::Ptr c);

protected:
  void add_void     (void* const listener, Worker& worker);
  void remove_void  (void* const listener);
  void call1p_void  (void* const listener, Call* c);
  void queue1p_void (void* const listener, Call* c);
  void updatep      (void const* const member,
                     const size_t bytes, Call::Ptr cp);

private:
  Proxy* find_proxy (const void* member, int bytes);

private:
  Groups m_groups;
  Proxies m_proxies;
  timestamp_t m_timestamp;
  CacheLine::Aligned <ReadWriteMutex> m_groups_mutex;
  CacheLine::Aligned <ReadWriteMutex> m_proxies_mutex;
  AllocatorType::Ptr m_allocator;
  CallAllocatorType::Ptr m_callAllocator;
};

//------------------------------------------------------------------------------

template <class ListenerClass>
class Listeners : public ListenersBase
{
public:
  template <class Functor>
  class CallType : public Call
  {
  public:
    CallType (const Functor& f) : m_f (f)
    {
    }

    void operator() (void* const listener)
    {
      m_f.operator() (static_cast <ListenerClass*> (listener));
    }

  private:
    Functor m_f;
  };

public:
  //
  // Add a listener to receive call notifications.
  //
  //  #1 All other functions are blocked during add().
  //  #2 The listener's receipt of every subsequent call() is guaranteed.
  //  #3 Member functions are called on the specified thread queue.
  //  #4 The listener must not already exist in the list.
  //  #5 This can be called from any thread.
  // 
  void add (ListenerClass* const listener, Worker& worker)
  {
    add_void (listener, worker);
  }
  void add (ListenerClass* const listener, Worker* worker)
  {
    add (listener, *worker);
  }

  //
  // Remove a listener from the list
  //
  //  #1 All other functions are blocked during remove().
  //  #2 The listener is guaranteed not to receive calls
  //     after remove() returns.
  //  #3 The listener must exist in the list.
  //  #4 This can be called from any thread.
  //
  // A listener should always be removed before it's corresponding
  //   Worker is closed.
  //
  void remove (ListenerClass* const listener)
  {
    remove_void (listener);
  }

  //
  // Call a specified member function on every listener's associated
  // Worker with the given functor.
  //
  //  #1 The arguments must match the function signature.
  //  #2 A listener that removes itself afterwards may not get called.
  //  #3 Calls from the same thread always execute in order.
  //  #4 Listener members are always invoked immediately in call() by the
  //     current thread of execution if it matches the thread used
  //     by the listener's thread queue. This happens before call() returns.
  //  #5 A listener can always remove itself even if there are pending calls.
  //

  // Queue a call to a single listener.
  // The worker is processed if called on the associated thread.
  //
  inline void call1p (ListenerClass* const listener, Call::Ptr c)
  {
    call1p_void (listener, c);
  }

  // Queue a call to a single listener.
  //
  inline void queue1p (ListenerClass* const listener, Call::Ptr c)
  {
    queue1p_void (listener, c);
  }

  // Queue a call to all listeners.
  // The worker is processed if called on the associated thread.
  //
  template <class Functor>
  inline void callf (const Functor& f)
  {
    callp (new (getCallAllocator ()) CallType <Functor> (f));
  }

  // Queue a call to all listeners.
  //
  template <class Functor>
  inline void queuef (const Functor& f)
  {
    queuep (new (getCallAllocator ()) CallType <Functor> (f));
  }

  template <class Functor>
  inline void call1f (ListenerClass* const listener, const Functor& f)
  {
    call1p (listener, new (getCallAllocator ()) CallType <Functor> (f));
  }

  template <class Functor>
  inline void queue1f (ListenerClass* const listener, const Functor& f)
  {
    queue1p (listener, new (getCallAllocator ()) CallType <Functor> (f));
  }

  template <class Member, class Functor>
  inline void updatef (Member member, const Functor& f)
  {
    updatep (reinterpret_cast <void*> (&member), sizeof (Member),
             new (getCallAllocator ()) CallType <Functor> (f));
  }

  template <class Mf>
  inline void call (Mf mf)
  { callf (vf::bind (mf, vf::_1)); }

  template <class Mf, typename  T1>
  inline void call (Mf mf,   const T1& t1)
  { callf (vf::bind (mf, vf::_1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  inline void call (Mf mf,   const T1& t1, const T2& t2)
  { callf (vf::bind (mf, vf::_1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  inline void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3)
  { callf (vf::bind (mf, vf::_1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  inline void call (Mf mf,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { callf (vf::bind (mf, vf::_1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  inline void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { callf (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  inline void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { callf (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  inline void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { callf (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  inline void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { callf (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  //
  // Queue a call without synchronizing
  //

  template <class Mf>
  inline void queue (Mf mf)
  { queuef (vf::bind (mf, vf::_1)); }

  template <class Mf, typename  T1>
  inline void queue (Mf mf,  const T1& t1)
  { queuef (vf::bind (mf, vf::_1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  inline void queue (Mf mf,  const T1& t1, const T2& t2)
  { queuef (vf::bind (mf, vf::_1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  inline void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3)
  { queuef (vf::bind (mf, vf::_1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  inline void queue (Mf mf,  const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queuef (vf::bind (mf, vf::_1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  inline void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queuef (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  inline void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queuef (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  inline void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queuef (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  inline void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queuef (vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  // These are for targeting individual listeners.
  // Use carefully!

  template <class Mf>
  inline void call1 (ListenerClass* const listener, Mf mf)
  { call1f (listener, vf::bind (mf, vf::_1)); }

  template <class Mf, typename  T1>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1)
  { call1f (listener, vf::bind (mf, vf::_1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2)
  { call1f (listener, vf::bind (mf, vf::_1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3)
  { call1f (listener, vf::bind (mf, vf::_1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { call1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { call1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { call1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { call1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  inline void call1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { call1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  template <class Mf>
  inline void queue1 (ListenerClass* const listener, Mf mf)
  { queue1f (listener, vf::bind (mf, vf::_1)); }

  template <class Mf, typename  T1>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1)
  { queue1f (listener, vf::bind (mf, vf::_1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2)
  { queue1f (listener, vf::bind (mf, vf::_1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3)
  { queue1f (listener, vf::bind (mf, vf::_1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queue1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queue1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queue1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queue1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  inline void queue1 (ListenerClass* const listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queue1f (listener, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  //
  // update()
  //
  // Like call(), but if there is a previous unprocessed call for
  // the same member f, the previous call is replaced. It is
  // up to the caller to determine if this behavior is desired.
  //

  template <class Mf>
  inline void update (Mf mf)
  { updatef (mf, vf::bind (mf, vf::_1)); }

  template <class Mf, typename  T1>
  inline void update (Mf mf,  const T1& t1)
  { updatef (mf, vf::bind (mf, vf::_1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  inline void update (Mf mf, const T1& t1, const T2& t2)
  { updatef (mf, vf::bind (mf, vf::_1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  inline void update (Mf mf, const T1& t1, const T2& t2, const T3& t3)
  { updatef (mf, vf::bind (mf, vf::_1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  inline void update (Mf mf, const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { updatef (mf, vf::bind (mf, vf::_1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  inline void update (Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { updatef (mf, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  inline void update (Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { updatef (mf, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  inline void update (Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { updatef (mf, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  inline void update (Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { updatef (mf, vf::bind (mf, vf::_1, t1, t2, t3, t4, t5, t6, t7, t8)); }
};

#endif
