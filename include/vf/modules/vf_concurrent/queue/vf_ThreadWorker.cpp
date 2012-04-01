// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

ThreadWorker::ThreadWorker (String name)
  : Worker (name)
  , m_thread (name)
  , m_calledStart (false)
  , m_calledStop (false)
  , m_shouldStop (false)
{
}

ThreadWorker::~ThreadWorker ()
{
  stop_and_wait ();
}

void ThreadWorker::start (idle_t worker_idle,
						  init_t worker_init,
						  exit_t worker_exit)
{
  {
    // TODO: Atomic for this
	VF_JUCE::CriticalSection::ScopedLockType lock (m_mutex);

    // start() MUST be called.
    vfassert (!m_calledStart);
    m_calledStart = true;
  }

  m_init = worker_init;
  m_idle = worker_idle;
  m_exit = worker_exit;

  m_thread.start (vf::bind (&ThreadWorker::run, this));
}

void ThreadWorker::stop (bool const wait)
{
  // can't call stop(true) from within a thread function
  vfassert (!wait || !m_thread.isTheCurrentThread ());

  {
	VF_JUCE::CriticalSection::ScopedLockType lock (m_mutex);

    // start() MUST be called.
    vfassert (m_calledStart);

    // TODO: Atomic for this
    if (!m_calledStop)
    {
      m_calledStop = true;

      {
		VF_JUCE::CriticalSection::ScopedUnlockType unlock (m_mutex); // getting fancy

        call (&ThreadWorker::do_stop, this);

        // in theory something could slip in here

        close ();
      }
    }
  }

  if (wait)
    m_thread.join ();
}

// Should be called periodically by the idle function.
// There are three possible results:
//
// #1 Returns false. The idle function may continue or return.
// #2 Returns true. The idle function should return as soon as possible.
// #3 Throws a Thread::Interruption exception.
//
// If interruptionPoint returns true or throws, it must
// not be called again before the thread has the opportunity to reset.
//
const InterruptibleThread::Interrupted ThreadWorker::interruptionPoint ()
{
  return m_thread.interruptionPoint ();
}

// Interrupts the idle function by queueing a call that does nothing.
void ThreadWorker::interrupt ()
{
  call (Function <void (void)>::None ());
}

void ThreadWorker::reset ()
{
}

void ThreadWorker::signal ()
{
  m_thread.interrupt ();
}

void ThreadWorker::do_stop ()
{
  m_shouldStop = true;
}

void ThreadWorker::run ()
{
  m_init ();

  for (;;)
  {
    Worker::process ();

    if (m_shouldStop)
      break;

    // idle_t::None() must return a non signaled Thread::Interrupted.
    InterruptibleThread::Interrupted interrupted = m_idle ();

    if (!interrupted)
      interrupted = interruptionPoint ();

    if (!interrupted)
      m_thread.wait ();
  }

  m_exit ();
}
