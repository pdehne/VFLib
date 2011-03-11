// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_VFHEADER__
#define __VF_VFHEADER__

//
// This is the only header file that should be included by outside applications.
//
// SOURCE AND HEADER FILES IN THE VF LIBRARY SHOULD NEVER INCLUDE THIS!!!!
//

namespace vf_DummyNamespace {}      // To catch unbalanced braces before here

#define VF_PUBLIC_INCLUDES 1        // Flag to know when we're compiling the lib
#include "vf/vf_StandardHeader.h"   // Minimum requirements

#include "vf/vf_ext_includes.h"     // Outside our namespace

BEGIN_VF_NAMESPACE

  #include "vf/vf_public_includes.h"

END_VF_NAMESPACE

#ifdef HAVE_VF_PRIVATE
  #include "vf/vf_private_includes.h"
#endif

#endif
