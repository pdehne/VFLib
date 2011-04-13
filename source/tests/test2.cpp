// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

//
// This file is used for detecting compile problems.
//
//
// Every include file needs to be able to be included on
// its own without requiring any other dependencies with
// the exception of the standard header
//

// This always needs to be here
#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

//
// Change this to the file to check for problems
//

#include "vf/vf_SharedSingleton.h"

END_VF_NAMESPACE

namespace { void dummmy () { } } // Hides a linker warning
