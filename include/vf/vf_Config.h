// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CONFIG_VFHEADER__
#define __VF_CONFIG_VFHEADER__

//
// This is the master settings file controls features of the library
//

#ifndef VF_NAMESPACE
#define VF_NAMESPACE vf
#endif

//
// This will enable features that require Juce
//
#ifndef VF_USE_JUCE
#define VF_USE_JUCE 1
#endif

//
// This will enable features that require boost
//
#ifndef VF_USE_BOOST
#define VF_USE_BOOST 1
#endif

//
// Turns vflib specific debugging support on or off
//
#ifndef VF_DEBUG
  #if VF_USE_JUCE
    #define VF_DEBUG JUCE_DEBUG
  #else
    #ifndef NDEBUG
      #define VF_DEBUG 1
    #else
      #define VF_DEBUG 0
    #endif
  #endif
#endif

//
// Enables leak checking
//
#ifndef VF_CHECK_LEAKS
  #if VF_USE_JUCE
    #define VF_CHECK_LEAKS JUCE_CHECK_MEMORY_LEAKS
  #else
    #define VF_CHECK_LEAKS VF_DEBUG
  #endif
#endif

#endif
