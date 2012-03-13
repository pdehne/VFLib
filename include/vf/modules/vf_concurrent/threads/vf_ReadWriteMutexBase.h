// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_READWRITEMUTEXBASE_VFHEADER__
#define __VF_READWRITEMUTEXBASE_VFHEADER__

namespace detail {

template <class ReadWriteMutexType>
struct ScopedReadLock : NonCopyable
{
  inline explicit ScopedReadLock (const ReadWriteMutexType& mutex)
    : m_mutex (mutex)
  {
    m_mutex.enter_read ();
  }

  inline ~ScopedReadLock ()
  {
    m_mutex.exit_read ();
  }

private:
  const ReadWriteMutexType& m_mutex;
};

//------------------------------------------------------------------------------

template <class ReadWriteMutexType>
struct ScopedWriteLock : NonCopyable
{
  inline explicit ScopedWriteLock (const ReadWriteMutexType& mutex)
    : m_mutex (mutex)
  {
    m_mutex.enter_write ();
  }

  inline ~ScopedWriteLock ()
  {
    m_mutex.exit_write ();
  }

private:
  const ReadWriteMutexType& m_mutex;
};

//------------------------------------------------------------------------------

// CAUSES DEADLOCK
#if 0
template <class ReadWriteMutexType>
struct ScopedUpgradeWriteLock : NonCopyable
{
  inline explicit ScopedUpgradeWriteLock (const ReadWriteMutexType& mutex)
    : m_mutex (mutex)
  {
    m_mutex.upgrade_write ();
  }

  inline ~ScopedUpgradeWriteLock ()
  {
    m_mutex.downgrade_read ();
  }

private:
  const ReadWriteMutexType& m_mutex;
};
#endif

}

#endif
