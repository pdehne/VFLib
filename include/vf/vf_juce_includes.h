// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_JUCE_INCLUDES_VFHEADER__
#define __VF_JUCE_INCLUDES_VFHEADER__

// Includes Juce for you if its not already included

#ifndef __JUCE_JUCEHEADER__

#ifdef _CRTDBG_MAP_ALLOC
#error "MSVC C Runtime Debug Macros not supported"
#endif

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

#endif
