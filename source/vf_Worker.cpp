// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Mutex.h"
#include "vf/vf_Worker.h"

namespace {

// Simple scoped object to turn on a flag.
class ScopedFlag : NonCopyable
{
public:
  explicit ScopedFlag (Atomic <int>& flag) : m_flag (flag)
  {
#if VF_DEBUG
    const bool success = m_flag.compareAndSetBool (1, 0);
    vfassert (success);
#else
    m_flag.compareAndSetBool (1, 0);
#endif
  }

  ~ScopedFlag ()
  {
#if VF_DEBUG
    const bool success = m_flag.compareAndSetBool (0, 1);
    vfassert (success);
#else
    m_flag.compareAndSetBool (0, 1);
#endif
  }

private:
  Atomic <int>& m_flag;
};

}

Worker::Worker (const char* szName)
: m_szName (szName)
, m_closed (0)
, m_in_process (0)
{
}

Worker::~Worker ()
{
  // Someone forget to close the queue.
  vfassert (m_closed.get() == 1);

  // Can't destroy queue with unprocessed calls.
  vfassert (m_calls.empty());
}

bool Worker::process ()
{
  // Detect recursion into do_process()
  ScopedFlag flag (m_in_process);

  // Remember this thread.
  m_id = CurrentThread::getId();

  const bool did_something = do_process ();

  return did_something;
}

// Can still have pending calls, just can't put new ones in.
void Worker::close ()
{
#if VF_DEBUG
  bool success = m_closed.compareAndSetBool (1, 0);
  vfassert (success);
#else
  m_closed.compareAndSetBool (1, 0);
#endif
}

// Process everything in the queue. The list of pending calls is
// acquired atomically. New calls may enter the queue while we are
// processing.
//
// Returns true if any functors were called.
//
bool Worker::do_process ()
{
  bool did_something;

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
  // NOTE: at this point we could be signaled, but m_calls is empty.

  // this is safe since list is unshared
  if (!list.empty ())
  {
    // process the calls
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

    did_something = true;
  }
  else
  {
    did_something = false;
  }

  return did_something;
}

// Adds a call to the queue of execution.
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
  vfassert (!closed ());

  const bool first = m_calls.push_front (c);

  // Must do this to follow spec.
  if (first)
    signal ();

  // If we are called on the process thread and we are not
  // recursed into do_process, then process the queue. This
  // makes calls from the process thread synchronous.
  //
  // NOTE: The value of in_process is invalid/volatile unless
  // this thread is the last process thread.
  //
  // NOTE: There is a small window of opportunity where we
  // might get an undesired synchronization if new thread
  // calls process() concurrently.
  //
  if (CurrentThread::getId() == m_id &&
     !in_process())
  {
    // Detect recursion into do_process()
    ScopedFlag flag (m_in_process);

    // Because functors may make additional calls during process(),
    // we loop until there is no work left. This is manual unrolling
    // of the implicit tail recursion.
    //
    // TODO: It could be useful to put a limit on the number of iterations
    while (do_process ())
    {
    }
  }
}

END_VF_NAMESPACE
