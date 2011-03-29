// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

//
// This file is used for detecting compile problems.
//
// Checks to make sure everything compiles without juce or boost.
// Without Juce or Boost, significant functionality will
// be missing but it should still be possible to include
// the library and get the framework-independent stuff.
//

// TODO: IMPLEMENT THIS

#define VF_STR2_(x) #x
#define VF_STR1_(x) VF_STR2_(x)
#define VF_LOC_ __FILE__ "("VF_STR1_(__LINE__)") : WARNING: "
#pragma message(VF_LOC_"Missing test")

#if 0

#define VF_HAVE_JUCE 0
#define VF_HAVE_BOOST 0

#include "vf/vf.h"

#endif

namespace { void dummmy () { } } // stop linker warning
