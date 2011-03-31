// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__
#define __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_CacheLinePadding.h"
#include "vf/vf_ReadWriteMutexBase.h"

namespace LockFree {

// Multiple-reader, single writer, write preferenced
// recursive mutex with a wait-free fast path.
//
class ReadWriteMutex
{
public:
  typedef detail::ScopedReadLock <ReadWriteMutex> ScopedReadLockType;
  typedef detail::ScopedWriteLock <ReadWriteMutex> ScopedWriteLockType;

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

typedef ReadWriteMutex::ScopedReadLockType ScopedReadLock;
typedef ReadWriteMutex::ScopedWriteLockType ScopedWriteLock;

}

#endif
