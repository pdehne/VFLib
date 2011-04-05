// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

//#include "vf/vf_StandardHeader.h"

// If the MSVC debug heap headers were included, disable
// the macros during the juce include since they conflict.
#ifdef _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #include <stdlib.h>
  #include <malloc.h>

  #pragma push_macro("calloc")
  #pragma push_macro("malloc")
  #pragma push_macro("realloc")
  #pragma push_macro("free")
  #undef calloc
  #undef malloc
  #undef realloc
  #undef free
#endif

#if ! VF_PUBLIC_INCLUDES
#  ifndef DONT_SET_USING_JUCE_NAMESPACE
#  define DONT_SET_USING_JUCE_NAMESPACE
#  endif
#endif

// If you want Juce with macros, or with a different
// configuration, just include it before you include vf.h
#ifdef malloc
#error die
#endif

#include "src/juce_WithoutMacros.h"

#ifdef _CRTDBG_MAP_ALLOC
  #pragma pop_macro("calloc")
  #pragma pop_macro("malloc")
  #pragma pop_macro("realloc")
  #pragma pop_macro("free")
#endif
