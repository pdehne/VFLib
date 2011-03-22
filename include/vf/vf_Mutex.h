// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MUTEX_VFHEADER__
#define __VF_MUTEX_VFHEADER__

#include "vf/vf_BoostMutex.h"
#include "vf/vf_JuceMutex.h"

//
// Simple lightweight recursive mutex
//

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

// Lift one implementation

#if VF_HAVE_JUCE

using Juce::Mutex;

#elif VF_HAVE_BOOST

using Boost::Mutex;

#else

#pragma error ("Missing Mutex implementation")

#endif

typedef detail::ScopedLock <Mutex> ScopedLock;
typedef detail::ScopedUnlock <Mutex> ScopedUnlock;

#endif
