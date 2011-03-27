// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_JUCE

#include "vf/vf_CatchAny.h"
#include "vf/vf_JuceThread.h"

namespace Juce {

Thread::InterruptionModel::InterruptionModel ()
  : m_state (stateReset)
{
}

bool Thread::InterruptionModel::do_wait ()
{
  bool should_wait;

  for (;;)
  {
    const int state = m_state.get ();

    vfassert (state != stateWaiting);

    if (state == stateSignaled)
    {
      should_wait = false;

      m_state.set (stateReset);

      break;
    }
    else if (m_state.compareAndSetBool (stateWaiting, stateReset))
    {
      should_wait = true;

      break;
    }
  }

  return should_wait;
}

void Thread::InterruptionModel::interrupt (Thread& thread)
{
  bool should_signal;

  for (;;)
  {
    const int state = m_state.get ();

    if (state == stateSignaled ||
        m_state.compareAndSetBool (stateSignaled, stateReset))
    {
      should_signal = false;

      break;
    }
    else if (m_state.compareAndSetBool (stateReset, stateWaiting))
    {
      should_signal = true;

      break;
    }
  }

  if (should_signal)
    thread.notify ();
}

bool Thread::InterruptionModel::do_interruptionPoint ()
{
  // How could we possibly be in the wait state and get called?
  vfassert (m_state.get() != stateWaiting);

  // If we are in the signaled state, switch to reset, and interrupt.
  // Only one thread will "win" this contest.
  const bool interrupted = m_state.compareAndSetBool (stateReset, stateSignaled);

  return interrupted;
}

//------------------------------------------------------------------------------

void Thread::ExceptionBased::wait (Thread& thread)
{
  const bool should_wait = do_wait ();

  if (should_wait)
    thread.wait (-1);
  else
    throw detail::Thread::Interruption();
}

Thread::Interrupted Thread::ExceptionBased::interruptionPoint (Thread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  const bool interrupted = do_interruptionPoint ();

  if (interrupted)
    throw detail::Thread::Interruption();

  return Thread::Interrupted (false);
}

//------------------------------------------------------------------------------

void Thread::PollingBased::wait (Thread& thread)
{
  const bool should_wait = do_wait ();

  if (should_wait)
    thread.wait (-1);
}

Thread::Interrupted Thread::PollingBased::interruptionPoint (Thread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  const bool interrupted = do_interruptionPoint ();

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
  CatchAny (m_function);
}

//------------------------------------------------------------------------------

namespace CurrentThread {

Juce::Thread::Interrupted interruptionPoint ()
{
  bool interrupted;

  VF_JUCE::Thread* thread = VF_JUCE::Thread::getCurrentThread();

  // Can't use interruption points on the message thread
  vfassert (thread != 0);
  
  if (thread)
  {
    detail::ThreadBase* threadBase = dynamic_cast <detail::ThreadBase*> (thread);

    // Can only use interruption points from one of our threads
    vfassert (threadBase != 0);

    if (threadBase)
      interrupted = threadBase->interruptionPoint ();
    else
      interrupted = false;
  }
  else
  {
    interrupted = false;
  }

  return Thread::Interrupted (interrupted);
}

}

}

#endif

END_VF_NAMESPACE
