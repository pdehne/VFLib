// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LISTENERS_VFHEADER__
#define __VF_LISTENERS_VFHEADER__

#include "vf/vf_List.h"
#include "vf/vf_Allocator.h"
#include "vf/vf_LockFreeReadWriteMutex.h"
#include "vf/vf_SharedObject.h"
#include "vf/vf_SharedSingleton.h"
#include "vf/vf_Worker.h"

// List where each Listener registers with the desired Worker
// on which the call is made. Since the list traversal for an associated
// Worker is done on its thread, it is impossible for a Listener to
// get deleted while a callback is pending.

//
// TODO: CONST CORRECTNESS? List CONST-NESS?
//

class ListenersBase
{
protected:
  typedef unsigned long timestamp_t;

  typedef GlobalAllocator <ListenersBase> CallAllocatorType;

  //
  // Reference counted polymorphic unary functor void (*)(void* listener).
  // A timestamp distinguishes a Call created before a listener is added.
  //
  class Call : public SharedObject,
               public CallAllocatorType::Allocated
  {
  public:
    typedef SharedObjectPtr <Call> Ptr;

    class Work : public Worker::Call
    {
    public:
      Work (ListenersBase::Call* const c, void* const listener)
        : m_call (c), m_listener (listener) { }
      void operator() ()
        { m_call->operator() (m_listener); }
    private:
      ListenersBase::Call::Ptr m_call;
      void* const m_listener;
    };

  public:
    virtual void operator () (void* const listener) = 0;

  private:
    void destroySharedObject () { delete this; }
  };

private:
  class Group;
  typedef List <Group> Groups;

  class Proxy;
  typedef List <Proxy> Proxies;

  //
  // Maintains a list of listeners registered on the same Worker
  //
  class Group : public Groups::Node, public SharedObject
  {
  public:
    typedef SharedObjectPtr <Group> Ptr;

    explicit Group    (Worker* const worker);
    virtual ~Group    ();
    void add          (void* listener, const timestamp_t timestamp);
    bool remove       (void* listener);
    bool contains     (void const* listener);
    void call         (Call* const c, const timestamp_t timestamp);
    void queue        (Call* const c, const timestamp_t timestamp);
    void queue1       (Call* const c, const timestamp_t timestamp, void* const listener);
    void do_call      (Call* const c, const timestamp_t timestamp);
    void do_call1     (Call* const c, const timestamp_t timestamp, void* const listener);

    bool empty        () const { return m_list.empty(); }
    Worker* getWorker () const { return m_worker; }

    class Work : public Worker::Call
    {
    public:
      Work (Group* group, ListenersBase::Call* c, const timestamp_t timestamp)
        : m_group (group), m_call (c), m_timestamp (timestamp) { }
      void operator() ()
        { m_group->do_call (m_call, m_timestamp); }
    private:
      Group::Ptr m_group;
      ListenersBase::Call::Ptr m_call;
      const timestamp_t m_timestamp;
    };

    class Work1 : public Worker::Call
    {
    public:
      Work1 (Group* group, ListenersBase::Call* c, const timestamp_t timestamp, void* const listener)
        : m_group (group), m_call (c), m_timestamp (timestamp), m_listener (listener) { }
      void operator() ()
        { m_group->do_call1 (m_call, m_timestamp, m_listener); }
    private:
      Group::Ptr m_group;
      ListenersBase::Call::Ptr m_call;
      const timestamp_t m_timestamp;
      void* const m_listener;
    };

  private:
    void destroySharedObject ()
      { globalDelete (this); }

  private:
    struct Entry;
    typedef List <Entry> List;
    struct Entry : List::Node
    {
      Entry (void* const l, const timestamp_t t)
        : listener (l), timestamp (t) { }
      void* const listener;
      const timestamp_t timestamp;
    };

    Worker* const m_worker;
    List m_list;
    void* m_listener;
    LockFree::ReadWriteMutex m_mutex;
  };

  //
  // A Proxy is keyed to a unique pointer-to-member of a
  // ListenerClass and is used to consolidate multiple unprocessed
  // Calls into a single call to prevent excess messaging. It is up
  // to the user of the class to decide when this behavior is appropriate.
  //
  class Proxy : public Proxies::Node
  {
  public:
    explicit Proxy ();
    ~Proxy ();

