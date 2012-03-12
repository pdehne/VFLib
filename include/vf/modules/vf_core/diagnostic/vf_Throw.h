// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_THROW_VFHEADER__
#define __VF_THROW_VFHEADER__

#include "vf/modules/vf_core/diagnostic/vf_Debug.h"

//
// Throw an exception, with the opportunity to get a
// breakpoint with the call stack before the throw.
//

template <class Exception>
inline void Throw (Exception const& e)
{
  Debug::breakPoint ();

#if 1
  boost::throw_exception (e);
#else
  throw e;
#endif
}

#endif
