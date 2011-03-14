// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCETHREAD_VFHEADER__
#define __VF_JUCETHREAD_VFHEADER__

#if VF_HAVE_JUCE

#include "vf/vf_Function.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Thread.h"
#include "vf/vf_String.h"

//
// Implementation of vf::Thread using Juce
//

namespace Juce {

class Thread : public VF_JUCE::Thread
{
public:
  //
  // Interruption models
  //
  class ExceptionBased
  {
  public:
    explicit ExceptionBased (Thread& thread)
      : m_thread (thread)
      , m_waiting (false)
      , m_interrupt (false)
    {
    }

    ~ExceptionBased ()
    {
    }

    void wait ()
    {
      bool do_wait;

      {
        ScopedLock lock (m_mutex);

        fatal_vfassert (!m_waiting);

        if (m_interrupt)
        {
          m_interrupt = false;
          do_wait = false;
        }
        else
        {
          m_waiting = true;
          do_wait = true;
        }
      }

      if (do_wait)
        m_thread.wait ();
      else
        throw detail::Thread::Interruption();
    }

    void interrupt ()
    {
      {
        ScopedLock lock (m_mutex);

        if (m_waiting)
        {
          m_waiting = false;

          m_thread.notify ();
        }
        else
        {
          m_interrupt = true;
        }
      }
    }

    bool interruptionPoint ()
    {
      bool do_interrupt;

      {
        ScopedLock lock (m_mutex);

        fatal_vfassert (!m_waiting);

        do_interrupt = m_interrupt;

        m_interrupt = false;
      }

      if (do_interrupt)
        throw detail::Thread::Interruption();

      return false;
    }

  private:
    Thread& m_thread;
    Mutex m_mutex;
    bool m_waiting;
    bool m_interrupt;
  };

  class PollingBased
  {
  public:
    explicit PollingBased (Thread& thread)
      : m_thread (thread)
      , m_waiting (false)
      , m_interrupt (false)
    {
    }

    ~PollingBased ()
    {
    }

    void wait ()
    {
      bool do_wait;

      {
        ScopedLock lock (m_mutex);

        fatal_vfassert (!m_waiting);
        
        do_wait = !m_interrupt;

        if (m_interrupt)
        {
          do_wait = false;
          m_interrupt = false;
        }
        else
        {
          do_wait = true;
          m_waiting = true;
        }
      }

      if (do_wait)
        m_thread.wait ();
    }

    void interrupt ()
    {
      {
        ScopedLock lock (m_mutex);

        if (m_waiting)
        {
          fatal_vfassert (!m_interrupt);

          m_waiting = false;

          m_thread.notify ();
        }
        else
        {
          m_interrupt = true;
        }
      }
    }

    bool interruptionPoint ()
    {
      bool do_interrupt;

      {
        ScopedLock lock (m_mutex);

        fatal_vfassert (!m_waiting);

        do_interrupt = m_interrupt;

        if (do_interrupt)
          m_interrupt = false;
      }

      m_mutex.exit ();

      if (do_interrupt)
        throw detail::Thread::Interruption();

      return false;
    }

  private:
    Thread& m_thread;
    Mutex m_mutex;
    bool m_waiting;
    bool m_interrupt;
  };

public:
  typedef VF_JUCE::Thread::ThreadID id;

  typedef detail::Thread::Interruption Interruption;

  explicit Thread (const VF_NAMESPACE::String& name);
  ~Thread ();

  template <class Callable>
  void start (const Callable& c)
  {
    m_callable = c;
    VF_JUCE::Thread::startThread ();
  }

  void join ();

  id getId () const;

  // only valid if the thread is running
  bool isTheCurrentThread () const;

  void setPriority (int priority);

  void wait ();

  void interrupt ();

  bool interruptionPoint ();

private:
  void run ();

  volatile bool m_interrupted; // caller must synchronize!
  Function m_callable;
};

namespace CurrentThread {

extern Thread::id getId ();

// Avoid this function because the implementation is slow.
// Use Juce::Thread::interruptionPoint() instead.
extern bool interruptionPoint ();

extern void setPriority (int priority);

}

}

#endif

#endif

