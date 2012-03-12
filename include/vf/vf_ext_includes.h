// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_EXT_INCLUDES_VFHEADER__
#define __VF_EXT_INCLUDES_VFHEADER__

// These are included outside of our namespace.

#if 1
// forward declare
// not the best place for this but convenient.
struct sqlite3;
struct sqlite3_blob;
struct sqlite3_stmt;
#endif

// all the includes for external libraries bundled with vf lib
// But maybe not...this isn't a good idea.

// Commented out because bzlib.h includes <windows.h>
//#include "bzip2/bzlib.h" // ACK THIS BRINGS IN WINDOWS.H!!!

// When this causes an error (compiling on iOs or android?), we
// will need to add some stuff to detect if intrinsics are available
#ifdef _MSC_VER
#include <xmmintrin.h>
#define VF_HAVE_SIMD_INTRINSICS 1
#endif

#endif
