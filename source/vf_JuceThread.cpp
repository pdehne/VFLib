// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_JUCE

#include "vf/vf_CatchAny.h"
#include "vf/vf_JuceThread.h"

#define LOCK_FREE 1

namespace Juce {

Thread::ExceptionBased::ExceptionBased ()
  : m_state (stateReset)
  , m_waiting (false)
  , m_interrupt (false)
{
}

void Thread::ExceptionBased::wait (Thread& thread)
{
  bool do_wait;

#if LOCK_FREE
  // Lock-free
  for (;;)
  {
    const int state = m_state.get ();

    vfassert (state != stateWaiting);

    if (state == stateSignaled)
    {
      do_wait = false;

      m_state.set (stateReset);

      break;
    }
    else if (m_state.compareAndSetBool (stateWaiting, stateReset))
    {
      do_wait = true;

      break;
    }
  }

#else
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
#endif

  if (do_wait)
    thread.wait (-1);
  else
    throw detail::Thread::Interruption();
}

void Thread::ExceptionBased::interrupt (Thread& thread)
{
#if LOCK_FREE
  bool do_signal;

  // Lock-free
  for (;;)
  {
    const int state = m_state.get ();

    if (state == stateSignaled ||
        m_state.compareAndSetBool (stateSignaled, stateReset))
    {
      do_signal = false;

      break;
    }
    else if (m_state.compareAndSetBool (stateReset, stateWaiting))
    {
      do_signal = true;

      break;
    }
  }

  if (do_signal)
    thread.notify ();

#else
  ScopedLock lock (m_mutex);

  // TODO: USE ATOMIC
  if (m_waiting)
  {
    m_waiting = false;

    thread.notify ();
  }
  else
  {
    m_interrupt = true;
  }

#endif
}

Thread::Interrupted Thread::ExceptionBased::interruptionPoint (Thread& thread)
{
  bool interrupted;

  //
  // Lock-free
  //
#if LOCK_FREE
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());
  // How could we possibly be in the wait state and get called?
  vfassert (m_state.get() != stateWaiting);

  // If we are in the signaled state, switch to reset, and interrupt.
  // Only one thread will "win" this contest.
  interrupted = m_state.compareAndSetBool (stateReset, stateSignaled);

#else
  // TODO: USE ATOMIC
  {
    ScopedLock lock (m_mutex);

    fatal_vfassert (!m_waiting);

    do_interrupt = m_interrupt;

    m_interrupt = false;
  }

#endif

  if (interrupted)
    throw detail::Thread::Interruption();

  return Thread::Interrupted (false);
}

//------------------------------------------------------------------------------

Thread::PollingBased::PollingBased ()
  : m_state (stateReset)
  , m_waiting (false)
  , m_interrupt (false)
{
}

void Thread::PollingBased::wait (Thread& thread)
{
  bool do_wait;

#if LOCK_FREE
  // Lock-free
  for (;;)
  {
    const int state = m_state.get ();

    vfassert (state != stateWaiting);

    if (state == stateSignaled)
    {
      do_wait = false;

      m_state.set (stateReset);

      break;
    }
    else if (m_state.compareAndSetBool (stateWaiting, stateReset))
    {
      do_wait = true;

      break;
    }
  }

  if (do_wait)
    thread.wait (-1);

#else
  // Wait-states
  {
    ScopedLock lock (m_mutex);

    fatal_vfassert (!m_waiting);
    
    if (m_interrupt)
    {
      do_wait = false;
      m_interrupt = false;
      m_waiting = false;
    }
    else
    {
      do_wait = true;
      m_waiting = true;
    }
  }

  if (do_wait)
  {
    thread.wait (-1);

    {
      ScopedLock lock (m_mutex);

      vfassert (!m_waiting);

      m_waiting = false;
    }
  }

#endif
}

void Thread::PollingBased::interrupt (Thread& thread)
{
#if LOCK_FREE
  bool do_signal;

  // Lock-free
  for (;;)
  {
    const int state = m_state.get ();

    if (state == stateSignaled ||
        m_state.compareAndSetBool (stateSignaled, stateReset))
    {
      do_signal = false;

      break;
    }
    else if (m_state.compareAndSetBool (stateReset, stateWaiting))
    {
      do_signal = true;

      break;
    }
  }

  if (do_signal)
    thread.notify ();

#else
  {
    ScopedLock lock (m_mutex);

    // TODO: USE ATOMIC
    if (m_waiting)
    {
      fatal_vfassert (!m_interrupt);

      m_waiting = false;

      thread.notify ();
    }
    else
    {
      m_interrupt = true;
    }
  }
#endif
}

Thread::Interrupted Thread::PollingBased::interruptionPoint (Thread& thread)
{
  bool interrupted;

  //
  // Lock-free
  //
#if LOCK_FREE
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());
  // How could we possibly be in the wait state and get called?
  vfassert (m_state.get() != stateWaiting);

  // If we are in the signaled state, switch to reset, and interrupt.
  // Only one thread will "win" this contest.
  interrupted = m_state.compareAndSetBool (stateReset, stateSignaled);

#else
  {
    ScopedLock lock (m_mutex);

    // TODO: USE ATOMIC
    fatal_vfassert (!m_waiting);
    fatal_vfassert (thread.isTheCurrentThread ());

    interrupted = m_interrupt;

    m_interrupt = false;
  }

#endif

  return Thread::Interrupted (interrupted);
}

//------------------------------------------------------------------------------

Thread::Thread (const VF_NAMESPACE::String& name) : VF_JUCE::Thread (name)
{
}

Thread::~Thread ()
{
  join ();
}

void Thread::join ()
{
  VF_JUCE::Thread::stopThread (-1);
}

Thread::id Thread::getId () const
{
  return VF_JUCE::Thread::getThreadId ();
}

bool Thread::isTheCurrentThread () const
{
  return VF_JUCE::Thread::getCurrentThreadId () ==
         VF_JUCE::Thread::getThreadId ();
}

void Thread::setPriority (int priority)
{
  VF_JUCE::Thread::setPriority (priority);
}

void Thread::run ()
{
  CatchAny (m_callable);
}

//------------------------------------------------------------------------------

namespace CurrentThread {

Juce::Thread::id getId ()
{
  return VF_JUCE::Thread::getCurrentThreadId ();
}

Juce::Thread::Interrupted interruptionPoint ()
{
  bool interrupted;

  VF_JUCE::Thread* thread = VF_JUCE::Thread::getCurrentThread();

  // Can't use interruption points on the message thread
  vfassert (thread != 0);
  
  if (thread)
  {
    Juce::Thread* vfThread = dynamic_cast <VF_NAMESPACE::Juce::Thread*> (thread);

    // Can only use interruption points from a Juce::Thread
    vfassert (vfThread != 0);

    if (vfThread)
      interrupted = vfThread->interruptionPoint ();
    else
      interrupted = false;
  }
  else
  {
    interrupted = false;
  }

  return Thread::Interrupted (interrupted);
}

void setPriority (int priority) // [0, 10] where 5 = normal
{
  VF_JUCE::Thread::setCurrentThreadPriority (priority);
}

}

}

#endif

END_VF_NAMESPACE
