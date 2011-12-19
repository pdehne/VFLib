// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

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
