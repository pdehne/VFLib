// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__
#define __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_CacheLinePadding.h"
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
  void enter_read () const;
  void exit_read () const;

  // Recursive.
  // Cannot hold a read lock when acquiring a write lock.
  void enter_write () const;
  void exit_write () const;

private:
  NoCacheLinePadding <Mutex> m_mutex;
  mutable NoCacheLinePadding <Atomic::Counter> m_writes;
  mutable NoCacheLinePadding <Atomic::Counter> m_readers;
};

struct ScopedReadLock : NonCopyable
{
  inline explicit ScopedReadLock (const ReadWriteMutex& mutex)
    : m_mutex (mutex) { m_mutex.enter_read (); }
  inline ~ScopedReadLock () { m_mutex.exit_read (); }
private:
  const ReadWriteMutex& m_mutex;
};

struct ScopedWriteLock : NonCopyable
{
  inline explicit ScopedWriteLock (const ReadWriteMutex& mutex)
    : m_mutex (mutex) { m_mutex.enter_write (); }
  inline ~ScopedWriteLock () { m_mutex.exit_write (); }
private:
  const ReadWriteMutex& m_mutex;
};

}

#endif
