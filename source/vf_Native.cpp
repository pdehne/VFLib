// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Native.h"

namespace Native {

//------------------------------------------------------------------------------

#ifdef _MSC_VER

//
// Microsoft Visual Studio
//

#pragma intrinsic (__debugbreak)
void breakToDebugger ()
{
  __debugbreak();
}

//------------------------------------------------------------------------------

#else

#pragma message(VF_LOC_"Missing platform-specific implementation")

#endif

}

END_VF_NAMESPACE