// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BOOSTTHREAD_VFHEADER__
#define __VF_BOOSTTHREAD_VFHEADER__

#include "vf/vf_Thread.h"

//
// Implementation of vf::Thread using Boost
//

#if VF_HAVE_BOOST

namespace Boost {

class Thread
{
public:
  typedef boost::thread::id id;

  typedef detail::Thread::Interruption Interruption;

  explicit Thread (const VF_NAMESPACE::String& name)
  {
  }

  ~Thread ()
  {
    join ();
  }

  template <class Callable>
  void start (const Callable& c)
  {
    m_thread = boost::thread (c);
  }

  void join ()
  {
    m_thread.join ();
  }

  id getId ()
  {
    return m_thread.get_id ();
  }

  // only valid if the thread is running
  bool isTheCurrentThread () const
  {
    return m_thread.get_id () == boost::this_thread::get_id ();
  }

  void setPriority (int)
  {
    // Unavailable in boost or pthreads.
  }

  void wait ()
  {
    try
    {
      // sleep until interrupted
      boost::this_thread::sleep (
        boost::posix_time::ptime (
          boost::date_time::max_date_time));
    }
    catch (boost::thread_interrupted&)
    {
      // wake up
    }
  }

  void interrupt ()
  {
    m_thread.interrupt ();
  }

  bool interruptionPoint ()
  {
    vfassert (isTheCurrentThread ());

    try
    {
      boost::this_thread::interruption_point ();
    }
    catch (boost::thread_interrupted&)
    {
      // re-throw it as a boost-independent object
      throw detail::Thread::Interruption();
    }

    return false;
  }

private:
  boost::thread m_thread;
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return boost::this_thread::get_id ();
}

// Avoid using this routine.
// Use Boost::Thread::interruptionPoint() instead.
inline bool interruptionPoint ()
{
  try
  {
    boost::this_thread::interruption_point ();
  }
  catch (boost::thread_interrupted&)
  {
    // re-throw it as a boost-independent object
    throw detail::Thread::Interruption();
  }

  return false;
}

inline void setPriority (int)
{
  // Boost has no equivalent, and pthreads doesn't support it.
}

}

}

#endif

#endif
