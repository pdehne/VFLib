// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_ATOMIC_VFHEADER__
#define __VF_ATOMIC_VFHEADER__

#include "vf/vf_JuceAtomic.h"

namespace Atomic {

#if VF_HAVE_JUCE

using namespace Juce::Atomic;

#else

#error "Missing Atomic"

#endif

}


#endif
