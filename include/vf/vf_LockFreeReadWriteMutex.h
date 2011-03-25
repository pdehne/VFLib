// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__
#define __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_Mutex.h"

namespace LockFree {

// Multiple-reader, single writer, write preferenced
// recursive mutex with a wait-free fast path.
//
class ReadWriteMutex
{
public:
  ReadWriteMutex ();
  ~ReadWriteMutex ();

  // Recursive.
  void enter_read ();
  void exit_read ();

  // Recursive.
  // Cannot hold a read lock when acquiring a write lock.
  void enter_write ();
  void exit_write ();

private:
  Mutex m_mutex;
  Atomic::Counter m_writes;
  Atomic::Counter m_readers;
};

struct ScopedReadLock : NonCopyable
{
  inline explicit ScopedReadLock (ReadWriteMutex& mutex)
    : m_mutex (mutex) { m_mutex.enter_read (); }
  inline ~ScopedReadLock () { m_mutex.exit_read (); }
private:
  ReadWriteMutex& m_mutex;
};

struct ScopedWriteLock : NonCopyable
{
  inline explicit ScopedWriteLock (ReadWriteMutex& mutex)
    : m_mutex (mutex) { m_mutex.enter_write (); }
  inline ~ScopedWriteLock () { m_mutex.exit_write (); }
private:
  ReadWriteMutex& m_mutex;
};

}

#endif
