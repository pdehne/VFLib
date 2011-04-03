// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADINDEX_VFHEADER__
#define __VF_THREADINDEX_VFHEADER__

namespace ThreadIndex {

//
// NOT IMPLEMENTED
// THIS MIGHT NOT BE POSSIBLE WITHOUT C++0x
//

// Returns a small non-negative integer that identifies the calling thread.
// The values returned by this function are guaranteed to be as small
// as possible. These are used in some optimizations that use arrays
// of per-thread data.
//

extern int getCurrent ();

}

#endif

