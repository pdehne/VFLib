// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREAD_VFHEADER__
#define __VF_THREAD_VFHEADER__

#include "vf/modules/vf_core/threads/vf_BoostThread.h"
#include "vf/modules/vf_core/threads/vf_JuceThread.h"

#if 1
typedef JuceThread Thread;
namespace CurrentThread = CurrentJuceThread;
#else
typedef BoostThread Thread;
namespace CurrentThread = CurrentBoostThread;
#endif

#endif
