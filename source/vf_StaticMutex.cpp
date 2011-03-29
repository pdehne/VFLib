// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeDelay.h"
#include "vf/vf_StaticMutex.h"

void StaticMutex::enter () const
{
  // Did we initialize?
  if (m_inited->isClear ())
  {
    // No so try to do it.
    if (m_initing->trySet ())
    {
      // We set the flag, everyone else fails.
      // Construct the Mutex with placement new.
      new (m_mutex.getObject()) Mutex;

      // Set flag.
      m_inited->set ();
    }
    else
    {
      // Wait until the thread that set the flag initializes.
      LockFree::Delay delay;
      do
      {
        delay.spin ();
      }
      while (m_inited->isClear ());
    }
  }

  m_mutex->enter ();
}

void StaticMutex::exit () const
{
  m_mutex->exit ();
}

END_VF_NAMESPACE
