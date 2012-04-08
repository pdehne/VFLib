// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_THROW_VFHEADER
#define VF_THROW_VFHEADER

#include "vf_Debug.h"

//
// Throw an exception, with the opportunity to get a
// breakpoint with the call stack before the throw.
//

template <class Exception>
inline void Throw (Exception const& e)
{
  Debug::breakPoint ();

  throw e;
}

#endif
