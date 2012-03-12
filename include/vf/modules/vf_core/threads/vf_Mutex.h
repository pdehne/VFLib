// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_MUTEX_VFHEADER__
#define __VF_MUTEX_VFHEADER__

// Ideas based on Juce

template <class MutexType>
struct ScopedLock : NonCopyable
{
  inline explicit ScopedLock (MutexType const& mutex) : m_mutex (mutex)
  {
    m_mutex.enter ();
  }

  inline ~ScopedLock ()
  {
    m_mutex.exit ();
  }

private:
  const MutexType& m_mutex;
};

//------------------------------------------------------------------------------

template <class MutexType>
struct ScopedUnlock : NonCopyable
{
  inline explicit ScopedUnlock (MutexType const& mutex) : m_mutex (mutex)
  {
    m_mutex.exit ();
  }

  inline ~ScopedUnlock ()
  {
    m_mutex.enter ();
  }

private:
  MutexType const& m_mutex;
};

//------------------------------------------------------------------------------

class NoMutex : NonCopyable
{
public:
  typedef ScopedLock <NoMutex> ScopedLockType;
  typedef ScopedUnlock <NoMutex> ScopedUnlockType;

  static inline void enter () { }
  static inline void exit () { }
};

//------------------------------------------------------------------------------

class Mutex : NonCopyable
{
public:
  typedef ScopedLock <Mutex> ScopedLockType;
  typedef ScopedUnlock <Mutex> ScopedUnlockType;

  inline void enter () const { m_mutex.enter (); }
  inline void exit () const { m_mutex.exit (); }

private:
  VF_JUCE::CriticalSection m_mutex;
};

#endif
