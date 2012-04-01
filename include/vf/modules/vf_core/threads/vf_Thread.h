// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_THREAD_VFHEADER
#define VF_THREAD_VFHEADER

#include "vf/modules/vf_core/functor/vf_Function.h"

#include "vf/modules/vf_core/diagnostic/vf_SafeBool.h"

class JuceThread;

namespace detail
{

// Stores a back pointer to our object so we can keep
// the derivation from juce::Thread private and still use
// dynamic_cast.
//
class JuceThreadWrapper : public VF_JUCE::Thread
{
public:
  JuceThreadWrapper (String name, JuceThread& juceThread)
    : VF_JUCE::Thread (name)
    , m_juceThread (juceThread)
  {
  }

  JuceThread& getJuceThread () const
  {
    return m_juceThread;
  }

private:
  JuceThread& m_juceThread;
};

}

// InterruptibleThread
class JuceThread : public detail::JuceThreadWrapper
{
public:
  // This is the flag used to indicate if an interruption
  // occurred when using the polling model. It is designed
  // to detect improper usage (specifically, not checking
  // the flag, which would result in incorrect behavior).
  //
  // #1 The default constructor must produce an object that
  //    is considered non-signaled (i.e. not interrupted)
  //    in order for ThreadWorker that uses a default Callable
  //    for its idle function to work.
  //
  class Interrupted : public SafeBool <Interrupted>
  {
  public:
    Interrupted ()
     : m_interrupted (false)
     , m_checked (false)
    {
    }

    explicit Interrupted (bool interrupted)
     : m_interrupted (interrupted)
     , m_checked (false)
    {
    }

    Interrupted (const Interrupted& other)
      : m_interrupted (other.m_interrupted)
      , m_checked (false)
    {
      other.m_checked = true;
    }

    ~Interrupted ()
    {
      vfassert (!m_interrupted || m_checked);
    }

    Interrupted& operator= (const Interrupted& other)
    {
      m_interrupted = other.m_interrupted;
      m_checked = false;
      other.m_checked = true;
      return *this;
    }

    bool asBoolean () const
    {
      m_checked = true;
      return m_interrupted;
    }

  private:
    bool m_interrupted;
    bool mutable m_checked;
  };

  typedef VF_JUCE::Thread::ThreadID id;

public:
  class PollingBased
  {
  public:
    PollingBased ();

    void interrupt (JuceThread& thread);
    bool wait (int milliseconds, JuceThread& thread);
    Interrupted interruptionPoint (JuceThread& thread);

  protected:
    bool do_wait ();    // true if interrupted
    bool do_timeout (); // true if interrupted
    bool do_interruptionPoint ();

  protected:
    enum
    {
      stateRun,
      stateInterrupt,
      stateReturn,
      stateWait
    };

    AtomicState m_state;
  };

public:
  explicit JuceThread (String name);
  ~JuceThread ();

  void start (Function <void (void)> const& f);

  // Blocks until an interrupt occurs or the timeout expires.
  // If milliseconds is less than 0, the wait is infinite.
  // May only be called by the thread of execution.
  // Returns true if the interrupt occurred, or false if
  // the timeout expired.
  //

  bool wait (int milliseconds = -1)
  {
    return m_model.wait (milliseconds, *this);
  }

  // Interrupts the thread.
  //
  void interrupt ()
  {
    m_model.interrupt (*this);
  }

  // Called by the thread function, returns an Interrupted
  // object indicating whether or not interruption is requested.
  //
  Interrupted interruptionPoint ()
  {
    return m_model.interruptionPoint (*this);
  }

  void join ();

  id getId () const;

  // Returns true if the caller is this thread of execution.
  // Only valid if the thread has been started.
  //
  bool isTheCurrentThread () const;

  // Adjusts the priority, range is 0...10.
  // Only available in some implementations.
  //
  void setPriority (int priority);

private:
  void run ();

  Function <void (void)> m_function;
  VF_JUCE::WaitableEvent m_runEvent;
  id m_threadId;

  PollingBased m_model;
};

//------------------------------------------------------------------------------

namespace CurrentJuceThread {

// Avoid this function because the implementation is slow.
// Use JuceThread::interruptionPoint() instead.
//
extern JuceThread::Interrupted interruptionPoint ();

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

typedef JuceThread Thread;
namespace CurrentThread = CurrentJuceThread;

#endif
