// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Listeners.h"

//------------------------------------------------------------------------------
//
// Group
//
// - A list of listeners associated with the same Worker.
//
// - The list is only iterated on the Worker's thread.
//
// - It is safe to add or remove listeners from the group
//   at any time.
//

ListenersBase::Group::Group (Worker* const worker)
  : m_worker (worker)
  , m_listener (0)
{
}

ListenersBase::Group::~Group ()
{
  // If this goes off it means a Listener forgot to remove itself.
  vfassert (m_list.empty ());

  // shouldn't be deleting group during a call
  vfassert (m_listener == 0);
}

// Add the listener with the given timestamp.
// The listener will only get calls with higher timestamps.
// The caller must prevent duplicates.
//
void ListenersBase::Group::add (void* listener,
                                const timestamp_t timestamp)
{
  LockFree::ReadWriteMutex::ScopedWriteLockType lock (m_mutex);

  vfassert (!contains (listener));

  // Should never be able to get here while in call()
  vfassert (m_listener == 0);

  // Add the listener and remember the time stamp so we don't
  // send it calls that were queued earlier than the add().
  Entry* entry = globalAlloc <Entry>::New (listener, timestamp);
  m_list.push_back (entry);
}

// Removes the listener from the group if it exists.
// Returns true if the listener was removed.
//
bool ListenersBase::Group::remove (void* listener)
{
  bool found = false;

  LockFree::ReadWriteMutex::ScopedWriteLockType lock (m_mutex);

  // Should never be able to get here while in call()
  vfassert (m_listener == 0);

  for (List::iterator iter = m_list.begin(); iter != m_list.end(); ++iter)
  {
    Entry* entry = *iter;
    if (entry->listener == listener)
    {
      m_list.remove (entry);
      globalDelete (entry);
      found = true;
      break;
    }
  }

  return found;
}

// Used for assertions.
// The caller must synchronize.
//
bool ListenersBase::Group::contains (void const* listener) /*const*/
{
  bool found = false;

  for (List::iterator iter = m_list.begin(); iter != m_list.end(); iter++)
  {
    if (iter->listener == listener)
    {
      found = true;
      break;
    }
  }

  return found;
}

void ListenersBase::Group::call (Call* const c, const timestamp_t timestamp)
{
  vfassert (!empty ());
  m_worker->callp (new (m_worker->getAllocator()) Work (this, c, timestamp));
}

void ListenersBase::Group::queue (Call* const c, const timestamp_t timestamp)
{
  vfassert (!empty ());
  m_worker->queuep (new (m_worker->getAllocator()) Work (this, c, timestamp));
}

void ListenersBase::Group::queue1 (Call* const c, const timestamp_t timestamp, void* const listener)
{
  m_worker->queuep (new (m_worker->getAllocator()) Work1 (this, c, timestamp, listener));
}

// Queues a reference to the Call on the thread queue of each listener
// that is currently in our list. The thread queue must be in the
// stack's call chain, either directly from Worker::process(),
// or from Proxy::do_call() called from Worker::process().
//
void ListenersBase::Group::do_call (Call* const c, const timestamp_t timestamp)
{
  if (!empty ())
  {
    LockFree::ReadWriteMutex::ScopedReadLockType lock (m_mutex);

    // Recursion not allowed.
    vfassert (m_listener == 0);

    // The body of the loop MUST NOT cause listeners to get called.
    // Therefore, we don't have to worry about listeners removing
    // themselves while iterating the list.
    //
    for (List::iterator iter = m_list.begin(); iter != m_list.end();)
    {
      Entry* entry = *iter++;

      // Since it is possible for a listener to be added after a
      // Call gets queued but before it executes, this prevents listeners
      // from seeing Calls created before they were added.
      //
      if (timestamp > entry->timestamp)
      {
        m_listener = entry->listener;

        // The thread queue's process() function MUST be in our call
        // stack to guarantee that these calls will not execute immediately.
        // They will be handled by the tail recusion unrolling in the
        // thread queue.
        vfassert (m_worker->in_process ());

        m_worker->callp (new (m_worker->getAllocator()) Call::Work (c, m_listener));

        m_listener = 0;
      }
    }
  }
  else
  {
    // last listener was removed before we got here,
    // and the parent listener list may have been deleted.
  }
}

