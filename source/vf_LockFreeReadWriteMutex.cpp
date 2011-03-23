// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeDelay.h"
#include "vf/vf_LockFreeReadWriteMutex.h"

#define USE_DEBUG_MUTEX 0

namespace LockFree {

#if USE_DEBUG_MUTEX

ReadWriteMutex::ReadWriteMutex () { }
ReadWriteMutex::~ReadWriteMutex () { }
void ReadWriteMutex::enter_read () { m_mutex.enter (); }
void ReadWriteMutex::exit_read () { m_mutex.exit (); }
void ReadWriteMutex::enter_write () { m_mutex.enter (); }
void ReadWriteMutex::exit_write () { m_mutex.exit (); }

#else

ReadWriteMutex::ReadWriteMutex ()
{
}

ReadWriteMutex::~ReadWriteMutex ()
{
}

void ReadWriteMutex::enter_read ()
{
  for (;;)
  {
    // attempt the lock optimistically
    m_readers.addref ();

    // is there a writer?
    if (m_writes.is_signaled ())
    {
      // a writer exists, give up the read lock
      m_readers.release ();

      // block until the writer is done
      {
        ScopedLock lock (m_mutex);
      }

      // now try the loop again
    }
    else
    {
      break;
    }
  }
}

void ReadWriteMutex::exit_read ()
{
  m_readers.release ();
}

void ReadWriteMutex::enter_write ()
{
  // Optimistically acquire the write lock.
  m_writes.addref ();

  // Go for the mutex.
  // Another writer might block us here.
  m_mutex.enter ();

  // Only one competing writer will get here,
  // but we don't know who, so we have to drain
  // readers no matter what. New readers will be
  // blocked by the mutex.
  Delay delay; 
  while (m_readers.is_signaled ())
    delay.spin ();
}

void ReadWriteMutex::exit_write ()
{
  // Releasing the mutex first and then decrementing the
  // writer count allows another waiting writer to atomically
  // acquire the lock, thus starving readers. This fulfills
  // the write-preferencing requirement.

  m_mutex.exit ();

  m_writes.release ();
}

#endif

}

END_VF_NAMESPACE

