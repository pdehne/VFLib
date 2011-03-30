// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MUTEXBASE_VFHEADER__
#define __VF_MUTEXBASE_VFHEADER__

namespace detail {

// Ideas based on Juce

template <class MutexType>
struct ScopedLock : NonCopyable
{
  inline explicit ScopedLock (const MutexType& mutex) : m_mutex (mutex)
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

template <class MutexType>
struct ScopedUnlock : NonCopyable
{
  inline explicit ScopedUnlock (const MutexType& mutex) : m_mutex (mutex)
  {
    m_mutex.exit ();
  }

  inline ~ScopedUnlock ()
  {
    m_mutex.enter ();
  }

private:
  const MutexType& m_mutex;
};

}

#endif
