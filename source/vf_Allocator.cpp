// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Allocator.h"
#include "vf/vf_LockFreeDelay.h"
#include "vf/vf_MemoryAlignment.h"

// must come first due to order of construction issues
PageAllocator Allocator::s_pages (8192);

PageAllocator GlobalFixedAllocator::s_allocator (globalFixedAllocatorBlockSize + 64);

//
// Implementation notes
//
// - A Page is a large allocation from a global PageAllocator.
//
// - Each thread maintains an 'active' page from which it makes allocations.
//
// - When the active page is full, a new one takes it's place.
//
// - Page memory is deallocated when it is not active and no longer referenced.
//
// - Each instance of Allocator maintains its own set of per-thread active pages,
//   but uses a global PageAllocator. This reduces memory consumption without
//   affecting performance.
//

// This is the size of a page.
//
static const size_t globalPageBytes = 8 * 1024;

// This precedes every allocation
//
struct Allocator::Header
{
  Allocator::Page* page;
};

//------------------------------------------------------------------------------

class Allocator::Page : NonCopyable
{
public:
  explicit Page (const size_t bytes) : m_refs (1)
  {
    m_end = reinterpret_cast <char*> (this) + bytes;
    m_free = reinterpret_cast <char*> (
      Memory::pointerAdjustedForAlignment (this + 1));
  }

  ~Page ()
  {
    vfassert (m_refs.get() == 0);
  }

  inline bool release ()
  {
    vfassert (m_refs.get () > 0);

    return m_refs.release ();
  }

  void* allocate (size_t bytes)
  {
    vfassert (bytes > 0);

    char* p = Memory::pointerAdjustedForAlignment (m_free);
    char* free = p + bytes;

    if (free <= m_end)
    {
      m_free = free;

      m_refs.addref ();
    }
    else
    {
      p = 0;
    }

    return p;
  }

private:
  Atomic::Counter m_refs; // reference count
  char* m_free;           // next free byte 
  char* m_end;            // last free byte + 1
};

//------------------------------------------------------------------------------

class Allocator::PerThreadData : NonCopyable
{
public:
  explicit PerThreadData (Allocator* allocator)
    : m_allocator (*allocator)
    , m_active (m_allocator.newPage ())
  {
  }

  ~PerThreadData ()
  {
    if (m_active->release ())
      m_allocator.deletePage (m_active);
  }

  inline void* allocate (const size_t bytes)
  {
    const size_t headerBytes = Memory::sizeAdjustedForAlignment (sizeof (Header));
    const size_t bytesNeeded = headerBytes + bytes;

    if (bytesNeeded > Allocator::s_pages.getPageBytes ())
      Throw (Error().fail (__FILE__, __LINE__, TRANS("the memory request was too large")));

    Header* header;

    header = reinterpret_cast <Header*> (m_active->allocate (bytesNeeded));

    if (!header)
    {
      if (m_active->release ())
        deletePage (m_active);

      m_active = m_allocator.newPage ();

      header = reinterpret_cast <Header*> (m_active->allocate (bytesNeeded));
    }

    header->page = m_active;

    return reinterpret_cast <char*> (header) + headerBytes;
  }

private:
  Allocator& m_allocator;
  Page* m_active;
};

//------------------------------------------------------------------------------

inline Allocator::Page* Allocator::newPage ()
{
  return new (s_pages.allocate ()) Page (s_pages.getPageBytes());
}

inline void Allocator::deletePage (Page* page)
{
  // Safe, because each thread maintains its own active page.
  page->~Page ();
  PageAllocator::deallocate (page);
}

Allocator::Allocator ()
{
  //vfassert (s_pages.getPageBytes () >= sizeof (Page) + 256);
}

Allocator::~Allocator ()
{
}

//------------------------------------------------------------------------------

void* Allocator::allocate (const size_t bytes)
{
  PerThreadData* data = m_tsp.get ();

  if (!data)
  {
    data = new PerThreadData (this);
    m_tsp.reset (data);
  }

  return data->allocate (bytes);
}

//------------------------------------------------------------------------------

void Allocator::deallocate (void* p)
{
  const size_t headerBytes = Memory::sizeAdjustedForAlignment (sizeof (Header));
  Header* const header = reinterpret_cast <Header*> (reinterpret_cast <char*> (p) - headerBytes);
  Page* const page = header->page;

  if (page->release ())
    deletePage (page);
}

END_VF_NAMESPACE

