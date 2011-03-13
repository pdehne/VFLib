// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREAD_VFHEADER__
#define __VF_THREAD_VFHEADER__

#include "vf/vf_Function.h"

namespace detail {

namespace Thread {

// Used to throw generic thread interruptions when this option is desired.
class Interruption
{
};

}

}

#if VF_HAVE_BOOST

namespace Boost {

class Thread
{
public:
  typedef boost::thread::id id;

  Thread ()
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

  void setPriority (int)
  {
    // Unavailable in boost or pthreads.
  }

  void interrupt ()
  {
    m_thread.interrupt ();
  }

  bool interruptionPoint ()
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

private:
  boost::thread m_thread;
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return boost::this_thread::get_id ();
}

// Replacement for boost::this_thread::interruption_point().
// Avoid this use of this function when possible, use
// Thread::interruptionPoint() instead.
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

#if VF_HAVE_JUCE

namespace Juce {

class Thread : private VF_JUCE::Thread
{
public:
  typedef VF_JUCE::Thread::ThreadID id;

  Thread () : VF_JUCE::Thread (""), m_interrupted (false)
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
    return m_interrupted;
  }

private:
  void run ()
  {
    m_callable ();
  }

  volatile bool m_interrupted; // caller must synchronize!
  Function m_callable;
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return VF_JUCE::Thread::getCurrentThreadId ();
}

// AVOID this function since it cannot be implemented in all
// available frameworks! Use Thread::interruptionPoint() instead.

inline bool interruptionPoint ()
{
  return false;
}

inline void setPriority (int priority) // [0, 10] where 5 = normal
{
  VF_JUCE::Thread::setCurrentThreadPriority (priority);
}

}

}

#endif

//------------------------------------------------------------------------------

#if VF_HAVE_BOOST

class Thread
{
public:
  typedef boost::thread::id id;

  // Use this instead of boost::thread_interrupted to
  // remove the application dependency on boost threads.
  typedef detail::Thread::Interruption Interruption;

  inline void setPriority (int)
  {
    // Unavailable in boost or pthreads.
  }
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return boost::this_thread::get_id ();
}

// Replacement for boost::this_thread::interruption_point().
// Avoid this use of this function when possible, use
// Thread::interruptionPoint() instead.
inline bool interruptionPoint ()
{
  try
  {
    boost::this_thread::interruption_point ();
  }
  catch (boost::thread_interrupted&)
  {
    // re-throw it as a boost-independent object
    throw Thread::Interruption();
  }

  return false;
}

}

#elif VF_HAVE_JUCE

class Thread
{
public:
  typedef VF_JUCE::Thread::ThreadID id;

  typedef detail::Thread::Interruption Interruption;

  inline void setPriority (int priority)
  {
    m_thread.setPriority (priority);
  }

public:
  Thread () : m_thread (*this) { }
  ~Thread () { m_thread.stopThread (-1); }

private:
  void run () { }

  class JuceThread : public VF_JUCE::Thread
  {
  public:
    JuceThread (VF_NAMESPACE::Thread& owner)
      : VF_JUCE::Thread (""), m_owner (owner) { }
    void run () { m_owner.run (); }
  private:
    VF_NAMESPACE::Thread& m_owner;
  };

  JuceThread m_thread;
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return VF_JUCE::Thread::getCurrentThreadId ();
}

// AVOID this function since it cannot be implemented in all
// available frameworks! Use Thread::interruptionPoint() instead.

inline bool interruptionPoint ()
{
  return false;
}

}

#else
  #pragma message(VF_LOC_"Missing Thread")

#endif

namespace CurrentThread {

#if VF_HAVE_JUCE

// [0, 10] where 5 = normal
inline void setPriority (int priority)
{
  VF_JUCE::Thread::setCurrentThreadPriority (priority);
}

#else

// Boost has no equivalent, and pthreads doesn't support it.
inline void setPriority (int)
{
  // Can't do it.
}

#endif

}

#endif

