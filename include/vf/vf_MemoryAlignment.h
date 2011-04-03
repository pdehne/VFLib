// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MEMORYALIGNMENT_VFHEADER__
#define __VF_MEMORYALIGNMENT_VFHEADER__

namespace Memory {

// Constants

const int cacheLineBytes = 64;

const int allocationAlignmentBytes = 8;

// Returns the number of bytes needed to advance p to the correct alignment
template <typename P>
inline size_t bytesNeededForAlignment (P const* const p,
                                       const size_t bytes = allocationAlignmentBytes)
{
  const uintptr_t v = reinterpret_cast <uintptr_t> (p);
  const uintptr_t n = ((v + bytes - 1) / bytes) * bytes;
  return n - v;
}

// Returns the number of bytes to make size an aligned size
inline size_t sizeAdjustedForAlignment (const size_t size,
                                        const size_t bytes = allocationAlignmentBytes )
{
  return ((size + (bytes - 1)) / bytes) * bytes;
}

// Returns a pointer with alignment added.
template <typename P>
inline P* pointerAdjustedForAlignment (P* const p,
                                       const size_t bytes = allocationAlignmentBytes)
{
  return reinterpret_cast <P*> (reinterpret_cast <char*> (p) + bytesNeededForAlignment (p, bytes));
}

}

#endif
