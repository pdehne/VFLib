// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFree.h"

namespace LockFree {

ReadWriteMutex::ReadWriteMutex ()
{
}

ReadWriteMutex::~ReadWriteMutex ()
{
}

void ReadWriteMutex::enter_read ()
{
  // loop makes us write-preferenced
  for (;;)
  {
    m_readers.addref ();

    if (m_writes.is_signaled ())
    {
      m_readers.release ();

      {
        ScopedLock lock (m_mutex);
      }
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
  // Take this right away
  m_mutex.enter ();

  // Increment write usage
  const bool first = m_writes.addref ();

  // is this needed?
  //Atomic::memoryBarrier();

  if (first)
  {
    // Wait for readers to drain out if we're first
    Spinner wait; 
    while (m_readers.is_signaled ())
      wait.delay ();
  }
}

void ReadWriteMutex::exit_write ()
{
  // Must happen inside the mutex in case
  // there's another writer waiting.
  m_writes.release ();

  m_mutex.exit ();
}

}

END_VF_NAMESPACE

