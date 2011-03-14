// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADS_VFHEADER__
#define __VF_THREADS_VFHEADER__

#include "vf/vf_BoostThread.h"
#include "vf/vf_JuceThread.h"

#if 1 // Juce first
#if VF_HAVE_JUCE
typedef Juce::Thread Thread;
namespace CurrentThread = Juce::CurrentThread;
#elif VF_HAVE_BOOST
typedef Boost::Thread Thread;
namespace CurrentThread = Boost::CurrentThread;
#else
  #pragma message(VF_LOC_"Missing CurrentThread")
#endif

#else // Boost first

#if VF_HAVE_BOOST
typedef Boost::Thread Thread;
namespace CurrentThread = Boost::CurrentThread;
#elif VF_HAVE_JUCE
typedef Juce::Thread Thread;
namespace CurrentThread = Juce::CurrentThread;
#else
  #pragma message(VF_LOC_"Missing CurrentThread")
#endif

#endif

#endif

