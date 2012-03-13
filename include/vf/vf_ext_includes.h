// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_EXT_INCLUDES_VFHEADER__
#define __VF_EXT_INCLUDES_VFHEADER__

// These are included outside of our namespace.

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
