// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCEMUTEX_VFHEADER__
#define __VF_JUCEMUTEX_VFHEADER__

#if VF_HAVE_JUCE

namespace Juce {

class Mutex : NonCopyable
{
public:
  inline void enter () const { m_mutex.enter (); }
  inline void exit () const { m_mutex.exit (); }

private:
  VF_JUCE::CriticalSection m_mutex;
};

}

#endif

#endif
