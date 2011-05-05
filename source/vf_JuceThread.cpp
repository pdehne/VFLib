// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_CatchAny.h"
#include "vf/vf_JuceThread.h"

JuceThread::InterruptionModel::InterruptionModel ()
  : m_state (stateRun)
{
}

// Attempt to transition into the wait state.
// Returns true if we should interrupt instead.
//
bool JuceThread::InterruptionModel::do_wait ()
{
  bool interrupted;

  for (;;)
  {
    vfassert (m_state != stateWait);

    // See if we are interrupted
    if (m_state.tryChangeState (stateInterrupt, stateRun))
    {
      // We were interrupted, state is changed to Run.
      // Caller must run now.
      interrupted = true;
      break;
    }
    else if (m_state.tryChangeState (stateRun, stateWait))
    {
      // Transitioned from run to wait.
      // Caller must wait now.
      interrupted = false;
      break;
    }
  }

  return interrupted;
}

//
bool JuceThread::InterruptionModel::do_timeout ()
{
  bool interrupted;

  if (m_state.tryChangeState (stateWait, stateRun))
  {
    interrupted = false;
  }
  else
  {
    vfassert (m_state == stateInterrupt);

    interrupted = true;
  }

  return interrupted;
}

void JuceThread::InterruptionModel::interrupt (JuceThread& thread)
{
  for (;;)
  {
    const int state = m_state;

    // If we are already in the interrupt state, or if
    // we successfully transition from reset to interrupt,
    // then there is no need to signal because the thread
    // will see the new state.
    //
    if (state == stateInterrupt ||
        m_state.tryChangeState (stateRun, stateInterrupt))
    {
      break;
    }
    // If we are in the waiting state, then try to change to
    // reset. Whoever is successful will wake the thread up.
    //
    else if (m_state.tryChangeState (stateWait, stateRun))
    {
      thread.notify ();
      break;
    }
  }
}

bool JuceThread::InterruptionModel::do_interruptionPoint ()
{
  // How could we possibly be in the wait state and get called?
  vfassert (m_state != stateWait);

  // If we are interrupted, switch to run.
  // Only one thread will "win" this contest.
  const bool interrupted = m_state.tryChangeState (stateInterrupt, stateRun);

  return interrupted;
}

//------------------------------------------------------------------------------

bool JuceThread::PollingBased::wait (int milliseconds, JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted = do_wait ();

  if (!interrupted)
  {
    interrupted = thread.Thread::wait (milliseconds);

    if (!interrupted)
      interrupted = do_timeout ();
  }

  return interrupted;
}

ThreadBase::Interrupted JuceThread::PollingBased::interruptionPoint (JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  const bool interrupted = do_interruptionPoint ();

  return ThreadBase::Interrupted (interrupted);
}

//------------------------------------------------------------------------------

bool JuceThread::ExceptionBased::wait (int milliseconds, JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  bool interrupted = do_wait ();

  if (!interrupted)
  {
    interrupted = thread.VF_JUCE::Thread::wait (milliseconds);

    if (!interrupted)
      interrupted = do_timeout ();
  }

  if (interrupted)
    throw Interruption();

  return interrupted;
}

ThreadBase::Interrupted JuceThread::ExceptionBased::interruptionPoint (JuceThread& thread)
{
  // Can only be called from the current thread
  vfassert (thread.isTheCurrentThread ());

  const bool interrupted = do_interruptionPoint ();

  if (interrupted)
    throw Interruption();

  return ThreadBase::Interrupted (false);
}

//------------------------------------------------------------------------------

JuceThread::JuceThread (String name)
  : JuceThreadWrapper (name, *this)
{
}

JuceThread::~JuceThread ()
{
  join ();
}

void JuceThread::start (const Function <void (void)>& f)
{
  m_function = f;
  
  VF_JUCE::Thread::startThread ();
}

void JuceThread::join ()
{
  VF_JUCE::Thread::stopThread (-1);
}

JuceThread::id JuceThread::getId () const
{
  return VF_JUCE::Thread::getThreadId ();
}

bool JuceThread::isTheCurrentThread () const
{
  return VF_JUCE::Thread::getCurrentThreadId () ==
         VF_JUCE::Thread::getThreadId ();
}

void JuceThread::setPriority (int priority)
{
  VF_JUCE::Thread::setPriority (priority);
}

void JuceThread::run ()
{
  CatchAny (m_function);
}

//------------------------------------------------------------------------------

namespace CurrentJuceThread {

ThreadBase::Interrupted interruptionPoint ()
{
  bool interrupted;

  VF_JUCE::Thread* thread = VF_JUCE::Thread::getCurrentThread();

  // Can't use interruption points on the message thread
  vfassert (thread != 0);
  
  if (thread)
  {
    detail::JuceThreadWrapper* threadWrapper = dynamic_cast <detail::JuceThreadWrapper*> (thread);

    vfassert (threadWrapper != 0);

    if (threadWrapper)
      interrupted = threadWrapper->getThreadBase().interruptionPoint ();
    else
      interrupted = false;
  }
  else
  {
    interrupted = false;
  }

  return ThreadBase::Interrupted (interrupted);
}

}

END_VF_NAMESPACE
