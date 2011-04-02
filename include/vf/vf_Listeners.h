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

#include "vf/vf_Mutex.h" // REMOVE ASAP!!

// List where each Listener registers with the desired Worker
// on which the call is made. Since the list traversal for an associated
// Worker is done on its thread, it is impossible for a Listener to
// get deleted while a callback is pending.

//
// TODO: CONST CORRECTNESS? List CONST-NESS?
//

#define NEW_ALLOCATOR 0

class ListenersBase
{
protected:
  typedef unsigned long timestamp_t;

  //
  // Reference counted polymorphic unary functor void (*)(void* listener).
  // A timestamp distinguishes a Call created before a listener is added.
  //
  class Call : public SharedObject
  {
  public:
    typedef SharedObjectPtr <Call> Ptr;

    explicit Call (const timestamp_t timestamp)
      : m_timestamp (timestamp) { }
    
    bool is_newer_than (const timestamp_t when) const
      { return m_timestamp > when; }

    static void do_call (Ptr c, void* listener)
      { c->operator()(listener); }

    virtual void operator ()(void* listener) = 0;


  private:
    void destroySharedObject ()
#if NEW_ALLOCATOR
      { this->~Call(); LockFree::Allocator::deallocate (this); }
#else
      { globalDelete (this); }
#endif

  public:
    const timestamp_t m_timestamp;
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

    explicit Group    (Worker* worker);
    virtual ~Group    ();
    void add          (void* listener, const unsigned long timestamp);
    bool remove       (void* listener);
    bool contains     (void const* listener);
    void queue_call   (Call::Ptr c, bool sync);
    void do_call      (Call::Ptr c, const timestamp_t timestamp, Group::Ptr);

    bool empty        () { return m_list.empty(); }
    Worker* getWorker () { return m_worker; }

  private:
    void destroySharedObject ()
      { globalDelete (this); }

  private:
    struct Entry;
    typedef List <Entry> List;
    struct Entry : List::Node
    {
      void* listener; // the listener
      timestamp_t timestamp; // timestamp when they were added
      bool came_before (Call const* c) const
        { return c->is_newer_than (timestamp); }
    };

    List m_list;
    void* m_listener;
    LockFree::ReadWriteMutex m_mutex;
    Worker* m_worker;
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
    void add (Group::Ptr group);
    void remove (Group::Ptr group);
    void do_calls (Call::Ptr c);
    virtual bool match (const void* member, int bytes) const = 0;
  private:
    struct Entry;
    typedef List <Entry> Entries;
    struct Entry : Entries::Node, SharedObject
    {
      typedef SharedObjectPtr <Entry> Ptr;
      explicit Entry (Group::Ptr g) : group (g) {}
      ~Entry () { jassert (call.get () == 0); }
      void destroySharedObject () { globalDelete (this); }
      Group::Ptr group;
      Atomic::Pointer <Call> call;
    };
    void do_call (Entry::Ptr entry);

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
  void queue_call (Call::Ptr c, bool sync);
  void add_void (void* const listener, Worker* worker);
  void remove_void (void* const listener);

  // Search for an existing Proxy that matches the pointer to
  // member and replace it's Call, or create a new Proxy for it.
  // Caller must acquire the group read lock.
  template <int Bytes>
  void proxy_call (const void *member, Call::Ptr c)
  {
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
      proxy->do_calls (c);
    }
  }

private:
  Groups m_groups;
  Proxies m_proxies;
  LockFree::ReadWriteMutex m_proxies_mutex;
protected:
  timestamp_t m_timestamp;
  LockFree::ReadWriteMutex m_groups_mutex;
};

//------------------------------------------------------------------------------

template <class ListenerClass>
class Listeners : public ListenersBase
{
private:
  template <class Functor>
  class StoredCall : public Call
  {
  public:
    StoredCall (const timestamp_t timestamp, const Functor& f)
      : Call (timestamp), m_f (f) { }
    ~StoredCall ()
      { } // without this we get UB
    void operator() (void* listener)
      { m_f.operator() (static_cast <ListenerClass*> (listener)); }

  private:
    Functor m_f;
  };

  // Call that only executes for a matching listener.
  // The timestamp prevents the ABA problem.
  template <class Functor>
  class StoredCall1 : public Call
  {
  public:
    StoredCall1 (ListenerClass* listener, const timestamp_t timestamp, const Functor& f)
      : Call (timestamp), m_listener (listener), m_f (f) { }
    ~StoredCall1 ()
      { } // without this we get UB
    void operator() (void* listener)
    {
      if (m_listener == listener)
        m_f.operator() (static_cast <ListenerClass*> (listener));
    }

