// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STANDARDHEADER_VFHEADER__
#define __VF_STANDARDHEADER_VFHEADER__

// Handy macro that lets pragma warnings be clicked in the output window
#define VF_STR2_(x) #x
#define VF_STR1_(x) VF_STR2_(x)
#define VF_LOC_ __FILE__ "("VF_STR1_(__LINE__)") : WARNING: "

#include "vf/vf_Config.h"

#if VF_HAVE_JUCE
#include "vf/vf_juce_includes.h"
#define VF_JUCE JUCE_NAMESPACE
#endif

#if VF_HAVE_BOOST
#include "vf/vf_boost_includes.h"
#endif

#include "vf/vf_stdlib_includes.h"

//------------------------------------------------------------------------------

#ifdef VF_NAMESPACE
  #define BEGIN_VF_NAMESPACE namespace VF_NAMESPACE {
  #define END_VF_NAMESPACE   }
#else
  #define BEGIN_VF_NAMESPACE
  #define END_VF_NAMESPACE
#endif

//
// These are always avaialble
//

BEGIN_VF_NAMESPACE

// Borrowed from Juce so we are not dependent on it
template <bool b> struct VfStaticAssert;
template <> struct VfStaticAssert <true>
  { static void static_assert_failed() {} };
#define static_vfassert(expression) \
  VF_NAMESPACE::VfStaticAssert<expression>::static_assert_failed();

#include "vf/vf_String.h"
#include "vf/vf_Debug.h"
#include "vf/vf_Error.h"
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
#endif
END_VF_NAMESPACE

#endif
