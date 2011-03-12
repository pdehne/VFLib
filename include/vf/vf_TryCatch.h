// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_TRYCATCH_VFHEADER__
#define __VF_TRYCATCH_VFHEADER__

#include "vf/vf_Function.h"

//
// Call the Function wrapped in a try block.
// If the platform has additional structured exception handling
// or POSIX signals, these will be caught as well.
//

extern void TryCatch (Function f);

#endif
