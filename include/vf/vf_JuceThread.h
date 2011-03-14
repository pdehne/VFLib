// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCETHREAD_VFHEADER__
#define __VF_JUCETHREAD_VFHEADER__

#if VF_HAVE_JUCE

#include "vf/vf_Function.h"
#include "vf/vf_Thread.h"
#include "vf/vf_String.h"

//
// Implementation of vf::Thread using Juce
//

namespace Juce {

class Thread : public VF_JUCE::Thread
{
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

