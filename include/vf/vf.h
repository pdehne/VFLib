// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

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

// modules
#include "modules/vf_audio/vf_audio.h"
#include "modules/vf_concurrent/vf_concurrent.h"
#include "modules/vf_core/vf_core.h"
#include "modules/vf_db/vf_db.h"
#include "modules/vf_gui/vf_gui.h"

#endif