    void add    (Group* group);
    void remove (Group* group);
    void update (Call* const c, const timestamp_t timestamp);

    virtual bool match (const void* member, int bytes) const = 0;

  private:
    struct Entry;
    typedef List <Entry> Entries;
    struct Entry : Entries::Node, SharedObject
    {
      typedef SharedObjectPtr <Entry> Ptr;
      explicit Entry (Group* g) : group (g)
        { }
      ~Entry ()
        { vfassert (call.get () == 0); }
      void destroySharedObject ()
        { globalDelete (this); }
      Group::Ptr group;
      Atomic::Pointer <Call> call;
    };

    class Work : public Worker::Call
    {
    public:
      Work (Proxy* proxy, Entry* const entry, const timestamp_t timestamp)
        : m_proxy (proxy), m_entry (entry), m_timestamp (timestamp) { }
      void operator() ();
    private:
      Proxy* m_proxy;
      Entry::Ptr m_entry;
      const timestamp_t m_timestamp;
    };

    void do_call (Entry* const entry, const timestamp_t timestamp);

  private:
    Entries m_entries;
  };

  // The physical memory for the pointer to member is
  // only used as a key for comparison and not to produce a call.
  template <int Bytes>
  class StoredProxy : public Proxy
  {
  private:
    char m_member [Bytes];
  public:
    StoredProxy (const void* member)
      { memcpy (m_member, member, Bytes); }
    bool match (const void* member, int bytes) const
      { return (bytes == Bytes && memcmp (member, m_member, bytes) == 0); }
  };

  /*@ Implementation @*/
  Proxy* find_proxy (const void* member, int bytes);

protected:
  ListenersBase ();
  ~ListenersBase ();

  void add_void     (void* const listener, Worker* worker);
  void remove_void  (void* const listener);
  void callp        (Call::Ptr c);
  void queuep       (Call::Ptr c);
  void queue1p      (void* const listener, Call::Ptr c);

  // Search for an existing Proxy that matches the pointer to
  // member and replace it's Call, or create a new Proxy for it.
  //
  template <int Bytes>
  void updatep (const void *member, Call::Ptr cp)
  {
    Call* c = cp;

    LockFree::ScopedReadLock lock (m_groups_mutex);

    if (!m_groups.empty ())
    {
      Proxy* proxy;
      
      {
        LockFree::ScopedReadLock lock (m_proxies_mutex);

        // See if there's already a proxy
        proxy = find_proxy (member, Bytes);
      }

      // Possibly create one
      if (!proxy)
      {
        LockFree::ScopedWriteLock lock (m_proxies_mutex);

        // Have to search for it again in case someone else added it
        proxy = find_proxy (member, Bytes);

        if (!proxy)
        {
          // Create a new empty proxy
          proxy = globalAlloc <StoredProxy <Bytes> >::New (member);

          // Add all current groups to the Proxy.
          // We need the group read lock for this (caller provided).
          for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
          {
            Group* group = *iter++;
            proxy->add (group);
          }

          // Add it to the list.
          m_proxies.push_front (proxy);
        }
      }

      // Requires the group read lock
      proxy->update (c, m_timestamp);
    }
  }

private:
  Groups m_groups;
  Proxies m_proxies;
  LockFree::ReadWriteMutex m_proxies_mutex;
protected:
  CallAllocatorType m_callAllocator;
  timestamp_t m_timestamp;
  LockFree::ReadWriteMutex m_groups_mutex;
};

//------------------------------------------------------------------------------

template <class ListenerClass>
class Listeners : public ListenersBase
{
private:
  template <class Functor>
  class CallType : public Call
  {
  public:
    CallType (const Functor& f)
      : m_f (f) { }
    ~CallType ()
      { } // without this we get UB
    void operator() (void* const listener)
      { m_f.operator() (static_cast <ListenerClass*> (listener)); }

  private:
    Functor m_f;
  };

  template <class Functor>
  void callf (const Functor& f)
  {
    callp (new (m_callAllocator) CallType <Functor> (f));
  }

