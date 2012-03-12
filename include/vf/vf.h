// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_VFHEADER__
#define __VF_VFHEADER__

//
// This is the only header file that should be included by outside applications.
//
// SOURCE AND HEADER FILES IN THE LIBRARY SHOULD NEVER INCLUDE THIS!!!!
//

namespace vf_DummyNamespace {}      // To catch unbalanced braces before here

#define VF_PUBLIC_INCLUDES 1        // Flag to know when we're compiling the lib
#include "vf/vf_StandardHeader.h"   // Minimum requirements

BEGIN_VF_NAMESPACE

#include "vf/vf_includes.h"         // All public headers

END_VF_NAMESPACE

// modules
#include "modules/vf_audio/vf_audio.h"
#include "modules/vf_concurrent/vf_concurrent.h"
#include "modules/vf_db/vf_db.h"

#endif
