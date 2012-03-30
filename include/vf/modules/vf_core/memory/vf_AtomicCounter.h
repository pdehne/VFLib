// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_ATOMICCOUNTER_VFHEADER
#define VF_ATOMICCOUNTER_VFHEADER

class AtomicCounter
{
public:
  AtomicCounter (int initialValue = 0)
	: m_value (initialValue)
  {
  }

  //
  // Increments the usage count.
  // Returns true if the counter was previously zero.
  //
  inline bool addref ()
  {
	return (++m_value) == 1;
  }

  // Decrements the usage count.
  // Returns true if the counter became non-signaled.
  inline bool release ()
  {
	return (--m_value) == 0;
  }

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
  inline bool is_reset () const
  {
	return m_value.get() == 0;
  }

  inline bool is_signaled () const
  {
	return m_value.get() > 0;
  }

  // Caller must synchronize.
  inline void set (int value)
  {
	m_value.set (value);
  }

  // for diagnostics ONLY!
  inline int get () const
  {
	return m_value.get();
  }

private:
  VF_JUCE::Atomic <int> m_value;
};

#endif
