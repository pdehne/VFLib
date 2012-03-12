// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_core/memory/vf_MemoryAlignment.h"
#include "vf/vf_PageAllocator.h"

#define LOG_GC 0

namespace {

// This is the size of a page for GlobalPageAllocator.
//
static const size_t globalPageBytes = 8 * 1024;

// This is the upper limit on the amount of physical memory an instance of the
// allocator will allow. Going over this limit means that consumers cannot keep
// up with producers, and application logic should be re-examined.
//
// TODO: ENFORCE THIS GLOBALLY? MEASURE IN KILOBYTES AND FORCE KILOBYTE PAGE SIZES
#define HARD_LIMIT 1
const size_t hardLimitMegaBytes = 256;

}

/*

Implementation notes

- There are two pools, the 'hot' pool and the 'cold' pool.

- When a new page is needed we pop from the 'fresh' stack of the hot pool.

- When a page is deallocated it is pushed to the 'garbage' stack of the hot pool.

- Every so often, a garbage collection is performed on a separate thread.
  During collection, fresh and garbage are swapped in the cold pool.
  Then, the hot and cold pools are atomically swapped.

*/
//------------------------------------------------------------------------------

struct PageAllocator::Page : Pages::Node, LeakChecked <Page>
{
  explicit Page (PageAllocator* const allocator)
    : m_allocator (*allocator)
  {
  }

  PageAllocator& getAllocator () const
  {
    return m_allocator;
  }

private:
  PageAllocator& m_allocator;
};

inline void* PageAllocator::fromPage (Page* const p)
{
  return reinterpret_cast <char*> (p) +
    Memory::sizeAdjustedForAlignment (sizeof (Page));
}

inline PageAllocator::Page* PageAllocator::toPage (void* const p)
{
  return reinterpret_cast <Page*> (
    (reinterpret_cast <char *> (p) -
      Memory::sizeAdjustedForAlignment (sizeof (Page))));
}

//------------------------------------------------------------------------------

PageAllocator::PageAllocator (const size_t pageBytes)
  : m_pageBytes (pageBytes)
  , m_pageBytesAvailable (pageBytes - Memory::sizeAdjustedForAlignment (sizeof (Page)))
  , m_newPagesLeft ((hardLimitMegaBytes * 1024 * 1024) / m_pageBytes)
#if LOG_GC
  , m_swaps (0)
#endif
{
  m_hot  = m_pool1;
  m_cold = m_pool2;

  startOncePerSecond ();
}

PageAllocator::~PageAllocator ()
{
  endOncePerSecond ();

#if LOG_GC
  vfassert (m_used.is_reset ());
#endif

  dispose (m_pool1);
  dispose (m_pool2);

#if LOG_GC
  vfassert (m_total.is_reset ());
#endif
}

//------------------------------------------------------------------------------

void* PageAllocator::allocate ()
{
  Page* page = m_hot->fresh->pop_front ();

  if (!page)
  {
#if HARD_LIMIT
    const bool exhausted = m_newPagesLeft.release ();
    if (exhausted)
      Throw (Error().fail (__FILE__, __LINE__,
        TRANS("the limit of memory allocations was reached")));
#endif

    page = new (::malloc (m_pageBytes)) Page (this);
    if (!page)
      Throw (Error().fail (__FILE__, __LINE__,
        TRANS("a memory allocation failed")));

#if LOG_GC
    m_total.addref ();
#endif
  }

#if LOG_GC
  m_used.addref ();
#endif

  return fromPage (page);
}

void PageAllocator::deallocate (void* const p)
{
  Page* const page = toPage (p);
  PageAllocator& allocator = page->getAllocator ();

  allocator.m_hot->garbage->push_front (page);

#if LOG_GC
  allocator.m_used.release ();
#endif
}

//
// Perform garbage collection.
//
void PageAllocator::doOncePerSecond ()
{
  // Physically free one page.
  // This will reduce the working set over time after a spike.
  {
    Page* page = m_cold->garbage->pop_front ();
    if (page)
    {
      page->~Page ();
      ::free (page);
      m_newPagesLeft.addref ();
#ifdef LOG_GC
      m_total.release ();
#endif
    }
  }

  m_cold->fresh->swap (m_cold->garbage);

  // Swap atomically with respect to m_hot
  Pool* temp = m_hot;
  m_hot = m_cold; // atomic
  m_cold = temp;

#if LOG_GC
  String s;
  s << "swap " << String (++m_swaps);
  s << " (" << String (m_used.get ()) << "/"
    << String (m_total.get ()) << " of "
    << String (m_newPagesLeft.get ()) << ")";
  VF_JUCE::Logger::outputDebugString (s);
#endif
}

void PageAllocator::dispose (Pages& pages)
{
  for (;;)
  {
    Page* const page = pages.pop_front ();

    if (page)
    {
      page->~Page ();
      ::free (page);

#if LOG_GC
      m_total.release ();
#endif
    }
    else
    {
      break;
    }
  }
}

void PageAllocator::dispose (Pool& pool)
{
  dispose (pool.fresh);
  dispose (pool.garbage);
}

//------------------------------------------------------------------------------

GlobalPageAllocator::GlobalPageAllocator ()
  : SharedSingleton <GlobalPageAllocator> (SingletonLifetime::persistAfterCreation)
  , m_allocator (globalPageBytes)
{
}

GlobalPageAllocator::~GlobalPageAllocator ()
{
}

GlobalPageAllocator* GlobalPageAllocator::createInstance ()
{
  return new GlobalPageAllocator;
}

END_VF_NAMESPACE
