// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_JUCE

#include "vf/vf_CatchAny.h"
#include "vf/vf_JuceThread.h"

namespace Juce {

Thread::Thread (const VF_NAMESPACE::String& name)
  : VF_JUCE::Thread (name)
  , m_interrupted (false)
{
}

Thread::~Thread ()
{
  interrupt ();
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

// TODO: CONDITION VARIABLE TO FIX THIS
// this is not thread safe, and the caller must synchronize
void Thread::wait ()
{
  vfassert (isTheCurrentThread ());

  if (!m_interrupted)
    VF_JUCE::Thread::wait (-1);
  m_interrupted = false;
}

void Thread::interrupt ()
{
  m_interrupted = true;
  VF_JUCE::Thread::notify ();
}

bool Thread::interruptionPoint ()
{
  vfassert (isTheCurrentThread ());

  // This needs to be atomic!
  if (m_interrupted)
  {
    m_interrupted = false;
    // Avoid using vf::Throw() here
    throw detail::Thread::Interruption(); 
  }

  return m_interrupted;
}

void Thread::run ()
{
  CatchAny (m_callable);
}

namespace CurrentThread {

Juce::Thread::id getId ()
{
  return VF_JUCE::Thread::getCurrentThreadId ();
}

bool interruptionPoint ()
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
