// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_DEBUG_VFHEADER__
#define __VF_DEBUG_VFHEADER__

#include "vf/vf_String.h"

// Routines for debugging

namespace Debug {

#if VF_DEBUG

extern void breakPoint ();
extern bool isDebuggerAttached ();

#else

inline void breakPoint () { }
inline bool isDebuggerAttached () { return false; }

#endif

// Return only the filename portion of sourceFileName
// This hides the programmer's directory structure from end-users.
const String getFileNameFromPath (const char *sourceFileName);

// Convert a String that may contain double quotes and newlines
// into a String with double quotes escaped as \" and each
// line as a separate quoted command line argument.
String stringToCommandLine (const String& s);

// Convert a quoted and escaped command line back into a String
// that can contain newlines and double quotes.
String commandLineToString (const String& commandLine);

}

#endif
