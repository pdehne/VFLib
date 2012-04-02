// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_FIFOFREESTOREWITHTLS_VFHEADER
#define VF_FIFOFREESTOREWITHTLS_VFHEADER

#include "vf_GlobalPagedFreeStore.h"

/****
  Lock-free and mostly wait-free memory allocator optimized for FIFO
  style usage patterns.

  It is expected that over time, deallocations will occur in roughly
  the same order as allocations.

  Invariants:

  #1 allocate() and deallocate() are fully concurrent

  #2 The ABA problem is handled automatically

  This implementation uses Thread Local Storage to further improve
  performance. However, it requires boost style thread_specific_ptr.
*/
class FifoFreeStoreWithTLS
{
public:
  FifoFreeStoreWithTLS ();
  ~FifoFreeStoreWithTLS ();

  void* allocate (const size_t bytes);
  static void deallocate (void* const p);

private:
  typedef GlobalPagedFreeStore PagedFreeStoreType;
  struct Header;

  class Page;

  inline Page* newPage ();
  static inline void deletePage (Page* page);

private:
  class PerThreadData;
  boost::thread_specific_ptr <PerThreadData> m_tsp;

  PagedFreeStoreType::Ptr m_pages;
};

//------------------------------------------------------------------------------

// Singleton wrapper for a global allocator.
// Immune to order of construction / initialization problems.
//
template <class Tag>
class GlobalAllocator : public ReferenceCountedSingleton <GlobalAllocator <Tag> >
{
public:
  inline void* allocate (size_t bytes)
  {
    return m_allocator.allocate (bytes);
  }

  static inline void deallocate (void* const p)
  {
    FifoFreeStoreWithTLS::deallocate (p);
  }

  static GlobalAllocator* createInstance () // @implementation
  {
    return new GlobalAllocator;
  }

private:
  GlobalAllocator () : ReferenceCountedSingleton <GlobalAllocator <Tag> >
                        (SingletonLifetime::persistAfterCreation)
  {
  }

  ~GlobalAllocator ()
  {
  }

private:
  FifoFreeStoreWithTLS m_allocator;
};

#endif
