// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_INCLUDES_VFHEADER__
#define __VF_INCLUDES_VFHEADER__

#ifndef __VF_ALLOCATOR_VFHEADER__
#include "vf/vf_Allocator.h"
#endif

#ifndef __VF_ATOMIC_VFHEADER__
#include "vf/vf_Atomic.h"
#endif

#ifndef __VF_AUDIOSAMPLEBUFFERARRAY_VFHEADER__
#include "vf/vf_AudioSampleBufferArray.h"
#endif

#ifndef __VF_BIND_VFHEADER__
#include "vf/vf_Bind.h"
#endif

#ifndef __VF_CATCHANY_VFHEADER__
#include "vf/vf_CatchAny.h"
#endif

#ifndef __VF_CACHELINEPADDING_VFHEADER__
#include "vf/vf_CacheLinePadding.h"
#endif

#ifndef __VF_FPUFLAGS_VFHEADER__
#include "vf/vf_FPUFlags.h"
#endif

#ifndef __VF_FUNCTION_VFHEADER__
#include "vf/vf_Function.h"
#endif

#ifndef __VF_GUIWORKER_VFHEADER__
#include "vf/vf_GuiWorker.h"
#endif

#ifndef __VF_INTERVAL_VFHEADER__
#include "vf/vf_Interval.h"
#endif

#ifndef __VF_INTRINSICS_VFHEADER__
#include "vf/vf_Intrinsics.h"
#endif

#ifndef __VF_LIST_VFHEADER__
#include "vf/vf_List.h"
#endif

#ifndef __VF_LOCKFREEALLOCATOR_VFHEADER__
#include "vf/vf_LockFreeAllocator.h"
#endif

#ifndef __VF_LOCKFREEDELAY_VFHEADER__
#include "vf/vf_LockFreeDelay.h"
#endif

#ifndef __VF_LOCKFREELIST_VFHEADER__
#include "vf/vf_LockFreeList.h"
#endif

#ifndef __VF_LOCKFREEQUEUE_VFHEADER__
#include "vf/vf_LockFreeQueue.h"
#endif

#ifndef __VF_LOCKFREEREADWRITEMUTEX_VFHEADER__
#include "vf/vf_LockFreeReadWriteMutex.h"
#endif

#ifndef __VF_LOCKFREESTACK_VFHEADER__
#include "vf/vf_LockFreeStack.h"
#endif

#ifndef __VF_MATH_VFHEADER__
#include "vf/vf_Math.h"
#endif

#ifndef __VF_MANUALWORKER_VFHEADER__
#include "vf/vf_ManualWorker.h"
#endif

#ifndef __VF_MUTEX_VFHEADER__
#include "vf/vf_Mutex.h"
#endif

#ifndef __VF_ONCEPERSECOND_VFHEADER__
#include "vf/vf_OncePerSecond.h"
#endif

#ifndef __VF_SHAREDSINGLETON_VFHEADER__
#include "vf/vf_SharedSingleton.h"
#endif

#ifndef __VF_SHAREDSTATE_VFHEADER__
#include "vf/vf_SharedState.h"
#endif

#ifndef __VF_STATICMUTEX_VFHEADER__
#include "vf/vf_StaticMutex.h"
#endif

#ifndef __VF_STATICOBJECT_VFHEADER__
#include "vf/vf_StaticObject.h"
#endif

#ifndef __VF_SYNCHRONIZEDOBJECT_VFHEADER__
#include "vf/vf_SynchronizedObject.h"
#endif

#ifndef __VF_THREAD_VFHEADER__
#include "vf/vf_Thread.h"
#endif

#ifndef __VF_THREADWORKER_VFHEADER__
#include "vf/vf_ThreadWorker.h"
#endif

#ifndef __VF_TYPE_VFHEADER__
#include "vf/vf_Type.h"
#endif

#ifndef __VF_WORKER_VFHEADER__
#include "vf/vf_Worker.h"
#endif


  #ifndef __VF_LOCKFREE_VFHEADER__
  #include "vf/vf_LockFree.h"
  #endif

  #ifndef __VF_SHAREDOBJECT_VFHEADER__
  #include "vf/vf_SharedObject.h"
  #endif

  #ifndef __VF_LISTENERS_VFHEADER__
  #include "vf/vf_Listeners.h"
  #endif

  #ifndef __VF_MURMURHASH_VFHEADER__
  #include "vf/vf_MurmurHash.h" // Depends on some Juce declarations and macros
  #endif

  #ifndef __VF_DB_VFHEADER__
  #include "vf/vf_db.h" // db uses both some Juce and Boost
  #endif

//
// Juce-specific
//

#if VF_HAVE_JUCE

  #ifndef __VF_AUDIOBUFFERPOOL_VFHEADER__
  #include "vf/vf_AudioBufferPool.h"
  #endif

  #ifndef __VF_RADIALIMAGECONVOLUTIONKERNEL_VFHEADER__
  #include "vf/vf_RadialImageConvolutionKernel.h"
  #endif

#endif

#endif
