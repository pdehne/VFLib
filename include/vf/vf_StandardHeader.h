// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_STANDARDHEADER_VFHEADER__
#define __VF_STANDARDHEADER_VFHEADER__

// Handy macro that lets pragma warnings be clicked in the output window
// Usage: #pragma message(VF_LOC_"Advertise here!")
#define VF_STR2_(x) #x
#define VF_STR1_(x) VF_STR2_(x)
#define VF_LOC_ __FILE__ "("VF_STR1_(__LINE__)") : WARNING: "

#include "vf/vf_Config.h"

#if VF_HAVE_JUCE
#  include "vf/vf_juce_includes.h"
#  define VF_JUCE JUCE_NAMESPACE
using VF_JUCE::int64;
using VF_JUCE::uint64;
using VF_JUCE::uint32;
using VF_JUCE::uint8;
#endif

#if VF_HAVE_BOOST
#include "vf/vf_boost_includes.h"
#endif

#include "vf/vf_stdlib_includes.h"

#include "vf/vf_ext_includes.h"     // Outside our namespace

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

#if VF_USE_JUCE
  #define vfassert jassert
#else
  #define vfassert assert
#endif

// Borrowed from Juce so we are not dependent on it
template <bool condition> struct VfStaticAssert;
template <> struct VfStaticAssert <true>
  { static void static_assert_failed () {} };
#define static_vfassert(expression) \
  VF_NAMESPACE::VfStaticAssert <expression>::static_assert_failed ();

// This assertion goes into every build and causes an exception
#ifndef fatal_vfassert
#define fatal_vfassert(expression) if (!(expression)) \
  VF_NAMESPACE::Throw (Error().fail (__FILE__, __LINE__, #expression, VF_NAMESPACE::Error::assertFailed))
#endif

// Stuff here is available without having to include it directly
#include "vf/vf_Debug.h"
#include "vf/vf_Error.h"
#include "vf/vf_NonCopyable.h"
#include "vf/vf_SafeBool.h"
#include "vf/vf_String.h"
#include "vf/vf_Throw.h"

END_VF_NAMESPACE

#endif
