// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCETHREAD_VFHEADER__
#define __VF_JUCETHREAD_VFHEADER__

//
// Implementation of vf::Thread using Juce
//

#if VF_HAVE_JUCE

namespace Juce {

class Thread : public VF_JUCE::Thread
{
public:
  typedef VF_JUCE::Thread::ThreadID id;

  typedef detail::Thread::Interruption Interruption;

  explicit Thread (const VF_NAMESPACE::String& name)
    : VF_JUCE::Thread (name)
    , m_interrupted (false)
  {
  }

  ~Thread ()
  {
    interrupt ();
    join ();
  }

  template <class Callable>
  void start (const Callable& c)
  {
    m_callable = c;
    VF_JUCE::Thread::startThread ();
  }

  void join ()
  {
    VF_JUCE::Thread::stopThread (-1);
  }

  id getId () const
  {
    return VF_JUCE::Thread::getThreadId ();
  }

  // only valid if the thread is running
  bool isTheCurrentThread () const
  {
    return VF_JUCE::Thread::getCurrentThreadId () ==
           VF_JUCE::Thread::getThreadId ();
  }

  inline void setPriority (int priority)
  {
    VF_JUCE::Thread::setPriority (priority);
  }

  // TODO: CONDITION VARIABLE TO FIX THIS
  // this is not thread safe, and the caller must synchronize
  void wait ()
  {
    if (!m_interrupted)
      VF_JUCE::Thread::wait (-1);
    m_interrupted = false;
  }

  // this is not thread safe, and the caller must synchronize
  void interrupt ()
  {
    m_interrupted = true;
    VF_JUCE::Thread::notify ();
  }

  bool interruptionPoint ()
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

private:
  void run ()
  {
    CatchAny (m_callable);
  }

  volatile bool m_interrupted; // caller must synchronize!
  Function m_callable;
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return VF_JUCE::Thread::getCurrentThreadId ();
}

// Avoid this function because the implementation is slow.
// Use Juce::Thread::interruptionPoint() instead.
inline bool interruptionPoint ()
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

inline void setPriority (int priority) // [0, 10] where 5 = normal
{
  VF_JUCE::Thread::setCurrentThreadPriority (priority);
}

}

}

#endif

#endif

