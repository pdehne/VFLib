// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Allocator.h"
#include "vf/vf_LockFreeDelay.h"
#include "vf/vf_MemoryAlignment.h"

// must come first
PageAllocator Allocator::s_pages (8192);

PageAllocator GlobalFixedAllocator::s_allocator (globalFixedAllocatorBlockSize + 64);

Allocator GlobalAllocator::s_allocator;

namespace {

/* Implementation notes

- A Block is a large allocation from the system that is given
  out as memory in pieces until it is 'consumed'.

- A Pool is a set of two lists: 'fresh' and 'garbage'.

- There are two pools, the 'hot' pool and the 'cold' pool.

- When new blocks are needed we pop from the fresh list of the
  hot pool.

- Every so often, garbage collection is performed on a separate thread.
  During collection, fresh and garbage are swapped in the cold pool.
  Then, the hot and cold pools are atomically swapped.

- At any given time, there is always an 'active' block. It is from
  the active block that allocations are fulfilled.
  
- When an allocation causes the active block to fill (become 'consumed'),
  a new block is put into its place.

- After the last allocation in a consumed block is deallocated, it
  is pushed on to the garbage list of the hot pool.

*/

// If pageBytes is 0 on construction we will use this value instead.
//
static const size_t defaultPageBytes = 8 * 1024;

}

//------------------------------------------------------------------------------
//
// This precedes every allocation
//
struct Allocator::Header
{
  union
  {
    Allocator::Block* block; // backpointer to the page

    char pad [Memory::alignmentBytes];
  };
};

//------------------------------------------------------------------------------

class Allocator::Block : NonCopyable
{
public:
  explicit Block (const size_t bytes) : m_refs (1)
  {
    m_end = reinterpret_cast <char*> (this) + bytes;
    m_free = reinterpret_cast <char*> (
      Memory::pointerAdjustedForAlignment (this + 1));
  }

  ~Block ()
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

class Allocator::PerThreadData : public Allocator::Threads::Node
{
public:
  explicit PerThreadData (Allocator* allocator)
    : m_allocator (*allocator)
    , active (m_allocator.newBlock ())
  {
    //Mutex::ScopedLockType lock (m_allocator.m_mutex);
    //m_allocator.m_threads.push_back (this);
  }

  ~PerThreadData ()
  {
    //Mutex::ScopedLockType lock (m_allocator.m_mutex);
    //m_allocator.m_threads.remove (this);
    m_allocator.deleteBlock (active);
  }

  Block* active;

private:
  Allocator& m_allocator;
};

//------------------------------------------------------------------------------

inline Allocator::Block* Allocator::newBlock ()
{
  return new (s_pages.allocate ()) Block (s_pages.getPageBytes());
}

inline void Allocator::deleteBlock (Block* b)
{
  // It is critical that we do not call the destructor,
  // because due to the lock-free implementation, a Block
  // can be accessed for a short time after it is deleted.
  b->~Block (); // DO NOT CALL!!!

  PageAllocator::deallocate (b);
}

Allocator::Allocator (const size_t pageBytes)
  //: s_pages (pageBytes == 0 ? defaultPageBytes : pageBytes)
{
  if (s_pages.getPageBytes () < sizeof (Block) + 256)
    Throw (Error().fail (__FILE__, __LINE__, TRANS("the block size is too small")));
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

  const size_t actual = sizeof (Header) + bytes;

  if (actual > s_pages.getPageBytes ())
    Throw (Error().fail (__FILE__, __LINE__, TRANS("the memory request was too large")));

  Header* h;

  Block* b = data->active;

  h = reinterpret_cast <Header*> (b->allocate (actual));

  if (!h)
  {
    if (b->release ())
      deleteBlock (b);

    data->active = newBlock ();

    b = data->active;

    h = reinterpret_cast <Header*> (b->allocate (actual));

    vfassert (h);
  }

  h->block = b;

  return h + 1;
}

//------------------------------------------------------------------------------

void Allocator::deallocate (void* p)
{
  Block* const b = (reinterpret_cast <Header*> (p) - 1)->block;

  if (b->release ())
    deleteBlock (b);
}

END_VF_NAMESPACE