void ListenersBase::Group::do_call1 (Call* const c, const timestamp_t timestamp, void* const listener)
{
  if (!empty ())
  {
    LockFree::ReadWriteMutex::ScopedReadLockType lock (m_mutex);

    // Recursion not allowed.
    vfassert (m_listener == 0);

    for (List::iterator iter = m_list.begin(); iter != m_list.end();)
    {
      Entry* entry = *iter++;

      if (entry->listener == listener)
      {
        if (timestamp > entry->timestamp)
        {
          m_listener = entry->listener;

          vfassert (m_worker->in_process ());

          m_worker->callp (new (m_worker->getAllocator()) Call::Work (c, m_listener));

          m_listener = 0;
        }
      }
    }
  }
  else
  {
    // Listener was removed
  }
}

//------------------------------------------------------------------------------
//
// Proxy
//
//------------------------------------------------------------------------------

//
// Proxy
//
// This object acts as a proxy for a contained Call and gets put on Group
// thread queues. The proxy is kept in a list so if new calls are made
// for the same member before the existing calls execute, the existing
// calls can be replaced.
//
// Once a Proxy is created it lives for the lifetime of the listeners.
//
ListenersBase::Proxy::Proxy ()
{
}

ListenersBase::Proxy::~Proxy ()
{
  // If the proxy is getting destroyed it means:
  // - the listeners object is getting destroyed
  // - all listeners must have removed themselves
  // - all thread queues have been fully processed
  // Therefore, our entries should be gone.

  // NO it is possible for an empty Group, for which
  // the parent listeners object has been destroyed,
  // to still exist in a thread queue!!!

  // But all listeners should have removed themselves
  // so our list of groups should still be empty.
  vfassert (m_entries.empty ());
}

// Adds the group to the Proxy.
// Caller must have the proxies mutex.
// Caller is responsible for preventing duplicates.
void ListenersBase::Proxy::add (Group* group)
{
  Entry* entry (globalAlloc <Entry>::New (group));

  // Manual addref and put raw pointer in list
  entry->incReferenceCount ();
  m_entries.push_back (entry);
}

// Removes the group from the Proxy.
// Caller must have the proxies mutex.
// Caller is responsible for making sure the group exists.
void ListenersBase::Proxy::remove (Group* group)
{
  for (Entries::iterator iter = m_entries.begin(); iter != m_entries.end();)
  {
    Entry* entry = *iter++;

    if (entry->group == group)
    {
      // remove from list and manual release
      m_entries.remove (entry);
      entry->decReferenceCount();

      // Entry might still be in the empty group's thread queue
      break;
    }
  }
}

// For each group, updates the call.
// Queues each group that isn't already queued.
// Caller must acquire the group read lock.
//
void ListenersBase::Proxy::update (Call* const c, const timestamp_t timestamp)
{
  // why would we even want to be called?
  vfassert (!m_entries.empty());

  // With the read lock, this list can't change on us unless someone
  // adds a listener to a new thread queue in response to a call.
  for (Entries::iterator iter = m_entries.begin(); iter != m_entries.end();)
  {
    Entry* entry = *iter++;

    // Manually add a reference since we use a raw pointer
    c->incReferenceCount ();

    // Atomically exchange the new call for the old one
    Call* old = entry->call.exchange (c);

    // If no old call then they need to be queued
    if (!old)
    {
      Worker& worker = *entry->group->getWorker();
      worker.callp (new (worker.getAllocator ()) Work (this, entry, timestamp));
    }
    else
    {
      old->decReferenceCount ();
    }
  }
}

void ListenersBase::Proxy::Work::operator() ()
{
  ListenersBase::Call* c = m_entry->call.exchange (0);

  Group* group = m_entry->group;

  if (!group->empty ())
    group->do_call (c, m_timestamp);

  c->decReferenceCount ();
}

//------------------------------------------------------------------------------
//
// ListenersBase
//
//------------------------------------------------------------------------------

ListenersBase::ListenersBase ()
  : m_timestamp (0)
{
}

ListenersBase::~ListenersBase ()
{
  for (Groups::iterator iter = m_groups.begin (); iter != m_groups.end ();)
  {
    Group* group = *iter++;

    // If this goes off it means a Listener forgot to remove.
    vfassert (group->empty ());

    group->decReferenceCount ();
  }

  // Proxies are never deleted until here.
  for (Proxies::iterator iter = m_proxies.begin(); iter != m_proxies.end ();)
  {
    Proxy* proxy = *iter++;
    globalDelete (proxy);
  }
}

