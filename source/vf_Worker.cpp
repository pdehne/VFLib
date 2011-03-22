// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Mutex.h"
#include "vf/vf_Worker.h"

//
// DEPRECATED
//
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
, m_open (0)
, m_in_process (false)
{
}

Worker::~Worker ()
{
  // Someone forget to close the queue.
  vfassert (m_open.get() == 0);

  // Can't destroy queue with unprocessed calls.
  vfassert (m_calls.empty());
}

void Worker::open ()
{
#if VF_DEBUG
  bool success = m_open.compareAndSetBool (1, 0);
  vfassert (success);
#else
  m_open.compareAndSetBool (1, 0);
#endif
}

// Can still have pending calls, just can't put new ones in.

void Worker::close ()
{
#if VF_DEBUG
  bool success = m_open.compareAndSetBool (0, 1);
  vfassert (success);
#else
  m_open.compareAndSetBool (0, 1);
#endif
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

  if (!from_call)
  {
    // Recursive calls to process() are disallowed.
#if VF_DEBUG
    bool success = m_in_process.compareAndSetBool (1, 0);
    vfassert (success);
#else
    m_in_process.compareAndSetBool (1, 0);
#endif

    // Remember the thread we are called on. This
    // is done inside the mutex to handle the case
    // where the thread used to call process() changed.
    m_id = CurrentThread::getId();
  }
  else
  {
    // do_call() should have set this flag
    vfassert (m_in_process.get() == 1);

    // If we got here from do_call() then the thread
    // should have already been set and tested safely
    // while the mutex was held.
    vfassert (m_id == CurrentThread::getId());
  }

  // Atomically transfer the list of calls into an unshared
  // local variable.
  //
  // WARNING
  // These two should happen atomically but they don't.
  // My work-around is to perform the reset first. This means
  // the possibility of a signal and then causing an empty process
  // list. This is preferable to the alternative, which is no signal
  // with some unprocessed stuff. It all depends on the ordering of
  // the next two lines.
  reset ();
  // NOTE: another thread could cause signal() here
  Calls list (m_calls);

  // this is safe since list is unshared
  if (!list.empty ())
  {
    // Process the calls outside the mutex using our
    // local list. We will delete the call objects later.
    // Calling interruption points is invalid from here.
    list.reverse ();
    for (;;)
    {
      Call* call = list.pop_front ();

      if (call)
      {
        call->operator ()();
        m_allocator.Delete (call);
      }
      else
      {
        break;
      }
    }

    // Clear the recursion flag since we are past the
    // point where it is possible to invoke the functors.
    if (!from_call)
      m_in_process = false;

    did_something = true;
  }
  else
  {
    // Turn off the process flag if we turned it on
    if (!from_call)
    {
#if VF_DEBUG
      bool success = m_in_process.compareAndSetBool (0, 1);
      vfassert (success);
#else
      m_in_process.compareAndSetBool (0, 1);
#endif
    }

    did_something = false;
  }

  return did_something;
}

void Worker::do_queue (Call* c)
{
  const bool first = m_calls.push_front (c);

  // if we cause the queue to become non-empty then signal
  if (first)
    signal ();
}

// Append the Call to the queue. If this call is made from the same
// thread as the last thread that called process(), then the call
// will execute synchronously.
//
void Worker::do_call (Call* c)
{
  // If this goes off it means calls are being made after the
  // queue is closed, and probably there is no one around to
  // process it.
  vfassert (m_open.get() == 1);

  bool sync;

  {
    // Remember if we are on the process thread. Because the
    // thread id is set within the mutex, this test is atomic.
    const bool on_process_thread = CurrentThread::getId() == m_id;

    // See if do_process() is already in our call chain because
    // of tail recursion. This value is not meaningful unless
    // on_process_thread is true.
    const bool in_process = m_in_process.get() == 1;

    sync = on_process_thread && !in_process;
  }

  // If we are going to synchronize the queue then set the
  // flag to detect if another thread tries to process, which
  // is undefined behavior.
  if (sync)
  {
#if VF_DEBUG
    bool success = m_in_process.compareAndSetBool (1, 0);
    vfassert (success);
#else
    m_in_process.compareAndSetBool (1, 0);
#endif
  }

  const bool first = m_calls.push_front (c);

  // TODO: IS there a way to better manage the signal
  // when we are called on the process thread?
  // THIS LOOKS WRONG for the case of sync==true ???
  if (first)
    signal ();

  // To guarantee that calls made from the same thread as the
  // process thread happen immediately, process everything
  // in the queue if we are on the same thread. Because functors
  // may make additional calls during process(), we loop until
  // there is no work left. This is manual unrolling of the
  // implicit tail recursion.
  if (sync)
  {
    // Call do_process until there is no more work, and let it
    // know that it is being called from here instead of from process()
    //
    // TODO: It could be useful to put a limit on the number of iterations
    while (do_process (true))
    {
    }

#if VF_DEBUG
    bool success = m_in_process.compareAndSetBool (0, 1);
    // Should still be set
    vfassert (success);
#else
    m_in_process.compareAndSetBool (0, 1);
#endif
  }
}

END_VF_NAMESPACE
