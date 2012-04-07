// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CONCURRENT_VFHEADER
#define VF_CONCURRENT_VFHEADER

#include "../vf_core/vf_core.h"

namespace vf
{
#include "containers/vf_LockFreeQueue.h"
#include "containers/vf_LockFreeStack.h"

#include "memory/vf_AllocatedBy.h"
#include "memory/vf_FifoFreeStore.h"
#include "memory/vf_FifoFreeStoreWithoutTLS.h"
#include "memory/vf_FifoFreeStoreWithTLS.h"
#include "memory/vf_GlobalFifoFreeStore.h"
#include "memory/vf_GlobalPagedFreeStore.h"
#include "memory/vf_PagedFreeStore.h"

#include "threads/vf_CallQueue.h"
#include "threads/vf_ConcurrentObject.h"
#include "threads/vf_ConcurrentState.h"
#include "threads/vf_GuiCallQueue.h"
#include "threads/vf_Listeners.h"
#include "threads/vf_ManualCallQueue.h"
#include "threads/vf_ReadWriteMutex.h"
#include "threads/vf_ThreadWithCallQueue.h"
}

#endif

