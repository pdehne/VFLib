// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_DEBUG_VFHEADER__
#define __VF_DEBUG_VFHEADER__

#include "vf/vf_String.h"

// Routines for debugging

namespace Debug {

// Returns true if a debugger is attached, for any build.
extern bool isDebuggerAttached ();

// Breaks to the debugger if a debugger is attached.
extern void breakPoint ();

// VF: IS THIS REALLY THE RIGHT PLACE FOR THESE??

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

#if JUCE_MSVC && _DEBUG
extern void setHeapAlwaysCheck (bool bAlwaysCheck);
extern void setHeapDelayedFree (bool bDelayedFree);
extern void setHeapReportLeaks (bool bReportLeaks);
extern void checkHeap ();
#else
inline void setHeapAlwaysCheck (bool) { }
inline void setHeapDelayedFree (bool) { }
inline void setHeapReportLeaks (bool) { }
inline void checkHeap () { }
#endif

}

#endif
