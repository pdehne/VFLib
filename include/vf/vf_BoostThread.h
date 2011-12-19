// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BOOSTTHREAD_VFHEADER__
#define __VF_BOOSTTHREAD_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_Function.h"
#include "vf/vf_ThreadBase.h"

//
// Implementation of Thread using Boost
//

class BoostThread : public ThreadBase
{
public:
  typedef boost::thread::id id;

public:
  class ExceptionBased
  {
  public:
    bool wait (int milliseconds, BoostThread& thread);
    void interrupt (BoostThread& thread);
    Interrupted interruptionPoint (BoostThread& thread);
  };

  class PollingBased
  {
  public:
    PollingBased ();
    bool wait (int milliseconds, BoostThread& thread);
    void interrupt (BoostThread& thread);
    Interrupted interruptionPoint (BoostThread& thread);
  
  private:
    enum
    {
      stateRun,
      stateInterrupt,
      stateReturn,
      stateWait
    };

    int m_state;
    boost::mutex m_mutex;
    boost::condition_variable m_cond;
  };

public:
  explicit BoostThread (String name);

  void start (Function <void (void)> const& f);

  void join ();

  id getId ();

  bool isTheCurrentThread () const;

  inline void setPriority (int) { }

private:
  void run (Function <void (void)> f);

private:
  String const m_name;
  boost::thread m_thread;
  VF_JUCE::WaitableEvent m_runEvent;
};

//------------------------------------------------------------------------------

template <class InterruptionType>
class BoostThreadType : public BoostThread
{
public:
  explicit BoostThreadType (String name) : BoostThread (name)
  {
  }

  inline bool wait (int milliseconds = -1)
  {
    return m_model.wait (milliseconds, *this);
  }

  inline void interrupt ()
  {
    m_model.interrupt (*this);
  }

  inline Interrupted interruptionPoint ()
  {
    return m_model.interruptionPoint (*this);
  }

private:
  InterruptionType m_model;
};

//------------------------------------------------------------------------------

namespace CurrentBoostThread {

// Avoid using this routine.
// Use BoostThread::interruptionPoint() instead.
//
extern ThreadBase::Interrupted interruptionPoint ();

extern BoostThread::id getId ();

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
  if (milliseconds >= 0)
  {
    // bit of a hack but nice to avoid all the boost ptime/reltime nonsense
      boost::this_thread::sleep (boost::posix_time::milliseconds(milliseconds));
  }
  else
  {
    // sleep forever or until interruption point
    boost::this_thread::sleep (
      boost::posix_time::ptime (
        boost::date_time::max_date_time));
  }
}

}

#endif
