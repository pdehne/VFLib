// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

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
