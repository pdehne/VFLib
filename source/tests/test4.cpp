// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

// This file is used for detecting compile problems.

//
// Compiles without Juce
//

#define VF_USE_JUCE 0

#include "vf/vf.h"

namespace { void dummmy () { } } // stop linker warning

