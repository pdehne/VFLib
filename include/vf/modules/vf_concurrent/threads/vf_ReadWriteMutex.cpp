// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

ReadWriteMutex::ReadWriteMutex () noexcept
{
}

ReadWriteMutex::~ReadWriteMutex () noexcept
{
}

void ReadWriteMutex::enterRead () const noexcept
{
  for (;;)
  {
    // attempt the lock optimistically
    // THIS IS NOT CACHE-FRIENDLY!
    m_readers->addref ();

    // is there a writer?
    // THIS IS NOT CACHE-FRIENDLY!
    if (m_writes->is_signaled ())
    {
      // a writer exists, give up the read lock
      m_readers->release ();

      // block until the writer is done
      {
		VF_JUCE::CriticalSection::ScopedLockType lock (m_mutex);
      }

      // now try the loop again
    }
    else
    {
      break;
    }
  }
}

void ReadWriteMutex::exitRead () const noexcept
{
  m_readers->release ();
}

void ReadWriteMutex::enterWrite () const noexcept
{
  // Optimistically acquire the write lock.
  m_writes->addref ();

  // Go for the mutex.
  // Another writer might block us here.
  m_mutex.enter ();

  // Only one competing writer will get here,
  // but we don't know who, so we have to drain
  // readers no matter what. New readers will be
  // blocked by the mutex.
  //
  // Crafted to sometimes avoid the Delay ctor.
  //
  if (m_readers->is_signaled ())
  {
    SpinDelay delay; 
    do
    {
      delay.pause ();
    }
    while (m_readers->is_signaled ());
  }
}

void ReadWriteMutex::exitWrite () const noexcept
{
  // Releasing the mutex first and then decrementing the
  // writer count allows another waiting writer to atomically
  // acquire the lock, thus starving readers. This fulfills
  // the write-preferencing requirement.

  m_mutex.exit ();

  m_writes->release ();
}
