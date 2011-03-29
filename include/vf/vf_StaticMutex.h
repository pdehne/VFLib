// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STATICMUTEX_VFHEADER__
#define __VF_STATICMUTEX_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeDelay.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_StaticObject.h"

// Mutex with static storage duration.
//
template <class Tag>
class StaticMutex : NonCopyable
{
public:
  struct ScopedLock : NonCopyable
  {
    inline ScopedLock () { StaticMutex::enter (); }
    inline ~ScopedLock () { StaticMutex::exit (); }
  };

  typedef ScopedLock ScopedLockType;

  static void enter ()
  {
    // Did we initialize?
    if (s_inited->isClear ())
    {
      // No so try to do it.
      if (s_initing->trySet ())
      {
        // We set the flag, everyone else fails.
        // Construct the Mutex with placement new.
        new (s_mutex.getObject()) Mutex;

        // Set flag.
        s_inited->set ();
      }
      else
      {
        // Wait until the thread that set the flag initializes.
        LockFree::Delay delay;
        do
        {
          delay.spin ();
        }
        while (s_inited->isClear ());
      }
    }

    s_mutex->enter ();
  }

  static void exit ()
  {
    s_mutex->exit ();
  }

private:
  class CleanerUpper
  {
  public:
    ~CleanerUpper ()
    {
      if (StaticMutex::s_inited->isSet ())
        StaticMutex::s_mutex->~Mutex ();
    }
  };

  static StaticObject <Atomic::Flag> s_inited;
  static StaticObject <Atomic::Flag> s_initing;
  static StaticObject <Mutex> s_mutex;
  static CleanerUpper s_cleanerUpper;
};

template <class Tag>
StaticObject <Atomic::Flag> StaticMutex <Tag>::s_inited;

template <class Tag>
StaticObject <Atomic::Flag> StaticMutex <Tag>::s_initing;

template <class Tag>
StaticObject <Mutex> StaticMutex <Tag>::s_mutex;

template <class Tag>
typename StaticMutex <Tag>::CleanerUpper StaticMutex <Tag>::s_cleanerUpper;

#endif
