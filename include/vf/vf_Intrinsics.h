// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_INTRINSICS_VFHEADER__
#define __VF_INTRINSICS_VFHEADER__

#include "vf/vf_Thread.h"

namespace Intrinsic {

//
// SIMD
//

#if VF_HAVE_SIMD_INTRINSICS

template <int Count>
inline void mm_pause()
{
  for (int i = Count; --i >= 0;)
    _mm_pause();
}

template <>
inline void mm_pause <1> ()
{
  _mm_pause();
}

template <>
inline void mm_pause <50> ()
{
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
  _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause(); _mm_pause();
}

#else

//
// Default
//

template <int Count>
inline void mm_pause()
{
  CurrentThread::yield();
}

#endif

}

#endif
