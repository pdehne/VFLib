// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CORE_VFHEADER
#define VF_CORE_VFHEADER

#include "vf_StandardHeader.h"

namespace vf
{

#include "memory/vf_Uncopyable.h"

#include "containers/vf_List.h"

#include "diagnostic/vf_CatchAny.h"
#include "diagnostic/vf_Debug.h"
#include "diagnostic/vf_Error.h"
#include "diagnostic/vf_FPUFlags.h"
#include "diagnostic/vf_LeakChecked.h"
#include "diagnostic/vf_SafeBool.h"
#include "diagnostic/vf_Throw.h"

#include "events/vf_OncePerSecond.h"
#include "events/vf_PerformedAtExit.h"

#include "functor/vf_Bind.h"
#include "functor/vf_Function.h"

#include "math/vf_Interval.h"
#include "math/vf_Math.h"
#include "math/vf_MurmurHash.h"

#include "memory/vf_AtomicCounter.h"
#include "memory/vf_AtomicFlag.h"
#include "memory/vf_AtomicPointer.h"
#include "memory/vf_AtomicState.h"

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parmaeter
#pragma warning (disable: 4355) // 'this' used in base member
#endif
#include "memory/vf_CacheLine.h"
#if JUCE_MSVC
#pragma warning (pop)
#endif

#include "memory/vf_MemoryAlignment.h"
#include "memory/vf_ReferenceCountedSingleton.h"
#include "memory/vf_StaticObject.h"

#include "threads/vf_SpinDelay.h"
#include "threads/vf_InterruptibleThread.h"

}

#endif

