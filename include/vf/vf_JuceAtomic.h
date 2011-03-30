// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCEATOMIC_VFHEADER__
#define __VF_JUCEATOMIC_VFHEADER__

#if VF_HAVE_JUCE

#include "vf/vf_Intrinsics.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Thread.h"

namespace Juce {

namespace Atomic {

inline void memoryBarrier ()
{
  VF_JUCE::Atomic <int>::memoryBarrier ();
}

//------------------------------------------------------------------------------

// Tracks the amount of usage of a particular resource.
// The object is considered signaled if there are one or more uses
class Counter
{
public:
  Counter (int initialValue = 0) : m_value (initialValue) { }

  // Increments the usage count.
  // Returns true if the counter was previously non-signaled.
  inline bool addref () { return (++m_value) == 1; }

  // Decrements the usage count.
  // Returns true if the counter became non-signaled.
  inline bool release () { return (--m_value) == 0; }

  // Decrements the usage count and asserts that it a final release.
  inline void finalRelease ()
  {
#if VF_DEBUG
    const bool final = release(); vfassert (final);
#else
    release ();
#endif
  }

  // Returns the signaled state of the counter.
  // The caller must synchronize the value.
  bool is_reset () const { return m_value.get() == 0; }
  bool is_signaled () const { return m_value.get() > 0; }

  // Caller must synchronize.
  void set (int value) { m_value.set (value); }

  // for diagnostics ONLY!
  int get () const { return m_value.get(); }

private:
  VF_JUCE::Atomic <int> m_value;
};

//------------------------------------------------------------------------------

class Flag
{
public:
  // Starts non-signaled
  Flag () : m_value (0) { }

  void set ()
  {
#if VF_DEBUG
    const bool success = m_value.compareAndSetBool (1, 0);
    vfassert (success);
#else
    m_value.set (1);
#endif
  }

  void clear ()
  {
#if VF_DEBUG
    const bool success = m_value.compareAndSetBool (0, 1);
    vfassert (success);
#else
    m_value.set (0);
#endif
  }

  // returns true if it was successful at changing the flag
  bool trySet ()
  {
    return m_value.compareAndSetBool (1, 0);
  }

  // returns true if it was successful at changing the flag
  bool tryClear ()
  {
    return m_value.compareAndSetBool (0, 1);
  }

  // Caller must synchronize
  bool isSet () const { return m_value.get() == 1; }
  bool isClear () const { return m_value.get() == 0; }

private:
  VF_JUCE::Atomic <int> m_value;
};

//------------------------------------------------------------------------------

template <class P>
class Pointer
{
public:
  explicit Pointer (P* p = 0) : m_value (p) { }

  P* get () const { return m_value.get(); }
  operator P* () const { return get(); }
  operator P& () const { return &get(); }
  P* operator-> () const { return get(); }

  void set (P* p) { m_value.set (p); }
  void operator= (P* p) { set (p); }
  void operator= (P& p) { set (&p); }

  // returns the previous value
  P* exchange (P* p) { return m_value.exchange (p); }

  bool compareAndSet (P* newValue, P* oldValue)
  {
    return m_value.compareAndSetBool (newValue, oldValue);
  }

private:
  VF_JUCE::Atomic <P*> m_value;
};

}

}

#endif

#endif
