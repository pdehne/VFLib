// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_CATCHANY_VFHEADER__
#define __VF_CATCHANY_VFHEADER__

#include "vf/modules/vf_core/functor/vf_Function.h"

//
// Executes the function and catches any exception.
// In addition to C++ exceptions, this will also catch
// any platform-specific exceptions. For example, SEH
// (Structured Exception Handling) on Windows, or POSIX
// signals if they are available.
//
// If returnFromException is false then a framework
// specific unhandled exception handler will be called.
// Otherwise, this function will return true if it
// catches something or else false.
//
// The return value approach is useful for detecting
// when outside code fails (for example, a VST plugin),
// and disabling its future use for example.
//

extern bool CatchAny (Function <void (void)> f,
                      bool returnFromException = false);

#endif
