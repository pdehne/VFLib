// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf/vf_StandardHeader.h"

#include "vf_core.h"

#if JUCE_MSVC && _DEBUG
#include <crtdbg.h>
#endif

namespace vf
{

//#include "diagnostic/vf_CatchAny.cpp"
//#include "diagnostic/vf_Debug.cpp"

#include "diagnostic/vf_Debug.cpp"
#include "diagnostic/vf_Error.cpp"
#include "diagnostic/vf_FPUFlags.cpp"
#include "diagnostic/vf_LeakChecked.cpp"
#include "math/vf_MurmurHash.cpp"
#include "utility/vf_OncePerSecond.cpp"
#include "utility/vf_PerformedAtExit.cpp"
#include "threads/vf_BoostThread.cpp"
#include "threads/vf_JuceThread.cpp"

#if JUCE_WINDOWS
#include "native/vf_win32_FPUFlags.cpp"

#else
#include "native/vf_posix_FPUFlags.cpp"

#endif

}
