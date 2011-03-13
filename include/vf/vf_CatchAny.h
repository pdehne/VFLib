// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CATCHANY_VFHEADER__
#define __VF_CATCHANY_VFHEADER__

#include "vf/vf_Function.h"

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

extern bool CatchAny (Function f,
                      bool returnFromException = false);

#endif
