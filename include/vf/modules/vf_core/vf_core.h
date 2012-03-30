// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CORE_VFHEADER
#define VF_CORE_VFHEADER

namespace vf
{
#include "containers/vf_List.h"

#include "diagnostic/vf_CatchAny.h"
#include "diagnostic/vf_Debug.h"
#include "diagnostic/vf_Error.h"
#include "diagnostic/vf_FPUFlags.h"
#include "diagnostic/vf_LeakChecked.h"
#include "diagnostic/vf_SafeBool.h"
#include "diagnostic/vf_Throw.h"

#include "functor/vf_Bind.h"
#include "functor/vf_Function.h"

#include "math/vf_Interval.h"
#include "math/vf_Math.h"
#include "math/vf_MurmurHash.h"

#include "memory/vf_Atomic.h"
#include "memory/vf_CacheLine.h"
#include "memory/vf_MemoryAlignment.h"
#include "memory/vf_NonCopyable.h"
#include "memory/vf_StaticObject.h"

#include "utility/vf_OncePerSecond.h"
#include "utility/vf_PerformedAtExit.h"
#include "utility/vf_SharedSingleton.h"

#include "threads/vf_Mutex.h"
#include "threads/vf_SharedObject.h"
#include "threads/vf_SharedObjectPtr.h"
#include "threads/vf_SpinDelay.h"
#include "threads/vf_SpinLock.h"
#include "threads/vf_Thread.h"
#include "threads/vf_ThreadBase.h"

}

#endif

