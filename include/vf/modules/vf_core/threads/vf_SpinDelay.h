// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_SPINDELAY_VFHEADER
#define VF_SPINDELAY_VFHEADER

//
// Synchronization element
//

class SpinDelay
{
public:
  SpinDelay () : m_count (0)
  {
  }

  inline void pause ()
  {
	if (++m_count > 20)
	{
	  VF_JUCE::Thread::yield ();
	}
  }

private:
  int m_count;
};

#endif
