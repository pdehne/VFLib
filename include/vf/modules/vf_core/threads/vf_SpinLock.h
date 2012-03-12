// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SPINLOCK_VFHEADER__
#define __VF_SPINLOCK_VFHEADER__

#include "vf/modules/vf_core/memory/vf_Atomic.h"
#include "vf/modules/vf_concurrent/lockfree/vf_LockFreeDelay.h"
#include "vf/modules/vf_core/threads/vf_Mutex.h"

class SpinLock
{
public:
  typedef ScopedLock <SpinLock> ScopedLockType;

  inline void enter () const
  {
    if (!m_lock.trySet ())
    {
      LockFree::Delay delay;
      for (;;)
      {
        delay.spin ();
        if (m_lock.trySet ())
          break;
      }
    }
  }

  inline void exit () const
  {
    m_lock.clear ();
  }

private:
  mutable Atomic::Flag m_lock;
};

#endif

