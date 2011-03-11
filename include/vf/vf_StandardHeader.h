// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STANDARDHEADER_VFHEADER__
#define __VF_STANDARDHEADER_VFHEADER__

// Handy macro that lets pragma warnings be clicked in the output window
#define VF_STR2_(x) #x
#define VF_STR1_(x) VF_STR2_(x)
#define VF_LOC_ __FILE__ "("VF_STR1_(__LINE__)") : WARNING: "
// Usage:
// #pragma message(VF_LOC_"Description of problem")

#include "vf/vf_Config.h"

//------------------------------------------------------------------------------
//
// Juce
//

#if VF_USE_JUCE
# if VF_PUBLIC_INCLUDES
#  ifndef __JUCE_JUCEHEADER__
#   error Must include juce first
#  endif
# else
#  ifdef _CRTDBG_MAP_ALLOC
#   pragma push_macro("calloc")
#   pragma push_macro("malloc")
#   pragma push_macro("realloc")
#   pragma push_macro("free")
#   undef calloc
#   undef malloc
#   undef realloc
#   undef free
#  endif
#  include "src/juce_WithoutMacros.h"
#  ifdef _CRTDBG_MAP_ALLOC
#   pragma pop_macro("calloc")
#   pragma pop_macro("malloc")
#   pragma pop_macro("realloc")
#   pragma pop_macro("free")
#  endif
# endif
# define VF_JUCE JUCE_NAMESPACE
#endif

//------------------------------------------------------------------------------
//
// Boost
//
#if VF_USE_BOOST
# include "vf/vf_boost_includes.h"
#endif

//------------------------------------------------------------------------------
//
// Standard C/C++ Libraries
//

#include "vf/vf_stdlib_includes.h"

//------------------------------------------------------------------------------

#ifdef VF_NAMESPACE
  #define BEGIN_VF_NAMESPACE    namespace VF_NAMESPACE {
  #define END_VF_NAMESPACE      }
#else
  #define BEGIN_VF_NAMESPACE
  #define END_VF_NAMESPACE
#endif

//
// These are always avaialble
//

BEGIN_VF_NAMESPACE

#include "vf/vf_Throw.h"
#include "vf/vf_NonCopyable.h"
#include "vf/vf_LeakChecked.h"

END_VF_NAMESPACE

// EVERYTHING BELOW HERE IS A PIECE OF SHIT!!!!
// Basic vf includes needed by almost everyone
// TODO: REMOVE AS MUCH OF THIS AS POSSIBLE!!!!
BEGIN_VF_NAMESPACE
// The order is important here
#ifdef VF_HAVE_PRIVATE_INCLUDES
  // NONE OF THESE SHOULD INCLUDE OTHER FILES
  #include "vf/Assure.h"
  #include "vf/Types.h"
  #include "vf/Str.h"
  #include "vf/Err.h"
  #include "vf/Error.h"
#endif
END_VF_NAMESPACE

#endif
