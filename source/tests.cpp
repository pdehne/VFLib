// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

// This file is used for detecting compile problems.

#define WHICH_TEST 1 /* choose a test */

#if WHICH_TEST == 1

//
// Checks that the public library header compiles
//

#include "src/juce_WithoutMacros.h"
#include "vf/vf.h"

#elif WHICH_TEST == 2

//
// Checks a single include to make sure it compiles
// without requiring anything else included before hand
//

#include "vf/vf_StandardHeader.h" // always needed

BEGIN_VF_NAMESPACE

#include "vf/vf_FPUFlags.h" // put the file to test here

END_VF_NAMESPACE

#else

#endif

// Stops the linker from complaining about a lack of public symbols
namespace { void dummmy () { } }
