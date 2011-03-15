// Copyright (C) 2008-2011 by One Guy Group, Inc., All rights reserved worldwide.

#ifndef __VF_LISTENERS_VFHEADER__
#define __VF_LISTENERS_VFHEADER__

#include "vf/vf_List.h"
#include "vf/vf_SharedObject.h"
#include "vf/vf_Worker.h"

// List where each Listener registers with the desired Worker
// on which the call is made. Since the list traversal for an associated
// Worker is done on its thread, it is impossible for a Listener to
// get deleted while a callback is pending.

//
// TODO: CONST CORRECTNESS
//

namespace detail {

class Listeners
{
private:
  typedef unsigned long timestamp_t;

  // MAKE LIST WORK WITH SHARED OBJECTS
  class Group;
  typedef vf::List <Group> Groups;

  class Proxy;
  typedef vf::List <Proxy> Proxies;

  // Special multi-reader single writer write-preferenced
  // mutex With upgrade and downgrade capabilities.
  //
  // THIS IS GARBAGE
  //
  class ReadWriteMutex
  {
  private:
    int m_readers;
    WaitableEvent m_event;
    CriticalSection m_mutex;
    CriticalSection m_write_mutex;

  public:
    ReadWriteMutex ();
    ~ReadWriteMutex ();
    void enter_read ();
    void enter_write ();
    void exit_read ();
    void exit_write ();
    void promote_to_write ();
    void demote_to_read ();
  };

  //
  // Reference counted polymorphic unary functor of <void (Listener*)>.
  // A timestamp distinguishes a Call created before a listener is added.
  //
  class Call : public SharedObject, NonCopyable
  {
  protected:
    virtual ~Call () {}
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
    const timestamp_t m_timestamp;
  };

  template <class ListenerClass, class Functor>
  class StoredCall : public Call
  {
  public:
    StoredCall (const timestamp_t timestamp, const Functor& f)
      : Call (timestamp), m_f (f) {}
    // without this we get UB
    ~StoredCall () {}
    void operator() (void* listener)
      { m_f.operator() (static_cast <ListenerClass*> (listener)); }
  private:
    Functor m_f;
  };

  //
  // Maintains a list of listeners registered on the same thread queue
  //
  class Group : public Groups::Node, public SharedObject
  {
  public:
    typedef public SharedObjectPtr <Group> Ptr;
    explicit Group (Worker* worker);
    bool empty () const { return m_list.empty(); } // caller syncs
    void add (void* listener, const unsigned long timestamp);
    bool remove (void* listener);
    bool contains (void const* listener) const;
    void queue_call (Call::Ptr c);
    void do_call (Call::Ptr c, Group::Ptr);
    Worker* getWorker () { return m_worker; }

  private:
    virtual ~Group ();

  private:
    // Global deleted list from a pool of
    // recycled blocks would be best for this
    struct Entry
    {
      void* listener; // the listener
      timestamp_t timestamp; // timestamp when they were added
      bool came_before (Call const* c) const
        { return c->is_newer_than (timestamp); }
    };
    typedef std::list <Entry> list_t;

    list_t m_list;
    void* m_listener;
    CriticalSection m_mutex;
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
    typedef vf::List <Entry> Entries;
    struct Entry : Entries::Node, SharedObject
    {
      typedef SharedObjectPtr <Entry> Ptr;
      explicit Entry (Group::Ptr g) : group (g) {}
      ~Entry () { jassert (call.get () == 0); }
      Group::Ptr group;
      Atomic <Call*> call;
    };
    void do_call (Entry::Ptr entry);

  private:
    Entries m_entries;
    Atomic <Call*> m_call;
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
  Listeners ();
  ~Listeners ();
  void queue_call (Call::Ptr c);
  void add_void (void* const listener, Worker* worker);
  void remove_void (void* const listener);

  template <class ListenerClass, class Functor>
  Call::Ptr newCall (const Functor& f)
  {
    // Mutex is required to access the timestamp. Ownership of the
    // lock is transferred to the caller since we dont release it.
    m_groups_mutex.enter_read ();
    return new StoredCall <ListenerClass, Functor> (m_timestamp, f);
  }

