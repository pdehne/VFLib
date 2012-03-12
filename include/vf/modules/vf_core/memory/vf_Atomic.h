// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_ATOMIC_VFHEADER__
#define __VF_ATOMIC_VFHEADER__

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
    const bool final = release();
    vfassert (final);
#else
    release ();
#endif
  }

  // Returns the signaled state of the counter.
  // The caller must synchronize the value.
  inline bool is_reset () const { return m_value.get() == 0; }
  inline bool is_signaled () const { return m_value.get() > 0; }

  // Caller must synchronize.
  inline void set (int value) { m_value.set (value); }

  // for diagnostics ONLY!
  inline int get () const { return m_value.get(); }

private:
  VF_JUCE::Atomic <int> m_value;
};

//------------------------------------------------------------------------------

class Flag
{
public:
  // Starts non-signaled
  Flag () : m_value (0) { }

  inline void set ()
  {
#if VF_DEBUG
    const bool success = m_value.compareAndSetBool (1, 0);
    vfassert (success);
#else
    m_value.set (1);
#endif
  }

  inline void clear ()
  {
#if VF_DEBUG
    const bool success = m_value.compareAndSetBool (0, 1);
    vfassert (success);
#else
    m_value.set (0);
#endif
  }

  // returns true if it was successful at changing the flag
  inline bool trySet ()
  {
    return m_value.compareAndSetBool (1, 0);
  }

  // returns true if it was successful at changing the flag
  inline bool tryClear ()
  {
    return m_value.compareAndSetBool (0, 1);
  }

  // Caller must synchronize
  inline bool isSet () const { return m_value.get() == 1; }
  inline bool isClear () const { return m_value.get() == 0; }

private:
  VF_JUCE::Atomic <int> m_value;
};

//------------------------------------------------------------------------------

template <class P>
class Pointer
{
public:
  explicit Pointer (P* p = 0) : m_value (p) { }

  inline P* get () const { return m_value.get(); }
  inline operator P* () const { return get(); }
  inline operator P& () const { return &get(); }
  inline P* operator-> () const { return get(); }

  inline void set (P* p) { m_value.set (p); }
  inline void operator= (P* p) { set (p); }
  inline void operator= (P& p) { set (&p); }

  // returns the previous value
  inline P* exchange (P* p) { return m_value.exchange (p); }

  inline bool compareAndSet (P* newValue, P* oldValue)
  {
    return m_value.compareAndSetBool (newValue, oldValue);
  }

private:
  VF_JUCE::Atomic <P*> m_value;
};

//------------------------------------------------------------------------------

class State
{
public:
  explicit State (const int s = 0) : m_value (s) { }

  // Caller must synchronize
  inline operator int () const { return m_value.get(); }

  inline bool tryChangeState (const int from, const int to)
  {
    return m_value.compareAndSetBool (to, from);
  }

  inline void changeState (const int from, const int to)
  {
#if VF_DEBUG
    const bool success = tryChangeState (from, to);
    vfassert (success);
#else
    tryChangeState (from, to);
#endif
  }

private:
  VF_JUCE::Atomic <int> m_value;
};

}

#endif
