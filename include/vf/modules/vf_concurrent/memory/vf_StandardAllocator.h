// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_STANDARDALLOCATOR_VFHEADER__
#define __VF_STANDARDALLOCATOR_VFHEADER__

// Used for testing
//
class StandardAllocator
{
public:
  inline void* allocate (size_t bytes)
  {
    return malloc (bytes);
  }

  static inline void deallocate (void* const p)
  {
    free (p);
  }
};

#endif
