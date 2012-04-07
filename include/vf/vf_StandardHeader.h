// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_STANDARDHEADER_VFHEADER__
#define __VF_STANDARDHEADER_VFHEADER__

// Handy macro that lets pragma warnings be clicked in the output window
// Usage: #pragma message(VF_LOC_"Advertise here!")
#define VF_STR2_(x) #x
#define VF_STR1_(x) VF_STR2_(x)
#define VF_LOC_ __FILE__ "("VF_STR1_(__LINE__)") : WARNING: "

#include "vf/vf_Config.h"

#include "vf/vf_juce_includes.h"
#define VF_JUCE JUCE_NAMESPACE
using VF_JUCE::int64;
using VF_JUCE::uint64;
using VF_JUCE::uint32;
using VF_JUCE::uint8;

#include "vf/vf_boost_includes.h"

#include "vf/vf_stdlib_includes.h"

#ifdef _MSC_VER
#  pragma warning (disable: 4100) // Unused parameters
#endif

//------------------------------------------------------------------------------

//
// These are always avaialble
//

namespace vf
{

#define vfassert jassert

// Borrowed from Juce so we are not dependent on it
template <bool condition> struct VfStaticAssert;
template <> struct VfStaticAssert <true>
  { static void static_assert_failed () {} };
#define static_vfassert(expression) \
  vf::VfStaticAssert <expression>::static_assert_failed ();

// Stuff here is available without having to include it directly

#include "vf/modules/vf_core/diagnostic/vf_Debug.h"
#include "vf/modules/vf_core/diagnostic/vf_Error.h"
#include "vf/modules/vf_core/diagnostic/vf_LeakChecked.h"
#include "vf/modules/vf_core/memory/vf_Uncopyable.h"
#include "vf/modules/vf_core/diagnostic/vf_SafeBool.h"
#include "vf/modules/vf_core/memory/vf_StaticObject.h"
#include "vf/modules/vf_core/diagnostic/vf_Throw.h"

}

#endif
