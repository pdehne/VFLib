// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

// This file is used for detecting compile problems.

//
// Checks a single include to make sure it compiles
// without requiring anything else included before hand
//

#include "vf/vf_StandardHeader.h" // always needed

BEGIN_VF_NAMESPACE

#include "vf/vf_FPUFlags.h" // put the file to test here

END_VF_NAMESPACE

namespace { void dummmy () { } } // hides the linker warning
