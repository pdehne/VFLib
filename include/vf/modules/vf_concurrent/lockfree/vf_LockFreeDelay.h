// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_LOCKFREEDELAY_VFHEADER__
#define __VF_LOCKFREEDELAY_VFHEADER__

namespace LockFree {

//
// Synchronization delay element which avoids kernel blocking
//

class Delay
{
public:
  void spin ();
};

class DelayWithBackoff
{
public:
  DelayWithBackoff ();

  void spin ();

private:
  int m_backoff;
};

}

#endif
