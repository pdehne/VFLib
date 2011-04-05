// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_PAGEALLOCATOR_VFHEADER__
#define __VF_PAGEALLOCATOR_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_CacheLine.h"
#include "vf/vf_LockFreeStack.h"
#include "vf/vf_OncePerSecond.h"
#include "vf/vf_SharedSingleton.h"

//
// Wait-free allocator for fixed size pages with these properties:
//
// - Deallocated pages are re-used, after a delay to prevent ABA
//

class PageAllocator : private OncePerSecond
{
public:
  explicit PageAllocator (const size_t pageBytes = 128);
  ~PageAllocator ();

  // The available bytes per page is a little bit less
  // than requested in the constructor, due to overhead.
  //
  inline size_t getPageBytes () const { return m_pageBytesAvailable; }

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
    CacheLine::Padded <List> fresh;
    CacheLine::Padded <List> garbage;
  };

  static inline void* fromPage (Page* const p);
  static inline Page* toPage (void* const p);

  void dispose (List& list);
  void dispose (Pool& pool);

private:
  const size_t m_pageBytes;
  const size_t m_pageBytesAvailable;
  CacheLine::Aligned <Pool> m_pool1;  // pair of pools
  CacheLine::Aligned <Pool> m_pool2;
  Pool* volatile m_cold;            // pool which is cooling down
  Pool* volatile m_hot;             // pool we are currently using

  // TODO: MAKE THIS STATIC SOMEHOW??
  Atomic::Counter m_newPagesLeft; // limit of system allocations

#if 1
  int m_swaps;
  Atomic::Counter m_total;
  Atomic::Counter m_used;
#endif
};

//------------------------------------------------------------------------------

class GlobalPageAllocator : public SharedSingleton <GlobalPageAllocator>
{
private:
  GlobalPageAllocator ();
  ~GlobalPageAllocator ();

public:
  inline void* allocate ()
  {
    return m_allocator.allocate ();
  }

  inline size_t getPageBytes ()
  {
    return m_allocator.getPageBytes ();
  }

  static inline void deallocate (void* const p)
  {
    PageAllocator::deallocate (p);
  }

private:
  friend class SharedSingleton <GlobalPageAllocator>;
  static GlobalPageAllocator* createInstance ();

private:
  PageAllocator m_allocator;
};

#endif
