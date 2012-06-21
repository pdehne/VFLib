/*============================================================================*/
/*
  Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
  See the file GNU_GPL_v2.txt for full licensing terms.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*============================================================================*/

#ifndef VF_INTERRUPTIBLETHREAD_VFHEADER
#define VF_INTERRUPTIBLETHREAD_VFHEADER

#include "../diagnostic/vf_SafeBool.h"
#include "../functor/vf_Function.h"

/*============================================================================*/
/**
  A thread with soft interruption support.

  The thread must periodically call interruptionPoint(), which returns
  true the first time an interruption has occurred since the last call to
  interruptionPoint().

  @ingroup vf_core
*/
class InterruptibleThread : public Thread
{
public:
  // This is the flag used to indicate if an interruption
  // occurred when using the polling model. It is designed
  // to detect improper usage (specifically, not checking
  // the flag, which would result in incorrect behavior).
  //
  // #1 The default constructor must produce an object that
  //    is considered non-signaled (i.e. not interrupted)
  //    in order for ThreadWithCallQueue that uses a default Callable
  //    for its idle function to work.
  //
  class Interrupted : public SafeBool <Interrupted>
  {
  public:
    Interrupted ()
     : m_interrupted (false)
     , m_checked (false)
    {
    }

    explicit Interrupted (bool interrupted)
     : m_interrupted (interrupted)
     , m_checked (false)
    {
    }

    Interrupted (const Interrupted& other)
      : m_interrupted (other.m_interrupted)
      , m_checked (false)
    {
      other.m_checked = true;
    }

    ~Interrupted ()
    {
      vfassert (!m_interrupted || m_checked);
    }

    Interrupted& operator= (const Interrupted& other)
    {
      m_interrupted = other.m_interrupted;
      m_checked = false;
      other.m_checked = true;
      return *this;
    }

    bool asBoolean () const
    {
      m_checked = true;
      return m_interrupted;
    }

  private:
    bool m_interrupted;
    bool mutable m_checked;
  };

  typedef Thread::ThreadID id;

public:
  /**
    Construct an interruptible thread.

    The name is used for debugger diagnostics.

    @param name The name of the thread.
  */
  explicit InterruptibleThread (String name);

  /**
    Destroy the interruptible thread.

    This will signal an interrupt and wait until the thread exits.
  */
  ~InterruptibleThread ();

  /**
    Start the thread.
  */
  void start (Function <void (void)> const& f);

  /**
    Wait for the thread to exit.
  */
  void join ();

  /**
    Wait for interrupt or timeout.

    This call blocks until the thread is interrupted, or until the timeout
    expires if milliSeconds is non-negative.

    May only be called by the thread of execution.

    @param milliSeconds The amount of time to wait.
                        Negative values mean no timeout.
                        
    @return \c true if the interrupt occurred, or \c false if the timeout expired.
  */
  bool wait (int milliSeconds = -1);

  /**
    Interrupt the thread of execution.

    This can be called from any thread.
  */
  void interrupt ();

  /**
    Determine if an interruption is requested.

    After the function returns \c true, the interrupt status is cleared.
    Subsequent calls will return \c false until another interrupt is requested.

    May only be called by the thread of execution.

    @return \c true if an interrupt was requested.
  */
  Interrupted interruptionPoint ();

  /**
    Get the ID of the associated thread.

    @return The ID of the thread.
  */
  id getId () const;

  /**
    Determine if this is the thread of execution.

    @note The return value is undefined if the thread is not running.

    @return \c true if the caller is this thread of execution.
  */
  bool isTheCurrentThread () const;

  /**
    Adjust the thread priority.

    @note This only affects some platforms.

    @param priority A number from 0..10
  */
  void setPriority (int priority);

private:
  void run ();

  Function <void (void)> m_function;
  WaitableEvent m_runEvent;
  id m_threadId;

  enum
  {
    stateRun,
    stateInterrupt,
    stateReturn,
    stateWait
  };

  AtomicState m_state;
};

//------------------------------------------------------------------------------

namespace CurrentInterruptibleThread
{

extern InterruptibleThread::Interrupted interruptionPoint ();

}

#endif