  template <class Functor>
  void queuef (const Functor& f)
  {
    queuep (new (m_callAllocator) CallType <Functor> (f));
  }

  template <class Functor>
  void queue1f (void* const listener, const Functor& f)
  {
    queue1p (listener, new (m_callAllocator) CallType <Functor> (f));
  }

  template <class Member, class Functor>
  void updatef (Member member, const Functor& f)
  {
    updatep <sizeof (member)> (reinterpret_cast <void*> (&member),
                               new (m_callAllocator) CallType <Functor> (f));
  }

public:
  Listeners () { }
  ~Listeners () { }

  //
  // Add a listener to receive call notifications.
  //
  //  #1 All other functions are blocked during add().
  //  #2 The listener's receipt of every subsequent call() is guaranteed.
  //  #3 Member functions are called on the specified thread queue.
  //  #4 The listener must not already exist in the list.
  //  #5 This can be called from any thread.
  // 
  void add (ListenerClass* const listener, Worker* worker)
  {
    add_void (listener, worker);
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
  // Call a specified member function with the given arguments
  //  for every listener in the list.
  //
  //  #1 The arguments must match the function signature.
  //  #2 A listener that removes itself afterwards may not get called.
  //  #3 Calls from the same thread always execute in order.
  //  #4 Listener members will always be invoked immediately by the
  //     current thread of execution if it matches the thread used
  //     by the listener's thread queue. This happens before call() returns.
  //  #5 A listener can always remove itself even if ther are pending calls.
  //

  template <class Mf>
  void call (Mf mf)
  { callf (bind (mf, _1)); }

  template <class Mf, typename  T1>
  void call (Mf mf,   const T1& t1)
  { callf (bind (mf, _1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  void call (Mf mf,   const T1& t1, const T2& t2)
  { callf (bind (mf, _1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3)
  { callf (bind (mf, _1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void call (Mf mf,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { callf (bind (mf, _1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { callf (bind (mf, _1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { callf (bind (mf, _1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { callf (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { callf (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  //
  // Queue a call without synchronizing
  //

  template <class Mf>
  void queue (Mf mf)
  { queuef (bind (mf, _1)); }

  template <class Mf, typename  T1>
  void queue (Mf mf,   const T1& t1)
  { queuef (bind (mf, _1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  void queue (Mf mf,   const T1& t1, const T2& t2)
  { queuef (bind (mf, _1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void queue (Mf mf,   const T1& t1, const T2& t2, const T3& t3)
  { queuef (bind (mf, _1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void queue (Mf mf,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queuef (bind (mf, _1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queuef (bind (mf, _1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queuef (bind (mf, _1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queuef (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queuef (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  // These are for targeting individual listeners.
  // Use carefully!

  template <class Mf>
  void queue1 (Mf mf, ListenerClass* listener)
  { queue1f (listener, bind (mf, _1)); }

  template <class Mf, typename  T1>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1)
  { queue1f (listener, bind (mf, _1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1, const T2& t2)
  { queue1f (listener, bind (mf, _1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3)
  { queue1f (listener, bind (mf, _1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queue1f (listener, bind (mf, _1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queue1f (listener, bind (mf, _1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queue1f (listener, bind (mf, _1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queue1f (listener, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void queue1 (ListenerClass* listener,
               Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queue1f (listener, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  //
  // update()
  //
  // Like call(), but if there is a previous unprocessed call for
  // the same member f, the previous call is replaced. It is
  // up to the caller to determine if this behavior is desired.
  //

  template <class Mf>
  void update (Mf mf)
  { updatef (mf, bind (mf, _1)); }

  template <class Mf, typename  T1>
  void update (Mf mf,  const T1& t1)
  { updatef (mf, bind (mf, _1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  void update (Mf mf,  const T1& t1, const T2& t2)
  { updatef (mf, bind (mf, _1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3)
  { updatef (mf, bind (mf, _1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void update (Mf mf,  const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { updatef (mf, bind (mf, _1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { updatef (mf, bind (mf, _1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { updatef (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { updatef (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { updatef (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }
};

#endif
