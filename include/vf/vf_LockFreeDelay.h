// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEDELAY_VFHEADER__
#define __VF_LOCKFREEDELAY_VFHEADER__

#include "vf/vf_Intrinsics.h"
#include "vf/vf_Thread.h"

namespace LockFree {

//
// Synchronization delay element which avoids kernel blocking
//

class Delay
{
public:
  Delay () : m_backoff (0)
  {
  }

#if 0
  ~Delay ()
  {
    if (m_backoff > 1)
    {
      String s;
      s << "m_backoff = " << String (m_backoff);
      Logger::outputDebugString (s);
    }
  }
#endif

  inline void spin ()
  {
    if (m_backoff < 10)
    {
      Intrinsic::mm_pause <1> ();
    }
    else if (m_backoff < 20)
    {
      Intrinsic::mm_pause <50> ();
    }
    else if (m_backoff < 22)
    {
      CurrentThread::yield();
    }
    else if (m_backoff < 24)
    {
      CurrentThread::sleep (0);
    }
    else if (m_backoff < 26)
    {
      CurrentThread::sleep (1);
    }
    else
    {
      CurrentThread::sleep (10);
    }

    ++m_backoff;
  }

private:
  int m_backoff;
};

}

#endif
