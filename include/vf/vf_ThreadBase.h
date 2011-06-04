// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADBASE_VFHEADER__
#define __VF_THREADBASE_VFHEADER__

#include "vf/vf_SafeBool.h"

class ThreadBase
{
public:
  // Used to throw generic thread interruptions
  // when using the exception interruption model.
  typedef boost::thread_interrupted Interruption;

  // This is the flag used to indicate if an interruption
  // occurred when using the polling model. It is designed
  // to detect improper usage (specifically, not checking
  // the flag, which would result in incorrect behavior).
  //
  // #1 The default constructor must produce an object that
  //    is considered non-signaled (i.e. not interrupted)
  //    in order for ThreadWorker that uses a default Callable
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

public:
  // Blocks until an interrupt occurs or the timeout expires.
  // If milliseconds is less than 0, the wait is infinite.
  // May only be called by the thread of execution.
  // Returns true if the interrupt occurred, or false if
  // the timeout expired.
  //
  virtual bool wait (int milliseconds = -1) = 0;

  // Interrupts the thread.
  //
  virtual void interrupt () = 0;

  // Called by the thread function, either throws
  // an Interruption exception, or returns an Interrupted
  // object indicating whether or not interruption is requested.
  //
  virtual Interrupted interruptionPoint () = 0;

  // Returns true if the caller is this thread of execution.
  // Only valid if the thread has been started.
  //
  virtual bool isTheCurrentThread () const = 0;

  // Adjusts the priority, range is 0...10.
  // Only available in some implementations.
  //
  virtual void setPriority (int priority) = 0;
};

#endif
