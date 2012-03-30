// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_ATOMICFLAG_VFHEADER
#define VF_ATOMICFLAG_VFHEADER

class AtomicFlag
{
public:
  // Starts non-signaled
  AtomicFlag () noexcept
	: m_value (0)
  {
  }

  inline void set () noexcept
  {
#if VF_DEBUG
    const bool success = m_value.compareAndSetBool (1, 0);
    vfassert (success);
#else
    m_value.set (1);
#endif
  }

  inline void clear () noexcept
  {
#if VF_DEBUG
    const bool success = m_value.compareAndSetBool (0, 1);
    vfassert (success);
#else
    m_value.set (0);
#endif
  }

  // returns true if it was successful at changing the flag
  inline bool trySet () noexcept
  {
    return m_value.compareAndSetBool (1, 0);
  }

  // returns true if it was successful at changing the flag
  inline bool tryClear () noexcept
  {
    return m_value.compareAndSetBool (0, 1);
  }

  // Caller must synchronize
  inline bool isSet () const noexcept
  {
	return m_value.get() == 1;
  }

  // Caller must synchronize
  inline bool isClear () const noexcept
  {
	return m_value.get() == 0;
  }

private:
  VF_JUCE::Atomic <int> m_value;
};

#endif