// Searches for a proxy that matches the pointer to member.
// Caller synchronizes.
ListenersBase::Proxy* ListenersBase::find_proxy (const void* member, int bytes)
{
  Proxy* proxy = 0;

  for (Proxies::iterator iter = m_proxies.begin (); iter != m_proxies.end (); ++iter)
  {
    if ((*iter)->match (member, bytes))
    {
      proxy = *iter;
      break;
    }
  }

  return proxy;
}

void ListenersBase::add_void (void* const listener, Worker* worker)
{
  vfassert (worker != 0);

  LockFree::ScopedWriteLock lock (m_groups_mutex);

#ifdef JUCE_DEBUG
  // Make sure the listener has not already been added
  // SHOULD USE const_iterator!
  for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
  {
    Group* group = *iter++;

    // We can be in do_call() on another thread now, but it
    // doesn't modify the list, and we have the write lock.
    vfassert (!group->contains (listener));
  }
#endif

  // See if we already have a Group for this thread queue.
  Group::Ptr group;

  // SHOULD USE const_iterator
  for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
  {
    Group::Ptr cur = *iter++;
    if (cur->getWorker() == worker)
    {
      group = cur;
      break;
    }
  }

  if (!group)
  {
    group = globalAlloc <Group>::New (worker);

    // Add it to the list, and give it a manual ref
    // since the list currently uses raw pointers.
    group->incReferenceCount ();
    m_groups.push_back (group);

    // Tell existing proxies to add the group
    {
      LockFree::ScopedReadLock lock (m_proxies_mutex);

      for (Proxies::iterator iter = m_proxies.begin (); iter != m_proxies.end ();)
      {
        Proxy* proxy = *iter++;
        proxy->add (group);
      }
    }
  }

  // Add the listener to the group with the current timestamp
  group->add (listener, m_timestamp);

  // Increment the timestamp within the mutex so
  // future calls will be newer than this listener.
  ++m_timestamp;
}

void ListenersBase::remove_void (void* const listener)
{
  LockFree::ScopedWriteLock lock (m_groups_mutex);

  // Make sure the listener exists
#ifdef JUCE_DEBUG
  {
    bool exists = false;

    for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
    {
      Group* group = *iter++;

      // this should never happen while we hold the mutex
      vfassert (!group->empty ());

      if (group->contains (listener))
      {
        vfassert (!exists); // added twice?

        exists = true;
        // keep going to make sure there are no empty groups
      }
    }

    vfassert (exists);
  }
#endif

  // Find the group and remove
  for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
  {
    Group::Ptr group = *iter++;

    // If the listener is in there, take it out.
    if (group->remove (listener))
    {
      // Are we the last listener?
      if (group->empty ())
      {
        // Tell proxies to remove the group
        {
          LockFree::ScopedWriteLock lock (m_proxies_mutex);

          for (Proxies::iterator iter = m_proxies.begin (); iter != m_proxies.end ();)
          {
            Proxy* proxy = *iter++;
            proxy->remove (group);
          }
        }

        // Remove it from the list and manually release
        // the reference since the list uses raw pointers.
        m_groups.remove (group.getObject());
        group->decReferenceCount();

        // It is still possible for the group to exist at this
        // point in a thread queue but it will get processed,
        // do nothing, and release its own final reference.
      }

      break;
    }
  }
}

void ListenersBase::callp (Call::Ptr cp)
{
  Call* c = cp;

  LockFree::ScopedReadLock lock (m_groups_mutex);

  // can't be const iterator because queue() might cause called functors
  // to modify the list.
  for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
    (*iter++)->call (c, m_timestamp);
}

void ListenersBase::queuep (Call::Ptr cp)
{
  Call* c = cp;

  LockFree::ScopedReadLock lock (m_groups_mutex);

  // can't be const iterator because queue() might cause called functors
  // to modify the list.
  for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
    (*iter++)->queue (c, m_timestamp);
}

void ListenersBase::queue1p (void* const listener, Call::Ptr cp)
{
  Call* c = cp;

  LockFree::ScopedReadLock lock (m_groups_mutex);

  // can't be const iterator because queue() might cause called functors
  // to modify the list.
  for (Groups::iterator iter = m_groups.begin(); iter != m_groups.end();)
  {
    Group* group = *iter++;
    if (group->contains (listener))
    {
      group->queue1 (c, m_timestamp, listener);
      break;
    }
  }
}

END_VF_NAMESPACE
