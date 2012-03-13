// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

// Generates precompiled header file

//#define VF_PUBLIC_INCLUDES 0
#include "vf/vf_StandardHeader.h"

// Stops the linker from complaining about a lack of public symbols
namespace { void dummmy () { } }
