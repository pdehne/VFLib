// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_MemoryAlignment.h"
#include "vf/vf_PagePool.h"

PagePool::PagePool (int bytesPerPage)
  : m_bytesPerPage (bytesPerPage)
{
}

PagePool::~PagePool ()
{
}

void* PagePool::allocate ()
{
  return 0;
}

END_VF_NAMESPACE
