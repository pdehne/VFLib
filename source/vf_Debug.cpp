// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

#ifdef _MSC_VER
#include <windows.h>
#endif

BEGIN_VF_NAMESPACE

#include "vf/vf_Debug.h"

namespace Debug {

#if VF_DEBUG

//------------------------------------------------------------------------------
//
// Debug::breakPoint
//
#if VF_HAVE_JUCE && JUCE_DEBUG && defined (juce_breakDebugger)
  void breakPoint () { juce_breakDebugger; }

#elif defined (_MSC_VER)
  #pragma intrinsic (__debugbreak)
  void breakPoint () { __debugbreak(); }

#else
  #pragma message(VF_LOC_"Missing Debug::breakPoint()")
  void breakPoint () { }

#endif

//------------------------------------------------------------------------------
//
// Debug::isDebuggerAttached
//
#if VF_HAVE_JUCE
  bool isDebuggerAttached () { return VF_JUCE::juce_isRunningUnderDebugger (); }

#elif defined (_WINDOWS)
  bool isDebuggerAttached () { return IsDebuggerPresent (); }

#else
  #pragma message(VF_LOC_"Missing Debug::isDebuggerAttached()")
  bool isDebuggerAttached () { return false; }

#endif

//------------------------------------------------------------------------------

#endif

}

END_VF_NAMESPACE
