// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_ALLOCATORWITHOUTTLS_VFHEADER__
#define __VF_ALLOCATORWITHOUTTLS_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_PageAllocator.h"

// Lock-free Allocator that doesn't use thread local storage.
// This implementation is here in case a problem is found with
// the use of thread local storage.
//
class AllocatorWithoutTls
{
public:
  explicit AllocatorWithoutTls ();
  ~AllocatorWithoutTls ();

  void* allocate (const size_t bytes);
  static void deallocate (void* const p);

private:
  struct Header;

  class Block;

  inline Block* newBlock ();
  static inline void deleteBlock (Block* b);

private:
  Block* volatile m_active;
  static PageAllocator s_pages;
};

#endif
