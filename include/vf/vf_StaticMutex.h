// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STATICMUTEX_VFHEADER__
#define __VF_STATICMUTEX_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_StaticData.h"
#include "vf/vf_Mutex.h"

// Mutex that ONLY works with static storage duration!
// Any other usage is undefined.
//
class StaticMutex : NonCopyable
{
public:
  typedef detail::ScopedLock <StaticMutex> ScopedLockType;

  void enter () const;
  void exit () const;

private:
  mutable StaticData <Atomic::Flag> m_inited;
  mutable StaticData <Atomic::Flag> m_initing;
  mutable StaticData <Mutex> m_mutex;
};

#endif
