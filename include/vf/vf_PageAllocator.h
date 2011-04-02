// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_PAGEALLOCATOR_VFHEADER__
#define __VF_PAGEALLOCATOR_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeStack.h"
#include "vf/vf_OncePerSecond.h"

//
// Wait-free allocator for fixed size pages.
//

class PageAllocator : private OncePerSecond
{
public:
  explicit PageAllocator (const size_t pageBytes = 128);
  ~PageAllocator ();

  // The available bytes per page is a little bit less
  // than requested in the constructor, due to overhead.
  //
  inline size_t getPageBytes () const { return m_pageBytes; }

  inline void* allocate (const size_t bytes)
  {
    if (bytes > m_pageBytes)
      Throw (Error().fail (__FILE__, __LINE__, "the size is too large"));

    return allocate ();
  }

  void* allocate ();
  static void deallocate (void* const p);

private:
  void* newPage ();
  void doOncePerSecond ();

private:
  struct Page;
  typedef LockFree::Stack <Page> List;

  struct Pool
  {
    List fresh;
    List garbage;
  };

  static inline void* fromPage (Page* const p);
  static inline Page* toPage (void* const p);

  void free (List& list);
  void free (Pool& pool);

private:
  const size_t m_pageBytes;
  Pool m_pool[2];           // pair of pools
  Pool* volatile m_cold;    // pool which is cooling down
  Pool* volatile m_hot;     // pool we are currently using

  // TODO: MAKE THIS STATIC SOMEHOW??
  Atomic::Counter m_newPagesLeft; // limit of system allocations

#if 1
  int m_swaps;
  Atomic::Counter m_total;
  Atomic::Counter m_used;
#endif
};


class GlobalPageAllocator
{
public:
  inline void* allocate (size_t bytes)
  {
    return s_allocator.allocate (bytes);
  }

  static inline void deallocate (void* const p)
  {
    PageAllocator::deallocate (p);
  }

private:
  static PageAllocator s_allocator;
};

#endif
