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
