// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_ATOMICSTATE_VFHEADER
#define VF_ATOMICSTATE_VFHEADER

class AtomicState
{
public:
  explicit AtomicState (const int s = 0) noexcept
	: m_value (s)
  {
  }

  // Caller must synchronize
  inline operator int () const
  {
	return m_value.get();
  }

  inline bool tryChangeState (const int from, const int to) noexcept
  {
    return m_value.compareAndSetBool (to, from);
  }

  inline void changeState (const int from, const int to) noexcept
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

#endif
