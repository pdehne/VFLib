// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_ALLOCATOR_VFHEADER__
#define __VF_ALLOCATOR_VFHEADER__

#include "vf/modules/vf_concurrent/memory/vf_PageAllocator.h"

#define LOCKFREE_ALLOCATOR_LOGGING 0

// Lock-free allocator for small variably sized memory blocks.
//
// - Any thread may allocate and free concurrently.
//
// - Freed memory is re-used in future allocations after a certain amount
//   of time has passed, in order to prevent the ABA problem for callers.
//
// - When there are no free blocks available, the algorithm becomes a blocking
//   algorithm since we request memory from the standard library.
//
class Allocator
{
public:
  Allocator ();
  ~Allocator ();

  void* allocate (const size_t bytes);
  static void deallocate (void* const p);

private:
  struct Header;

  class Page;

  inline Page* newPage ();
  static inline void deletePage (Page* page);

private:
  class PerThreadData;
  boost::thread_specific_ptr <PerThreadData> m_tsp;

  GlobalPageAllocator::Ptr m_pages;
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
    Allocator::deallocate (p);
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
  // WTF?
  friend class ReferenceCountedSingleton <GlobalAllocator <Tag> >;

  static GlobalAllocator* createInstance ()
  {
    return new GlobalAllocator;
  }

private:
  Allocator m_allocator;
};

#endif
