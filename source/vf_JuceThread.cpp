// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_JUCE

#include "vf/vf_CatchAny.h"
#include "vf/vf_JuceThread.h"

namespace Juce {

Thread::ExceptionBased::ExceptionBased ()
  : m_waiting (false)
  , m_interrupt (false)
{
}

void Thread::ExceptionBased::wait (Thread& thread)
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
    thread.wait (-1);
  else
    throw detail::Thread::Interruption();
}

void Thread::ExceptionBased::interrupt (Thread& thread)
{
  {
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
  }
}

Thread::Interrupted Thread::ExceptionBased::interruptionPoint (Thread& thread)
{
  bool do_interrupt;

  // TODO: USE ATOMIC
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

//------------------------------------------------------------------------------

Thread::PollingBased::PollingBased ()
  : m_waiting (false)
  , m_interrupt (false)
{
}

void Thread::PollingBased::wait (Thread& thread)
{
  bool do_wait;

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
}

void Thread::PollingBased::interrupt (Thread& thread)
{
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
}

Thread::Interrupted Thread::PollingBased::interruptionPoint (Thread& thread)
{
  bool interrupted;

  {
    ScopedLock lock (m_mutex);

    // TODO: USE ATOMIC
    fatal_vfassert (!m_waiting);
    fatal_vfassert (thread.isTheCurrentThread ());

    interrupted = m_interrupt;

    m_interrupt = false;
  }

  return interrupted;
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

  return interrupted;
}

void setPriority (int priority) // [0, 10] where 5 = normal
{
  VF_JUCE::Thread::setCurrentThreadPriority (priority);
}

}

}

#endif

END_VF_NAMESPACE
