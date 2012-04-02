// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

InterruptibleThread::InterruptibleThread (String name)
  : Thread (name)
  , m_state (stateRun)
{
}

InterruptibleThread::~InterruptibleThread ()
{
  m_runEvent.signal ();

  join ();
}

void InterruptibleThread::start (const Function <void (void)>& f)
{
  m_function = f;

  VF_JUCE::Thread::startThread ();

  // prevents data race with member variables
  m_runEvent.signal ();
}

void InterruptibleThread::join ()
{
  VF_JUCE::Thread::stopThread (-1);
}

bool InterruptibleThread::wait (int milliSeconds)
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

void InterruptibleThread::interrupt ()
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

InterruptibleThread::Interrupted InterruptibleThread::interruptionPoint ()
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

  return InterruptibleThread::Interrupted (interrupted);
}

InterruptibleThread::id InterruptibleThread::getId () const
{
  return m_threadId;
}

bool InterruptibleThread::isTheCurrentThread () const
{
  return VF_JUCE::Thread::getCurrentThreadId () == m_threadId;
}

void InterruptibleThread::setPriority (int priority)
{
  VF_JUCE::Thread::setPriority (priority);
}

void InterruptibleThread::run ()
{
  m_threadId = VF_JUCE::Thread::getThreadId ();

  m_runEvent.wait ();

  CatchAny (m_function);
}

//------------------------------------------------------------------------------

namespace CurrentInterruptibleThread
{

InterruptibleThread::Interrupted interruptionPoint ()
{
  bool interrupted;

  VF_JUCE::Thread* thread = VF_JUCE::Thread::getCurrentThread();

  // Can't use interruption points on the message thread
  vfassert (thread != 0);
  
  if (thread)
  {
    InterruptibleThread* interruptibleThread = dynamic_cast <InterruptibleThread*> (thread);

    vfassert (interruptibleThread != nullptr);

    if (interruptibleThread != nullptr)
      interrupted = interruptibleThread->interruptionPoint ();
    else
      interrupted = false;
  }
  else
  {
    interrupted = false;
  }

  return InterruptibleThread::Interrupted (interrupted);
}

}
