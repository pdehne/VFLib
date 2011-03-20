// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_ConditionVariable.h"

UnfinishedConditionVariable::UnfinishedConditionVariable ()
: m_waiters_count (0)
, m_release_count (0)
, m_wait_generation_count (0)
, m_event (true)
{
}

void UnfinishedConditionVariable::signal ()
{
  m_mutex.enter();

  if (m_waiters_count > m_release_count)
  {
    m_event.signal();
    
    ++m_release_count;
    ++m_wait_generation_count;
  }

  m_mutex.exit();
}

END_VF_NAMESPACE
