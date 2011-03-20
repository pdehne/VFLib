// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_EXT_INCLUDES_VFHEADER__
#define __VF_EXT_INCLUDES_VFHEADER__

// These are included outside of our namespace.

// all the includes for external libraries bundled with vf lib

// Commented out because bzlib.h includes <windows.h>
//#include "bzip2/bzlib.h" // ACK THIS BRINGS IN WINDOWS.H!!!

#ifdef VF_HAVE_PRIVATE_INCLUDES
#include "vf/lua/luabridge.hpp"
#endif

#endif

