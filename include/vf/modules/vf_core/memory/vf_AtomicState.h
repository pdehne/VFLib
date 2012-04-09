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
