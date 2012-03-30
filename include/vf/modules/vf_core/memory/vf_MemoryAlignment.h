// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_MEMORYALIGNMENT_VFHEADER
#define VF_MEMORYALIGNMENT_VFHEADER

namespace Memory {

// Constants

const int cacheLineAlignBits  = 6; // 64 bytes
const int cacheLineAlignBytes = 1 << cacheLineAlignBits;
const int cacheLineAlignMask  = cacheLineAlignBytes - 1;

const int allocAlignBits  = 3; // 8 bytes
const int allocAlignBytes = 1 << allocAlignBits;
const int allocAlignMask  = allocAlignBytes - 1;

// Returns the number of bytes needed to advance p to the correct alignment
template <typename P>
inline size_t bytesNeededForAlignment (P const* const p)
{
  return (allocAlignBytes - (uintptr_t (p) & allocAlignMask))
         & allocAlignMask;
}

// Returns the number of bytes to make "bytes" an aligned size
inline size_t sizeAdjustedForAlignment (const size_t bytes)
{
  return (bytes + allocAlignMask) & ~allocAlignMask;
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
