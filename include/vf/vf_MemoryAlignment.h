// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MEMORYALIGNMENT_VFHEADER__
#define __VF_MEMORYALIGNMENT_VFHEADER__

namespace Memory {

// Constants

const int cacheLineAlignBits  = 6; // 64 bytes
const int cacheLineAlignBytes = 1 << cacheLineAlignBits;
const int cacheLineAlignMask  = cacheLineAlignBytes - 1;

const int allocLineAlignBits  = 3; // 8 bytes
const int allocLineAlignBytes = 1 << allocLineAlignBits;
const int allocLineAlignMask  = allocLineAlignBytes - 1;

// Returns the number of bytes needed to advance p to the correct alignment
template <typename P>
inline size_t bytesNeededForAlignment (P const* const p)
{
  return (allocLineAlignBytes - (uintptr_t (p) & allocLineAlignMask))
         & allocLineAlignMask;
}

// Returns the number of bytes to make size an aligned size
inline size_t sizeAdjustedForAlignment (const size_t bytes)
{
  return (bytes + allocLineAlignMask) & ~allocLineAlignMask;
}

// Returns a pointer with alignment added.
template <typename P>
inline P* pointerAdjustedForAlignment (P* const p)
{
  return reinterpret_cast <P*> (reinterpret_cast <char*> (p) +
                                bytesNeededForAlignment (p));
}

}

#endif
