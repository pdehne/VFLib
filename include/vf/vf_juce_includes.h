// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCE_INCLUDES_VFHEADER__
#define __VF_JUCE_INCLUDES_VFHEADER__

// Includes Juce for you if its not already included

#ifndef __JUCE_JUCEHEADER__

// If the MSVC debug heap headers were included, disable
// the macros during the juce include since they conflict.
#ifdef _CRTDBG_MAP_ALLOC
  #pragma push_macro("calloc")
  #pragma push_macro("malloc")
  #pragma push_macro("realloc")
  #pragma push_macro("free")
  #undef calloc
  #undef malloc
  #undef realloc
  #undef free
#endif

// If you want Juce with macros, or with a different
// configuration, just include it before you include vf.h
#include "src/juce_WithoutMacros.h"

#ifdef _CRTDBG_MAP_ALLOC
  #pragma pop_macro("calloc")
  #pragma pop_macro("malloc")
  #pragma pop_macro("realloc")
  #pragma pop_macro("free")
#endif

#endif

#endif
