// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_FIFOFREESTOREWITHOUTTLS_VFHEADER
#define VF_FIFOFREESTOREWITHOUTTLS_VFHEADER

#ifndef DOXYGEN

#include "vf_GlobalPagedFreeStore.h"

// Lock-free Allocator that doesn't use thread local storage.
// This implementation is here in case a problem is found with
// the use of thread local storage.
//
class FifoFreeStoreWithoutTLS
{
public:
  explicit FifoFreeStoreWithoutTLS ();
  ~FifoFreeStoreWithoutTLS ();

  void* allocate (const size_t bytes);
  static void deallocate (void* const p);

private:
  typedef GlobalPagedFreeStore PagedFreeStoreType;

  struct Header;

  class Block;

  inline Block* newBlock ();
  static inline void deleteBlock (Block* b);

private:
  Block* volatile m_active;
  PagedFreeStoreType::Ptr m_pages;
};

#endif

#endif
