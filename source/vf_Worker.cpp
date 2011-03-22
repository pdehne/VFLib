// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Mutex.h"
#include "vf/vf_Worker.h"

// If this is set to 1, then call() may execute the functor
// immediately if the call is made from the thread used to process the queue.
#define SYNCHRONIZED_CALL 1

// If this is 1 then we will use a custom allocator
#define CUSTOM_ALLOCATOR 0

#define LOCK_FREE 1

namespace {

class Allocator
{
public:
  void* alloc (size_t bytes) { return ::operator new (bytes); }
  void free (void* p) { ::operator delete (p); }
  void lock () { }
  void unlock () { }
  void locked_free (void* p) { this->free (p); }
};

}

static Allocator globalAllocator;

void* Worker::global_alloc (size_t bytes)
{
  return globalAllocator.alloc (bytes);
}

void Worker::global_free (void* p)
{
  globalAllocator.free (p);
}

//------------------------------------------------------------------------------

Worker::Worker (const char* szName)
: m_szName (szName)
, m_open (false)
, m_in_process (false)
{
}

Worker::~Worker ()
{
  ScopedLock lock (m_mutex);

  // Someone forget to close the queue.
  vfassert (!m_open);

  // Can't destroy queue with unprocessed calls.
  vfassert (m_calls.empty());
}

void Worker::open ()
{
  ScopedLock lock (m_mutex);

  vfassert (!m_open);

  m_open = true;
}

// Can still have pending calls, just can't put new ones in.

void Worker::close ()
{
  ScopedLock lock (m_mutex);

  vfassert (m_open);

  m_open = false;
}

bool Worker::process ()
{
  return do_process (false);
}

// Process everything in the queue. The list of pending calls is
// acquired atomically. New calls may enter the queue while we are
// processing.
//
// Returns true if any functors were called.
//
bool Worker::do_process (const bool from_call)
{
  bool did_something;

  m_mutex.enter ();

  if (!from_call)
  {
    // Recursive calls to process() are disallowed.
    vfassert (!m_in_process);
    m_in_process = true;

    // Remember the thread we are called on. This
    // is done inside the mutex to handle the case
    // where the thread used to call process() changed.
    m_id = CurrentThread::getId();
  }
  else
  {
    // do_call() should have set this flag
    vfassert (m_in_process);

    // If we got here from do_call() then the thread
    // should have already been set and tested safely
    // while the mutex was held.
    vfassert (m_id == CurrentThread::getId());
  }

  // Transfer the current set of calls into a local
  // list to process so that we can process the functors
  // without holding the mutex.
  //list.append (m_calls);
  Calls list (m_calls);

#if LOCK_FREE
  reset ();
#endif

  m_mutex.exit ();

  if (!list.empty ())
  {
    // Process the calls outside the mutex using our
    // local list. We will delete the call objects later.
    // Calling interruption points is invalid from here.
    for (Calls::iterator iter = list.begin(); iter != list.end();)
    {
      Call* call = *iter++;
      call->operator ()();

#if LOCK_FREE
      m_allocator.Delete (call);
#endif
    }

    // Clear the recursion flag since we are past the
    // point where it is possible to invoke the functors.
    if (!from_call)
      m_in_process = false;

#if ! LOCK_FREE
    {
      // Coalesce all the deletes under a single lock
      globalAllocator.lock ();

      // Do all the deletes
      for (Calls::iterator iter = list.begin (); iter != list.end ();)
      {
        Call* e = *iter++;
        e->~Call();
        globalAllocator.locked_free (e);
      }

      globalAllocator.unlock ();
    }

    // We defer the reset() so that the related thread
    // implementation doesn't have to use a condition
    // variable to synchronize its "interrupted" flag.
    //
    // Because we defer the call to reset(), functors added
    // after we picked up the list won't call signal(), therefore
    // we have to do it here to preserve behavior.
    {
      m_mutex.enter ();

      // Reset since we emptied the list.
      reset ();

      // But if there's anything new we have to re-signal.
      if (!m_calls.empty ())
        signal ();

      m_mutex.exit ();
    }
#endif

    did_something = true;
  }
  else
  {
    // Turn off the process flag if we turned it on
    if (!from_call)
    {
      m_mutex.enter ();
      vfassert (m_in_process);
      m_in_process = false;
      m_mutex.exit ();
    }

    did_something = false;
  }

  return did_something;
}

void Worker::do_queue (Call* c)
{
  ScopedLock lock (m_mutex);

  const bool was_empty = m_calls.empty ();

  m_calls.push_back (c);

  if (was_empty)
    signal ();
}

// Append the Call to the queue. If this call is made from the same
// thread as the last thread that called process(), then the call
// will execute synchronously.
//
void Worker::do_call (Call* c)
{
  m_mutex.enter ();

  // If this goes off it means calls are being made after the
  // queue is closed, and probably there is no one around to
  // process it.
  vfassert (m_open);

#if SYNCHRONIZED_CALL
  bool sync;

  {
    // Remember if we are on the process thread. Because the
    // thread id is set within the mutex, this test is atomic.
    const bool on_process_thread = CurrentThread::getId() == m_id;

    // See if do_process() is already in our call chain because
    // of tail recursion. This value is not meaningful unless
    // on_process_thread is true.
    const bool in_process = m_in_process;

    sync = on_process_thread && !in_process;
  }

  // If we are going to synchronize the queue then set the
  // flag to detect if another thread tries to process, which
  // is undefined behavior.
  if (sync)
    m_in_process = true;
#endif

  const bool was_empty = m_calls.empty ();

  m_calls.push_back (c);

  // TODO: IS there a way to better manage the signal
  // when we are called on the process thread?
  if (was_empty)
    signal ();

  m_mutex.exit ();

  // To guarantee that calls made from the same thread as the
  // process thread happen immediately, process everything
  // in the queue if we are on the same thread. Because functors
  // may make additional calls during process(), we loop until
  // there is no work left. This is manual unrolling of the
  // implicit tail recursion.
#if SYNCHRONIZED_CALL
  if (sync)
  {
    // Call do_process until there is no more work, and let it
    // know that it is being called from here instead of from process()
    //
    // TODO: It could be useful to put a limit on the number of iterations
    while (do_process (true))
    {
    }

    {
      m_mutex.enter ();

      // Should still be set
      vfassert (m_in_process);

      // set the flag back
      m_in_process = false;

      m_mutex.exit ();
    }
  }
#endif
}

END_VF_NAMESPACE
