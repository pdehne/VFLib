// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_READWRITEMUTEX_VFHEADER__
#define __VF_READWRITEMUTEX_VFHEADER__

#include "vf/modules/vf_core/memory/vf_Atomic.h"
#include "vf/modules/vf_core/memory/vf_CacheLine.h"
#include "Vf/modules/vf_core/threads/vf_Mutex.h"
#include "vf/modules/vf_concurrent/threads/vf_ReadWriteMutexBase.h"

// Multiple-reader, single writer, write preferenced
// partially recursive mutex with a wait-free fast path.
//
class ReadWriteMutex
{
public:
  typedef detail::ScopedReadLock <ReadWriteMutex> ScopedReadLockType;
  typedef detail::ScopedWriteLock <ReadWriteMutex> ScopedWriteLockType;

#if 0
  typedef detail::ScopedUpgradeWriteLock <ReadWriteMutex> ScopedUpgradeWriteLockType;
#endif

  ReadWriteMutex ();
  ~ReadWriteMutex ();

  // Recursive.
  void enter_read () const;
  void exit_read () const;

  // Recursive.
  // Cannot hold a read lock when acquiring a write lock.
  void enter_write () const;
  void exit_write () const;

  // CAUSES DEADLOCK
#if 0
  // Non-recursive.
  // Caller must hold exactly one read lock.
  // The lock is released with exit_write().
  void upgrade_write () const;

  // Caller must hold exactly one write lock.
  void downgrade_read () const;
#endif

private:
  CacheLine::Padded <Mutex> m_mutex;
  mutable CacheLine::Padded <Atomic::Counter> m_writes;
  mutable CacheLine::Padded <Atomic::Counter> m_readers;
};

typedef ReadWriteMutex::ScopedReadLockType ScopedReadLock;
typedef ReadWriteMutex::ScopedWriteLockType ScopedWriteLock;

#endif
