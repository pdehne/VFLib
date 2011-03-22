// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__
#define __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_Mutex.h"

namespace LockFree {

// Multiple-reader, single writer, write preferenced
// recursive mutex with a lock-free fast path.
//
class ReadWriteMutex
{
public:
  ReadWriteMutex ();
  ~ReadWriteMutex ();

  // Recursive.
  void enter_read ();
  void exit_read ();

  // Recursive.
  // Cannot hold a read lock when acquiring a write lock.
  void enter_write ();
  void exit_write ();

private:
  Mutex m_mutex;
  Atomic::UsageCounter m_writes;
  Atomic::UsageCounter m_readers;
};

}

#endif
