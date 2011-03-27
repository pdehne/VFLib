// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCETHREAD_VFHEADER__
#define __VF_JUCETHREAD_VFHEADER__

#if VF_HAVE_JUCE

#include "vf/vf_Function.h"
#include "vf/vf_ThreadBase.h"

namespace detail {

// Stores a back pointer to our object so we can keep
// the derivation from juce::Thread private and still use
// dynamic_cast.
//
struct JuceThreadWrapper : public VF_JUCE::Thread
{
  JuceThreadWrapper (const String& threadName, ThreadBase* threadBase)
    : VF_JUCE::Thread (threadName), m_threadBase (threadBase) { }
  ThreadBase* getThreadBase () const { return m_threadBase; }
private:
  ThreadBase* m_threadBase;
};

}

//
// Thread based on Juce
//
class JuceThread : public ThreadBase
                 , private detail::JuceThreadWrapper
{
public:
  typedef VF_JUCE::Thread::ThreadID id;

public:
  class InterruptionModel
  {
  protected:
    InterruptionModel ();
    bool do_wait ();

  public:
    void interrupt (JuceThread& thread);

  protected:
    bool do_interruptionPoint ();

  protected:
    enum
    {
      stateReset = 0,
      stateSignaled = 1,
      stateWaiting = 2
    };

    VF_JUCE::Atomic <int> m_state;
  };

  class ExceptionBased : public InterruptionModel
  {
  public:
    void wait (JuceThread& thread);
    Interrupted interruptionPoint (JuceThread& thread);
  };

  class PollingBased : public InterruptionModel
  {
  public:
    void wait (JuceThread& thread);
    Interrupted interruptionPoint (JuceThread& thread);
  };

public:
  explicit JuceThread (const VF_NAMESPACE::String& name);
  ~JuceThread ();

  void start (const Function <void (void)>& f);

  void join ();

  id getId () const;

  // only valid if the thread is running
  bool isTheCurrentThread () const;

  void setPriority (int priority);

private:
  void run ();

  Function <void (void)> m_function;
};

template <class InterruptionType>
class JuceThreadType : public JuceThread
{
public:
  explicit JuceThreadType (const VF_NAMESPACE::String& name)
    : JuceThread (name)
  {
  }

  void wait ()
  {
    m_model.wait (*this);
  }

  void interrupt ()
  {
    m_model.interrupt (*this);
  }

  Interrupted interruptionPoint ()
  {
    return m_model.interruptionPoint (*this);
  }

private:
  InterruptionType m_model;
};

namespace CurrentJuceThread {

// Avoid this function because the implementation is slow.
// Use JuceThread::interruptionPoint() instead.
//
extern ThreadBase::Interrupted interruptionPoint ();

inline JuceThread::id getId ()
{
  return VF_JUCE::Thread::getCurrentThreadId ();
}

// [0, 10] where 5 = normal
inline void setPriority (int priority) 
{
  VF_JUCE::Thread::setCurrentThreadPriority (priority);
}

inline void yield ()
{
  VF_JUCE::Thread::yield ();
}

inline void sleep (const int milliseconds)
{
  VF_JUCE::Thread::sleep (milliseconds);
}

}

#endif

#endif

