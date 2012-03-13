// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

Worker::Worker (String name)
: m_name (name)
{
}

Worker::~Worker ()
{
  // Someone forget to close the queue.
  vfassert (m_closed.isSet ());

  // Can't destroy queue with unprocessed calls.
  vfassert (m_list.empty ());
}

bool Worker::isAssociatedWithCurrentThread () const
{
  return CurrentThread::getId() == m_id;
}

// Adds a call to the queue of execution.
void Worker::queuep (Call* c)
{
  // If this goes off it means calls are being made after the
  // queue is closed, and probably there is no one around to
  // process it.
	  vfassert (m_closed.isClear());

  if (m_list.push_back (c))
    signal ();
}

// Append the Call to the queue. If this call is made from the same
// thread as the last thread that called process(), then the call
// will execute synchronously.
//
void Worker::callp (Call* c)
{
  queuep (c);

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
  if (isAssociatedWithCurrentThread () &&
      m_in_process.trySet ())
  {
    do_process ();

    m_in_process.clear ();
  }
}

void Worker::associateWithCurrentThread ()
{
  vfassert (m_in_process.isClear ());

  m_id = CurrentThread::getId();
}

bool Worker::process ()
{
  bool did_something;

  // Detect recursion into do_process(), and
  // break ties for concurrent calls atomically.
  //
  if (m_in_process.trySet ())
  {
    // Remember this thread.
    m_id = CurrentThread::getId();

    did_something = do_process ();

    m_in_process.clear ();
  }
  else
  {
    did_something = false;
  }

  return did_something;
}

// Can still have pending calls, just can't put new ones in.
void Worker::close ()
{
  m_closed.set ();
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

  // Reset since we are emptying the queue. Since we loop
  // until the queue is empty, it is possible for us to exit
  // this function with an empty queue and signaled state.
  //
  reset ();

  Call* call = m_list.pop_front ();

  if (call)
  {
    did_something = true;

    // This method of processing one at a time has the side
    // effect of synchronizing calls to us from a functor.
    //
    for (;;)
    {
      call->operator() ();
      delete call;

      call = m_list.pop_front ();
      if (call == 0)
        break;
    }
  }
  else
  {
    did_something = false;
  }

  return did_something;
}
