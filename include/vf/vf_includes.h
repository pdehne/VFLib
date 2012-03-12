// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_INCLUDES_VFHEADER__
#define __VF_INCLUDES_VFHEADER__

#ifndef __VF_ALLOCATEDBY_VFHEADER__
#include "vf/modules/vf_concurrent/memory/vf_AllocatedBy.h"
#endif

#ifndef __VF_ALLOCATOR_VFHEADER__
#include "vf/modules/vf_concurrent/memory/vf_Allocator.h"
#endif

#ifndef __VF_ATOMIC_VFHEADER__
#include "vf/modules/vf_core/memory/vf_Atomic.h"
#endif

#ifndef __VF_BIND_VFHEADER__
#include "vf/modules/vf_core/functor/vf_Bind.h"
#endif

#ifndef __VF_CATCHANY_VFHEADER__
#include "vf/modules/vf_core/diagnostic/vf_CatchAny.h"
#endif

#ifndef __VF_CACHELINE_VFHEADER__
#include "vf/modules/vf_core/memory/vf_CacheLine.h"
#endif

#ifndef __VF_FPUFLAGS_VFHEADER__
#include "vf/modules/vf_core/diagnostic/vf_FPUFlags.h"
#endif

#ifndef __VF_FUNCTION_VFHEADER__
#include "vf/modules/vf_core/functor/vf_Function.h"
#endif

#ifndef __VF_GUIWORKER_VFHEADER__
#include "vf/modules/vf_concurrent/queue/vf_GuiWorker.h"
#endif

#ifndef __VF_INTERVAL_VFHEADER__
#include "vf/modules/vf_core/math/vf_Interval.h"
#endif

#ifndef __VF_LIST_VFHEADER__
#include "vf/modules/vf_core/containers/vf_List.h"
#endif

#ifndef __VF_LISTENERS_VFHEADER__
#include "vf/modules/vf_concurrent/queue/vf_Listeners.h"
#endif

#ifndef __VF_LOCKFREELIST_VFHEADER__
#include "vf/modules/vf_concurrent/lockfree/vf_LockFreeList.h"
#endif

#ifndef __VF_LOCKFREEQUEUE_VFHEADER__
#include "vf/modules/vf_concurrent/lockfree/vf_LockFreeQueue.h"
#endif

#ifndef __VF_LOCKFREESTACK_VFHEADER__
#include "vf/modules/vf_concurrent/lockfree/vf_LockFreeStack.h"
#endif

#ifndef __VF_MATH_VFHEADER__
#include "vf/modules/vf_core/math/vf_Math.h"
#endif

#ifndef __VF_MANUALWORKER_VFHEADER__
#include "vf/modules/vf_concurrent/queue/vf_ManualWorker.h"
#endif

#ifndef __VF_MEMORYALIGNMENT_VFHEADER__
#include "vf/modules/vf_core/memory/vf_MemoryAlignment.h"
#endif

#ifndef __VF_MURMURHASH_VFHEADER__
#include "vf/modules/vf_core/math/vf_MurmurHash.h"
#endif

#ifndef __VF_MUTEX_VFHEADER__
#include "vf/modules/vf_core/threads/vf_Mutex.h"
#endif

#ifndef __VF_OBJECTALLOCATOR_VFHEADER__
#include "vf/modules/vf_concurrent/memory/vf_ObjectAllocator.h"
#endif

#ifndef __VF_ONCEPERSECOND_VFHEADER__
#include "vf/modules/vf_core/system/vf_OncePerSecond.h"
#endif

#ifndef __VF_PAGEALLOCATOR_VFHEADER__
#include "vf/modules/vf_concurrent/memory/vf_PageAllocator.h"
#endif

#ifndef __VF_PERFORMEDATEXIT_VFHEADER__
#include "vf/modules/vf_core/system/vf_PerformedAtExit.h"
#endif

#ifndef __VF_READWRITEMUTEX_VFHEADER__
#include "vf/modules/vf_core/threads/vf_ReadWriteMutex.h"
#endif

#ifndef __VF_READWRITEMUTEXBASE_VFHEADER__
#include "vf/modules/vf_core/threads/vf_ReadWriteMutexBase.h"
#endif

#ifndef __VF_SAFEBOOL_VFHEADER__
#include "vf/modules/vf_core/diagnostic/vf_SafeBool.h"
#endif

#ifndef __VF_SHAREDOBJECT_VFHEADER__
#include "vf/modules/vf_core/threads/vf_SharedObject.h"
#endif

#ifndef __VF_SHAREDOBJECTPTR_VFHEADER__
#include "vf/modules/vf_core/threads/vf_SharedObjectPtr.h"
#endif

#ifndef __VF_SHAREDSINGLETON_VFHEADER__
#include "vf/modules/vf_core/system/vf_SharedSingleton.h"
#endif

#ifndef __VF_SHAREDSTATE_VFHEADER__
#include "vf/modules/vf_core/threads/vf_SharedState.h"
#endif

#ifndef __VF_SPINLOCK_VFHEADER__
#include "vf/modules/vf_core/threads/vf_SpinLock.h"
#endif

#ifndef __VF_STATICOBJECT_VFHEADER__
#include "vf/modules/vf_concurrent/memory/vf_StaticObject.h"
#endif

#ifndef __VF_THREAD_VFHEADER__
#include "vf/modules/vf_core/threads/vf_Thread.h"
#endif

#ifndef __VF_THREADWORKER_VFHEADER__
#include "vf/modules/vf_concurrent/queue/vf_ThreadWorker.h"
#endif

#ifndef __VF_WORKER_VFHEADER__
#include "vf/modules/vf_concurrent/queue/vf_Worker.h"
#endif

// db

#ifndef __VF_DB_VFHEADER__
#include "vf/modules/vf_db/vf_db_old.h" // db uses both some Juce and Boost
#endif

#endif
