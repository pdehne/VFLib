// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeAllocator.h"

namespace LockFree {

FixedAllocator <globalAllocatorBlockSize> globalAllocator;

}

END_VF_NAMESPACE

