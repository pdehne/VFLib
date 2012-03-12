// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeDelay.h"

#include "vf/modules/vf_core/threads/vf_Thread.h"

namespace LockFree {

void Delay::spin ()
{
  CurrentThread::yield ();
}

//------------------------------------------------------------------------------

DelayWithBackoff::DelayWithBackoff () : m_backoff (0)
{
}

void DelayWithBackoff::spin ()
{
#if 0
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

#else
  CurrentThread::yield ();

#endif
}

}

END_VF_NAMESPACE
