// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

// Generates precompiled header file

//#define VF_PUBLIC_INCLUDES 0
#include "vf/vf_StandardHeader.h"

// Stops the linker from complaining about a lack of public symbols
namespace { void dummmy () { } }
