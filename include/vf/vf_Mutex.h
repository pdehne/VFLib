// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MUTEX_VFHEADER__
#define __VF_MUTEX_VFHEADER__

//
// Simple lightweight recursive mutex
//
// Juce::Mutex
// Boost::Mutex
// template <MutexType> detail::ScopedLock
// template <MutexType> detail::ScopedUnlock
// Mutex
// ScopedLock
// ScopedUnlock
//

//------------------------------------------------------------------------------
//
// Juce Mutex
//

#if VF_HAVE_JUCE

namespace Juce {

struct Mutex : NonCopyable
{
  inline void enter () const { m_mutex.enter (); }
  inline void exit () const { m_mutex.exit (); }

private:
  VF_JUCE::CriticalSection m_mutex;
};

}

#endif

//------------------------------------------------------------------------------
//
// Boost Mutex
//

#if VF_HAVE_BOOST

namespace Boost {

struct Mutex : NonCopyable
{
  inline void enter () { m_mutex.lock (); }
  inline void exit () { m_mutex.unlock (); }

private:
  boost::recursive_mutex m_mutex;
};

}

#endif

//------------------------------------------------------------------------------

namespace detail {

// Ideas based on Juce

template <class MutexType>
struct ScopedLock : NonCopyable
{
  inline explicit ScopedLock (const MutexType& mutex)
    : m_mutex (mutex) { m_mutex.enter (); }
  inline ~ScopedLock () { m_mutex.exit (); }
private:
  const MutexType& m_mutex;
};

template <class MutexType>
struct ScopedUnlock : NonCopyable
{
  inline explicit ScopedUnlock (const MutexType& mutex)
    : m_mutex (mutex) { m_mutex.exit (); }
  inline ~ScopedUnlock () { m_mutex.enter (); }
private:
  const MutexType& m_mutex;
};

}

//------------------------------------------------------------------------------

// Lift one implementation

#if VF_HAVE_JUCE

using Juce::Mutex;

#elif VF_HAVE_BOOST

using Boost::Mutex;

#endif

typedef detail::ScopedLock <Mutex> ScopedLock;
typedef detail::ScopedUnlock <Mutex> ScopedUnlock;

#endif
