// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_STANDARDHEADER_VFHEADER__
#define __VF_STANDARDHEADER_VFHEADER__

// Handy macro that lets pragma warnings be clicked in the output window
// Usage: #pragma message(VF_LOC_"Advertise here!")
#define VF_STR2_(x) #x
#define VF_STR1_(x) VF_STR2_(x)
#define VF_LOC_ __FILE__ "("VF_STR1_(__LINE__)") : WARNING: "

// Turns vflib specific debugging support on or off
//
#ifndef VF_DEBUG
#define VF_DEBUG JUCE_DEBUG // get the flag from Juce
#endif

// Enables leak checking
//
#ifndef VF_CHECK_LEAKS
#define VF_CHECK_LEAKS JUCE_CHECK_MEMORY_LEAKS
#endif

// Includes Juce for you if its not already included

#ifndef __JUCE_JUCEHEADER__

#ifdef _CRTDBG_MAP_ALLOC
#error "MSVC C Runtime Debug Macros not supported"
#endif

#include "AppConfig.h"

// If the MSVC debug heap headers were included, disable
// the macros during the juce include since they conflict.
#ifdef _CRTDBG_MAP_ALLOC
  #include <crtdbg.h>
  #include <stdlib.h>
  #include <malloc.h>

  #pragma push_macro("calloc")
  #pragma push_macro("free")
  #pragma push_macro("malloc")
  #pragma push_macro("realloc")
  #pragma push_macro("_recalloc")
  #pragma push_macro("_aligned_free")
  #pragma push_macro("_aligned_malloc")
  #pragma push_macro("_aligned_offset_malloc")
  #pragma push_macro("_aligned_realloc")
  #pragma push_macro("_aligned_recalloc")
  #pragma push_macro("_aligned_offset_realloc")
  #pragma push_macro("_aligned_offset_recalloc")
  #pragma push_macro("_aligned_msize")

  #undef calloc
  #undef free
  #undef malloc
  #undef realloc
  #undef _recalloc
  #undef _aligned_free
  #undef _aligned_malloc
  #undef _aligned_offset_malloc
  #undef _aligned_realloc
  #undef _aligned_recalloc
  #undef _aligned_offset_realloc
  #undef _aligned_offset_recalloc
  #undef _aligned_msize
#endif

#include "modules/juce_core/juce_core.h"
#include "modules/juce_gui_basics/juce_gui_basics.h"
#include "modules/juce_data_structures/juce_data_structures.h"
#include "modules/juce_events/juce_events.h"
#include "modules/juce_graphics/juce_graphics.h"
#include "modules/juce_video/juce_video.h"
#include "modules/juce_opengl/juce_opengl.h"
#include "modules/juce_audio_basics/juce_audio_basics.h"
#include "modules/juce_audio_formats/juce_audio_formats.h"
#include "modules/juce_audio_processors/juce_audio_processors.h"
#include "modules/juce_audio_devices/juce_audio_devices.h"
#include "modules/juce_cryptography/juce_cryptography.h"
#include "modules/juce_gui_extra/juce_gui_extra.h"
#include "modules/juce_audio_utils/juce_audio_utils.h"

// YIKES What is this doing here?? REMOVE ASAP
using namespace juce;

#ifdef _CRTDBG_MAP_ALLOC
  #pragma pop_macro("_aligned_msize")
  #pragma pop_macro("_aligned_offset_recalloc")
  #pragma pop_macro("_aligned_offset_realloc")
  #pragma pop_macro("_aligned_recalloc")
  #pragma pop_macro("_aligned_realloc")
  #pragma pop_macro("_aligned_offset_malloc")
  #pragma pop_macro("_aligned_malloc")
  #pragma pop_macro("_aligned_free")
  #pragma pop_macro("_recalloc")
  #pragma pop_macro("realloc")
  #pragma pop_macro("malloc")
  #pragma pop_macro("free")
  #pragma pop_macro("calloc")
#endif

#endif

#define VF_JUCE juce
using VF_JUCE::int64;
using VF_JUCE::uint64;
using VF_JUCE::uint32;
using VF_JUCE::uint8;

#ifndef VF_HAVE_BOOST
#define VF_HAVE_BOOST 0
#endif

#if VF_HAVE_BOOST
#include <boost/thread/tss.hpp>
#endif

#if JUCE_MSVC
#include <crtdbg.h>
#endif

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <exception>
#include <functional>
#include <istream>
#include <iterator>
#include <limits>
#include <list>
#include <map>
#include <new>
#include <numeric>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <locale.h>
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace vf
{

//using namespace juce;

#define vfassert jassert

// Borrowed from Juce so we are not dependent on it
template <bool condition> struct VfStaticAssert;
template <> struct VfStaticAssert <true>
  { static void static_assert_failed () {} };
#define static_vfassert(expression) \
  vf::VfStaticAssert <expression>::static_assert_failed ();

}

#endif
