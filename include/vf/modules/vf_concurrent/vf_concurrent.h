// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CONCURRENT_VFHEADER
#define VF_CONCURRENT_VFHEADER

#include "../vf_core/vf_core.h"

namespace vf
{
#include "lockfree/vf_LockFreeList.h"
#include "lockfree/vf_LockFreeQueue.h"
#include "lockfree/vf_LockFreeStack.h"

#include "memory/vf_AllocatedBy.h"
#include "memory/vf_Allocator.h"
#include "memory/vf_AllocatorWithoutTls.h"
#include "memory/vf_PageAllocator.h"
#include "memory/vf_StandardAllocator.h"

#include "queue/vf_CallQueue.h"
#include "queue/vf_GuiCallQueue.h"
#include "queue/vf_Listeners.h"
#include "queue/vf_ManualCallQueue.h"
#include "queue/vf_ThreadWorker.h"

#include "threads/vf_ReadWriteMutex.h"
#include "threads/vf_ReadWriteMutexBase.h"
#include "threads/vf_SharedObject.h"
#include "threads/vf_SharedState.h"
}

#endif

