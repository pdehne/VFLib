// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#define USE_DEBUG_MUTEX 0

#if USE_DEBUG_MUTEX

ReadWriteMutex::ReadWriteMutex () { }
ReadWriteMutex::~ReadWriteMutex () { }
void ReadWriteMutex::enter_read () const  { m_mutex->enter (); }
void ReadWriteMutex::exit_read () const   { m_mutex->exit (); }
void ReadWriteMutex::enter_write () const { m_mutex->enter (); }
void ReadWriteMutex::exit_write () const  { m_mutex->exit (); }
void ReadWriteMutex::upgrade_write () const { }
void ReadWriteMutex::downgrade_read () const { }

#else

ReadWriteMutex::ReadWriteMutex ()
{
}

ReadWriteMutex::~ReadWriteMutex ()
{
}

void ReadWriteMutex::enter_read () const
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
        Mutex::ScopedLockType lock (*m_mutex);
      }

      // now try the loop again
    }
    else
    {
      break;
    }
  }
}

void ReadWriteMutex::exit_read () const
{
  m_readers->release ();
}

void ReadWriteMutex::enter_write () const
{
  // Optimistically acquire the write lock.
  m_writes->addref ();

  // Go for the mutex.
  // Another writer might block us here.
  m_mutex->enter ();

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

void ReadWriteMutex::exit_write () const
{
  // Releasing the mutex first and then decrementing the
  // writer count allows another waiting writer to atomically
  // acquire the lock, thus starving readers. This fulfills
  // the write-preferencing requirement.

  m_mutex->exit ();

  m_writes->release ();
}

#if 0
void ReadWriteMutex::upgrade_write () const
{
  // Caller must have exactly one read lock

  m_writes->addref ();

  m_mutex->enter ();

  // Release our read lock
  m_readers->release ();

  if (m_readers->is_signaled ())
  {
    Delay delay; 
    do
    {
      delay.spin ();
    }
    while (m_readers->is_signaled ());
  }
}

void ReadWriteMutex::downgrade_read () const
{
  // Caller must have exactly one write lock

  m_readers->addref ();

  m_mutex->exit ();

  m_writes->release ();
}
#endif

#endif
