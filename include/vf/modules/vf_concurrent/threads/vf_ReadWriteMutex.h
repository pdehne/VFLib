// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_READWRITEMUTEX_VFHEADER__
#define __VF_READWRITEMUTEX_VFHEADER__

template <class LockType>
struct GenericScopedReadLock : Uncopyable
{
  inline explicit GenericScopedReadLock (LockType const& lock) noexcept
    : m_lock (lock)
  {
    m_lock.enterRead ();
  }

  inline ~GenericScopedReadLock () noexcept
  {
    m_lock.exitRead ();
  }

private:
  LockType const& m_lock;
};

template <class LockType>
struct GenericScopedWriteLock : Uncopyable
{
  inline explicit GenericScopedWriteLock (LockType const& lock) noexcept
    : m_lock (lock)
  {
    m_lock.enterWrite ();
  }

  inline ~GenericScopedWriteLock () noexcept
  {
    m_lock.exitWrite ();
  }

private:
  LockType const& m_lock;
};

//------------------------------------------------------------------------------

// Multiple-reader, single writer, write preferenced
// partially recursive mutex with a wait-free fast path.
//
class ReadWriteMutex
{
public:
  typedef GenericScopedReadLock <ReadWriteMutex> ScopedReadLockType;
  typedef GenericScopedWriteLock <ReadWriteMutex> ScopedWriteLockType;

  ReadWriteMutex () noexcept;
  ~ReadWriteMutex () noexcept;

  // Recursive
  //
  void enterRead () const noexcept;
  void exitRead () const noexcept;

  // Recursive
  // Cannot hold a read lock when acquiring a write lock.
  //
  void enterWrite () const noexcept;
  void exitWrite () const noexcept;

private:
  VF_JUCE::CriticalSection m_mutex;

  mutable CacheLine::Padded <AtomicCounter> m_writes;
  mutable CacheLine::Padded <AtomicCounter> m_readers;
};

//typedef GenericScopedReadLock::ScopedReadLockType ScopedReadLock;
//typedef GenericScopedWriteLock::ScopedWriteLockType ScopedWriteLock;

#endif