  // Search for an existing Proxy that matches the pointer to
  // member and replace it's Call, or create a new Proxy for it.
  // Caller must acquire the read lock.
  template <int Bytes>
  void proxy_call (const void *member, Call::Ptr c)
  {
    if (!m_groups.empty ())
    {
      // TODO: CHANGE THIS to an atomic fast path
      // with a reader count and an unlocked creation
      m_proxies_mutex.enter_read ();

      // See if there's already a proxy
      Proxy* proxy = find_proxy (member, Bytes);

      if (!proxy)
      {
        // Create a new empty proxy
        proxy = new StoredProxy <Bytes> (member);

        // We need the write lock to keep readers
        // out of find_proxy() and do_calls() while
        // we construct
        m_proxies_mutex.promote_to_write ();

        // Add all current groups to the Proxy.
        // We need the group read lock for this.
        for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
        {
          Group* group = *iter++;
          proxy->add (group);
        }

        // Add it to the list.
        m_proxies.push_front (proxy);

        m_proxies_mutex.exit_write ();
      }
      else
      {
        m_proxies_mutex.exit_read ();
      }

      proxy->do_calls (c);
      
      // Caller acquired the read mutex so release it.
      m_groups_mutex.exit_read ();
    }
    else
    {
      // No Groups, nothing to do.
      m_groups_mutex.exit_read ();
    }
  }

private:
  // Global deleted list from a pool of
  // recycled blocks would be best for this
  Groups m_groups;
  Proxies m_proxies;
  timestamp_t m_timestamp;
  ReadWriteMutex m_groups_mutex;
  ReadWriteMutex m_proxies_mutex;
};

}

//------------------------------------------------------------------------------

template <class ListenerClass>
class Listeners : public detail::Listeners
{
private:
  template <class Functor>
  void queue_fn (const Functor& f)
  {
    queue_call (newCall <ListenerClass> (f));
  }

  template <class Member, class Function>
  void proxy_fn (Member member, const Function& f)
  {
#if 1
    proxy_call <sizeof (member)> (reinterpret_cast <void*> (&member),
                                  newCall <ListenerClass> (f));
#else
    queue_fn (f);
#endif
  }

public:
  Listeners () {}
  ~Listeners () {}

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
  { queue_fn (bind (mf, _1)); }

  template <class Mf, typename  T1>
  void call (Mf mf,   const T1& t1)
  { queue_fn (bind (mf, _1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  void call (Mf mf,   const T1& t1, const T2& t2)
  { queue_fn (bind (mf, _1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3)
  { queue_fn (bind (mf, _1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void call (Mf mf,   const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void call (Mf mf,   const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { queue_fn (bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }

  //
  // call1()
  //
  // Like call(), but if there is a previous unprocessed call for
  // the same member f, the previous call is replaced. It is
  // up to the caller to determine if this behavior is desired.
  //

  template <class Mf>
  void call1 (Mf mf)
  { proxy_fn (mf, bind (mf, _1)); }

  template <class Mf, typename  T1>
  void call1 (Mf mf,  const T1& t1)
  { proxy_fn (mf, bind (mf, _1, t1)); }

  template <class Mf, typename  T1, typename  T2>
  void call1 (Mf mf,  const T1& t1, const T2& t2)
  { proxy_fn (mf, bind (mf, _1, t1, t2)); }

  template <class Mf, typename  T1, typename  T2, typename  T3>
  void call1 (Mf mf,  const T1& t1, const T2& t2, const T3& t3)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3)); }

  template <class Mf, typename  T1, typename  T2,
                      typename  T3, typename  T4>
  void call1 (Mf mf,  const T1& t1, const T2& t2,
                      const T3& t3, const T4& t4)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5>
  void call1 (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5)); }

  template <class Mf, typename  T1, typename  T2, typename  T3,
                      typename  T4, typename  T5, typename  T6>
  void call1 (Mf mf,  const T1& t1, const T2& t2, const T3& t3,
                      const T4& t4, const T5& t5, const T6& t6)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7>
  void call1 (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Mf, typename  T1, typename  T2, typename  T3, typename  T4,
                      typename  T5, typename  T6, typename  T7, typename  T8>
  void call1 (Mf mf,  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                      const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { proxy_fn (mf, bind (mf, _1, t1, t2, t3, t4, t5, t6, t7, t8)); }
};

#endif
