// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADS_VFHEADER__
#define __VF_THREADS_VFHEADER__

namespace Threads {

//------------------------------------------------------------------------------
//
// Boost
//

#if VF_HAVE_BOOST

typedef boost::thread::id id;

inline id getCurrent ()
{
  return boost::this_thread::get_id ();
}

#define VF_HAVE_THREADS 1

//------------------------------------------------------------------------------
//
// (nothing)
//

#else
  #pragma message(VF_LOC_"Missing namespace Threads")

#define VF_HAVE_THREADS 0

#endif

}

#endif

