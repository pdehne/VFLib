// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_ALLOCATOR_VFHEADER__
#define __VF_ALLOCATOR_VFHEADER__

#include "vf/vf_List.h"
#include "vf/vf_PageAllocator.h"

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
  explicit Allocator (const size_t pageBytes = 0);
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

  static PageAllocator s_pages;
};

class GlobalAllocator
{
public:
  inline void* allocate (size_t bytes)
  {
    return s_allocator.allocate (bytes);
  }

  static inline void deallocate (void* const p)
  {
    Allocator::deallocate (p);
  }

private:
  static Allocator s_allocator;
};

//
// LEGACY
//
enum
{
  //
  // This number should be as small as possible
  // without generating compile time assertions.
  //
  globalFixedAllocatorBlockSize = 96
};

class GlobalFixedAllocator
{
public:
  template <int Bytes>
  void* allocate ()
  {
    return s_allocator.allocate (Bytes);
  }

  void deallocate (void* p)
  {
    s_allocator.deallocate (p);
  }

private:
  static PageAllocator s_allocator;
};

#if 1

template <class C>
struct globalAlloc
{
  static C* New ()
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C;
  }

  template <class T1>
  static C* New (const T1& t1)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1);
  }

  template <class T1, class T2>
  static C* New (const T1& t1, const T2& t2)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1, t2);
  }

  template <class T1, class T2, class T3>
  static C* New (const T1& t1, const T2& t2, const T3& t3)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1, t2, t3);
  }

  template <class T1, class T2, class T3, class T4>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1, t2, t3, t4);
  }

  template <class T1, class T2, class T3, class T4, class T5>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
                 const T5& t5, const T6& t6)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5, t6);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
                 const T5& t5, const T6& t6, const T7& t7)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5, t6, t7);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
                 const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  {
    static_vfassert (sizeof (C) <= globalFixedAllocatorBlockSize);
    return new (GlobalFixedAllocator().allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5, t6, t7, t8);
  }
};

template <class C>
void globalDelete (C* c)
{
  c->~C();
  GlobalFixedAllocator().deallocate (c);
}

#else

template <class C>
struct globalAlloc
{
  static C* New ()
  {
    return new (GlobalAllocator().allocate (sizeof (C))) C;
  }

  template <class T1>
  static C* New (const T1& t1)
  {
    return new (GlobalAllocator().allocate (sizeof (C))) C (t1);
  }

  template <class T1, class T2>
  static C* New (const T1& t1, const T2& t2)
  {
    return new (GlobalAllocator().allocate (sizeof (C))) C (t1, t2);
  }
};

template <class C>
void globalDelete (C* c)
{
  c->~C();
  Allocator::deallocate (c);
}

#endif

#endif
