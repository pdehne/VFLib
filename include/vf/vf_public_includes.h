// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_INCLUDES_VFHEADER__
#define __VF_INCLUDES_VFHEADER__

#ifndef __VF_AUDIOSAMPLEBUFFERARRAY_VFHEADER__
#include "vf/vf_AudioSampleBufferArray.h"
#endif

#ifndef __VF_BIND_VFHEADER__
#include "vf/vf_Bind.h"
#endif

#ifndef __VF_CATCHANY_VFHEADER__
#include "vf/vf_CatchAny.h"
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

#ifndef __VF_LIST_VFHEADER__
#include "vf/vf_List.h"
#endif

#ifndef __VF_MANUALWORKER_VFHEADER__
#include "vf/vf_ManualWorker.h"
#endif

#ifndef __VF_MUTEX_VFHEADER__
#include "vf/vf_Mutex.h"
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

#ifndef __VF_WORKER_VFHEADER__
#include "vf/vf_Worker.h"
#endif

//
// Boost-specific
//

#if VF_HAVE_BOOST

#ifndef __VF_BOOSTTHREAD_VFHEADER__
#include "vf/vf_BoostThread.h"
#endif

#endif

//
// Juce-specific
//

#if VF_HAVE_JUCE

#ifndef __VF_AUDIOBUFFERPOOL_VFHEADER__
#include "vf/vf_AudioBufferPool.h"
#endif

#ifndef __VF_JUCETHREAD_VFHEADER__
#include "vf/vf_JuceThread.h"
#endif

  // UNFORTUNATELY BOOST IS MISSING ATOMICS, WHICH WE NEED
  #ifndef __VF_SHAREDOBJECT_VFHEADER__
  #include "vf/vf_SharedObject.h"
  #endif

  #ifndef __VF_LISTENERS_VFHEADER__
  #include "vf/vf_Listeners.h"
  #endif

#endif

#endif
