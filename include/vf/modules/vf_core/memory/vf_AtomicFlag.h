// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_ATOMICFLAG_VFHEADER
#define VF_ATOMICFLAG_VFHEADER

class AtomicFlag
{
public:
  // Starts non-signaled
  AtomicFlag ()
	: m_value (0)
  {
  }

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
  inline bool isSet () const
  {
	return m_value.get() == 1;
  }

  // Caller must synchronize
  inline bool isClear () const
  {
	return m_value.get() == 0;
  }

private:
  VF_JUCE::Atomic <int> m_value;
};

#endif