  private:
    void* m_listener;
    Functor m_f;
  };

  // Caller is responsible for a group read lock.
  template <class Functor>
  Call::Ptr newCall (const Functor& f)
  {
    // group read lock needed for access to m_timestamp    
#if NEW_ALLOCATOR
    return new (GlobalAllocator().allocate (sizeof(StoredCall <Functor>)))
      StoredCall <Functor> (m_timestamp, f);
#else
    return globalAlloc <StoredCall <Functor> >::New (m_timestamp, f);
#endif
  }

  template <class Functor>
  Call::Ptr newCall1 (ListenerClass* listener, const Functor& f)
  {
    // group read lock needed for access to m_timestamp
#if NEW_ALLOCATOR
    return new (GlobalAllocator().allocate (sizeof (StoredCall1 <Functor>)))
      StoredCall1 <Functor> (listener, m_timestamp, f);
#else
    return globalAlloc <StoredCall1 <Functor> >::New (listener, m_timestamp, f);
#endif
  }

  template <class Functor>
  void queue_fn (const Functor& f, bool sync)
  {
    LockFree::ScopedReadLock lock (m_groups_mutex);

    queue_call (newCall (f), sync);
  }

  template <class Functor>
  void queue1_fn (ListenerClass* listener, const Functor& f, bool sync)
  {
    LockFree::ScopedReadLock lock (m_groups_mutex);

    queue_call (newCall1 (listener, f), sync);
  }

  template <class Member, class Function>
  void proxy_fn (Member member, const Function& f)
  {
    LockFree::ScopedReadLock lock (m_groups_mutex);

    proxy_call <sizeof (member)> (reinterpret_cast <void*> (&member),
                                  newCall (f));
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
  { queue_fn (bind (mf, _1), true); }

  template <class Mf, typename  T1>
  void call (Mf mf,   const T1& t1)
  { queue_fn (bind (mf, _1, t1), true); }

  template <class Mf, typename  T1, typename  T2>
  void call (Mf mf,   const T1& t1, const T2& t2)
  { queue_fn (bind (mf, _1, t1, t2), true); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3)
  { queue_fn (bind (mf, _1, t1, t2, t3), true); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void call (Mf mf,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4), true); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5), true); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6), true); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7), true); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8), true); }

  //
  // Queue a call without synchronizing
  //

  template <class Mf>
  void queue (Mf mf)
  { queue_fn (bind (mf, _1), false); }

  template <class Mf, typename  T1>
  void queue (Mf mf,   const T1& t1)
  { queue_fn (bind (mf, _1, t1), false); }

  template <class Mf, typename  T1, typename  T2>
  void queue (Mf mf,   const T1& t1, const T2& t2)
  { queue_fn (bind (mf, _1, t1, t2), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void queue (Mf mf,   const T1& t1, const T2& t2, const T3& t3)
  { queue_fn (bind (mf, _1, t1, t2, t3), false); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void queue (Mf mf,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void queue (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8), false); }

  // These are for targeting individual listeners.
  // Use carefully!

  template <class Mf>
  void queue1 (Mf mf, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1), false); }

  template <class Mf, typename  T1>
  void queue1 (Mf mf, const T1& t1, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1), false); }

  template <class Mf, typename  T1, typename  T2>
  void queue1 (Mf mf, const T1& t1, const T2& t2, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1, t2), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void queue1 (Mf mf, const T1& t1, const T2& t2, const T3& t3, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1, t2, t3), false); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void queue1 (Mf mf, const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1, t2, t3, t4), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void queue1 (Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1, t2, t3, t4, t5), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void queue1 (Mf mf, const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1, t2, t3, t4, t5, t6), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void queue1 (Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7), false); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void queue1 (Mf mf, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8, ListenerClass* listener)
  { queue1_fn (listener, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8), false); }

  //
  // update()
  //
  // Like call(), but if there is a previous unprocessed call for
  // the same member f, the previous call is replaced. It is
  // up to the caller to determine if this behavior is desired.
  //

  template <class Mf>
  void update (Mf mf)
  { proxy_fn (mf, bind (mf, _1)); }

  template <class Mf, typename  T1>
  void update (Mf mf,  const T1& t1)
  { proxy_fn (mf, bind (mf, _1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  void update (Mf mf,  const T1& t1, const T2& t2)
  { proxy_fn (mf, bind (mf, _1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void update (Mf mf,  const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void update (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }
};

#endif
