// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

JuceThread::JuceThread (String name)
  : JuceThreadWrapper (name, *this)
  , m_state (stateRun)
{
}

JuceThread::~JuceThread ()
{
  m_runEvent.signal ();

  join ();
}

void JuceThread::start (const Function <void (void)>& f)
{
  m_function = f;

  VF_JUCE::Thread::startThread ();

  // prevents data race with member variables
  m_runEvent.signal ();
}

void JuceThread::join ()
{
  VF_JUCE::Thread::stopThread (-1);
}

bool JuceThread::wait (int milliSeconds)
{
  // Can only be called from the current thread
  vfassert (isTheCurrentThread ());

  bool interrupted;

  for (;;)
  {
    vfassert (m_state != stateWait);

    // See if we are interrupted
    if (m_state.tryChangeState (stateInterrupt, stateRun))
    {
      // We were interrupted, state is changed to Run.
      // Caller must run now.
      interrupted = true;
      break;
    }
    else if (m_state.tryChangeState (stateRun, stateWait) ||
             m_state.tryChangeState (stateReturn, stateWait))
    {
      // Transitioned to wait.
      // Caller must wait now.
      interrupted = false;
      break;
    }
  }

  if (!interrupted)
  {
    interrupted = Thread::wait (milliSeconds);

    if (!interrupted)
	{
	  if (m_state.tryChangeState (stateWait, stateRun))
	  {
		interrupted = false;
	  }
	  else
	  {
		vfassert (m_state == stateInterrupt);

		interrupted = true;
	  }
	}
  }

  return interrupted;
}

void JuceThread::interrupt ()
{
  for (;;)
  {
    int const state = m_state;

    if (state == stateInterrupt ||
        state == stateReturn ||
        m_state.tryChangeState (stateRun, stateInterrupt))
    {
      // Thread will see this at next interruption point.
      break;
    }
    else if (m_state.tryChangeState (stateWait, stateRun))
    {
      VF_JUCE::Thread::notify ();
      break;
    }
  }
}

JuceThread::Interrupted JuceThread::interruptionPoint ()
{
  // Can only be called from the current thread
  vfassert (isTheCurrentThread ());

  if (m_state == stateWait)
  {
    // It is impossible for this function
    // to be called while in the wait state.
    Throw (Error().fail (__FILE__, __LINE__));
  }
  else if (m_state == stateReturn)
  {
    // If this goes off it means the thread called the
    // interruption a second time after already getting interrupted.
    Throw (Error().fail (__FILE__, __LINE__));
  }

  // Switch to Return state if we are interrupted
  //bool const interrupted = m_state.tryChangeState (stateInterrupt, stateReturn);
  bool const interrupted = m_state.tryChangeState (stateInterrupt, stateRun);

  return JuceThread::Interrupted (interrupted);
}

JuceThread::id JuceThread::getId () const
{
  return m_threadId;
}

bool JuceThread::isTheCurrentThread () const
{
  return VF_JUCE::Thread::getCurrentThreadId () == m_threadId;
}

void JuceThread::setPriority (int priority)
{
  VF_JUCE::Thread::setPriority (priority);
}

void JuceThread::run ()
{
  m_threadId = VF_JUCE::Thread::getThreadId ();

  m_runEvent.wait ();

  CatchAny (m_function);
}

//------------------------------------------------------------------------------

namespace CurrentJuceThread {

JuceThread::Interrupted interruptionPoint ()
{
  bool interrupted;

  VF_JUCE::Thread* thread = VF_JUCE::Thread::getCurrentThread();

  // Can't use interruption points on the message thread
  vfassert (thread != 0);
  
  if (thread)
  {
    detail::JuceThreadWrapper* threadWrapper = dynamic_cast <detail::JuceThreadWrapper*> (thread);

    vfassert (threadWrapper != 0);

    if (threadWrapper)
      interrupted = threadWrapper->getJuceThread().interruptionPoint ();
    else
      interrupted = false;
  }
  else
  {
    interrupted = false;
  }

  return JuceThread::Interrupted (interrupted);
}

}
