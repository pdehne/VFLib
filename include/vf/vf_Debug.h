// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_DEBUG_VFHEADER__
#define __VF_DEBUG_VFHEADER__

// Routines for debugging

namespace Debug {

#if VF_DEBUG

extern void breakPoint ();
extern bool isDebuggerAttached ();

#else

inline void breakPoint () { }
inline bool isDebuggerAttached () { return false; }

#endif

}

#endif
