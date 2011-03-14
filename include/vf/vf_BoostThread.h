// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BOOSTTHREAD_VFHEADER__
#define __VF_BOOSTTHREAD_VFHEADER__

#if VF_HAVE_BOOST

#include "vf/vf_Function.h"
#include "vf/vf_String.h"
#include "vf/vf_Thread.h"

//
// Implementation of vf::Thread using Boost
//

namespace Boost {

class Thread
{
public:
  typedef boost::thread::id id;

  typedef detail::Thread::Interruption Interruption;

  explicit Thread (const VF_NAMESPACE::String& name);
  ~Thread ();

  template <class Callable>
  void start (const Callable& c)
  {
    m_thread = boost::thread (c);
  }

  void join ();

  id getId ();

  // only valid if the thread is running
  bool isTheCurrentThread () const;

  // unavailable in boost
  inline void setPriority (int) { }

  void wait ();

  void interrupt ();

  bool interruptionPoint ();

private:
  boost::thread m_thread;
};

namespace CurrentThread {

extern Thread::id getId ();

// Avoid using this routine.
// Use Boost::Thread::interruptionPoint() instead.
extern bool interruptionPoint ();

// Boost has no equivalent, and pthreads doesn't support it.
inline void setPriority (int) { }

}

}

#endif

#endif
