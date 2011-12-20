// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

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
