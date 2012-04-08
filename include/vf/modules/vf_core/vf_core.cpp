// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#include "vf_core.h"

#if JUCE_MSVC && _DEBUG
#include <crtdbg.h>
#endif

#if JUCE_MSVC
#pragma warning (push)
#pragma warning (disable: 4100) // unreferenced formal parmaeter
#pragma warning (disable: 4355) // 'this' used in base member
#endif

namespace vf
{

#include "diagnostic/vf_CatchAny.cpp"
#include "diagnostic/vf_Debug.cpp"
#include "diagnostic/vf_Error.cpp"
#include "diagnostic/vf_FPUFlags.cpp"
#include "diagnostic/vf_LeakChecked.cpp"

#include "events/vf_OncePerSecond.cpp"
#include "events/vf_PerformedAtExit.cpp"

#include "math/vf_MurmurHash.cpp"

#include "threads/vf_InterruptibleThread.cpp"

#if JUCE_WINDOWS
#include "native/vf_win32_FPUFlags.cpp"

#else
#include "native/vf_posix_FPUFlags.cpp"

#endif

}

#if JUCE_MSVC
#pragma warning (pop)
#endif
