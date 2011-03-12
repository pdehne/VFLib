// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CONFIG_VFHEADER__
#define __VF_CONFIG_VFHEADER__

//
// This is the master settings file controls features of the library
//

// used to control the namespace
#ifndef VF_NAMESPACE
#define VF_NAMESPACE vf
#endif

//
// This will enable features that require Juce. If juce is
// included before vf.h, the flag will automatically be set
// to 1 if it hasn't already been set.
//
#ifndef VF_HAVE_JUCE
#  ifdef __JUCE_JUCEHEADER__
#    define VF_HAVE_JUCE 1
#  else
#    define VF_HAVE_JUCE 1 // set this to 0 if you dont have Juce
#  endif
#endif

//
// This will enable features that require boost
//
#ifndef VF_HAVE_BOOST
#define VF_HAVE_BOOST 1
#endif

//
// Turns vflib specific debugging support on or off
//
#ifndef VF_DEBUG
  #if VF_HAVE_JUCE
    #define VF_DEBUG JUCE_DEBUG // get the flag from Juce
  #else
    #ifndef NDEBUG              // use standard macro
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
  #if VF_HAVE_JUCE
    #define VF_CHECK_LEAKS JUCE_CHECK_MEMORY_LEAKS
  #else
    #define VF_CHECK_LEAKS VF_DEBUG
  #endif
#endif

#endif
