// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BOOSTTHREAD_VFHEADER__
#define __VF_BOOSTTHREAD_VFHEADER__

#if VF_HAVE_BOOST

#include "vf/vf_Function.h"
#include "vf/vf_String.h"
#include "vf/vf_ThreadBase.h"

//
// Implementation of vf::Thread using Boost
//

namespace Boost {

class Thread : public detail::ThreadBase
{
public:
  typedef boost::thread::id id;

public:
  explicit Thread (const VF_NAMESPACE::String& name);
  ~Thread ();

  void start (const Function <void (void)>& f);

  void join ();

  id getId ();

  // only valid if the thread is running
  bool isTheCurrentThread () const;

  // unavailable in boost
  inline void setPriority (int) { }

  void wait ();

  void interrupt ();

  Interrupted interruptionPoint ();

private:
  boost::thread m_thread;
};

namespace CurrentThread {

extern Thread::id getId ();

// Avoid using this routine.
// Use Boost::Thread::interruptionPoint() instead.
extern Thread::Interrupted interruptionPoint ();

// Boost has no equivalent, and pthreads doesn't support it.
inline void setPriority (int)
{
}

inline void yield ()
{
  boost::this_thread::yield();
}

inline void sleep (const int milliseconds)
{
  // bit of a hack but nice to avoid all the boost ptime/reltime nonsense
  boost::this_thread::interruptible_wait (milliseconds);
}

}

}

#endif

#endif
