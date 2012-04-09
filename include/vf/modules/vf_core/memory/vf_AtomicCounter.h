//==============================================================================
//
// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file GNU_GPL_v2.txt for full licensing terms.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program; if not, write to the Free Software Foundation, Inc., 51
// Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
// 
//==============================================================================

#ifndef VF_ATOMICCOUNTER_VFHEADER
#define VF_ATOMICCOUNTER_VFHEADER

class AtomicCounter
{
public:
  AtomicCounter (int initialValue = 0) noexcept
	: m_value (initialValue)
  {
  }

  //
  // Increments the usage count.
  // Returns true if the counter was previously zero.
  //
  inline bool addref () noexcept
  {
	return (++m_value) == 1;
  }

  // Decrements the usage count.
  // Returns true if the counter became non-signaled.
  inline bool release () noexcept
  {
	return (--m_value) == 0;
  }

  // Decrements the usage count and asserts that it a final release.
  inline void finalRelease () noexcept
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
  inline bool is_reset () const noexcept
  {
	return m_value.get() == 0;
  }

  inline bool is_signaled () const noexcept
  {
	return m_value.get() > 0;
  }

  // Caller must synchronize.
  inline void set (int value) noexcept
  {
	m_value.set (value);
  }

  // for diagnostics ONLY!
  inline int get () const noexcept
  {
	return m_value.get();
  }

private:
  VF_JUCE::Atomic <int> m_value;
};

#endif
