// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_READWRITEMUTEX_VFHEADER
#define VF_READWRITEMUTEX_VFHEADER

#ifndef DOXYGEN
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
#endif

//==============================================================================

/** Multiple reader, single writer synchronization primitive.

    This is an optimized lock for the multiple reader, single writer
    scenario. It provides only a subset of features of the more general
    traditional read/write lock. Specifically, these rules apply:

    - A caller cannot hold a read lock while acquiring a write lock.

    - Write locks are only recursive with respect to write locks.

    - Read locks are only recursive with respect to read locks.

    - A write lock cannot be downgraded.

    - Writes are preferenced over reads.

    For real-time applications, these restrictions are often not an issue.

    The implementation is wait-free in the fast path: acquiring read access
    for a lock without contention - just one interlocked increment!
*/
class ReadWriteMutex
{
public:
  /** Provides the type of scoped read lock to use with a ReadWriteMutex. */
  typedef GenericScopedReadLock <ReadWriteMutex> ScopedReadLockType;

  /** Provides the type of scoped write lock to use with a ReadWriteMutex. */
  typedef GenericScopedWriteLock <ReadWriteMutex> ScopedWriteLockType;

  /** Create a ReadWriteMutex */
  ReadWriteMutex () noexcept;
  
  /** Destroy a ReadWriteMutex

      If the object is destroyed while a lock is held, the result is
      undefined behavior.
  */
  ~ReadWriteMutex () noexcept;

  /** Acquire a read lock.

      This is recursive with respect to other read locks. Calling this while
      holding a write lock is undefined.
  */
  void enterRead () const noexcept;

  /** Release a previously acquired read lock */
  void exitRead () const noexcept;

  /** Acquire a write lock.

      This is recursive with respect to other write locks. Calling this while
      holding a read lock is undefined.
  */
  void enterWrite () const noexcept;

  /** Release a previously acquired write lock */
  void exitWrite () const noexcept;

private:
  VF_JUCE::CriticalSection m_mutex;

  mutable CacheLine::Padded <AtomicCounter> m_writes;
  mutable CacheLine::Padded <AtomicCounter> m_readers;
};

#endif
