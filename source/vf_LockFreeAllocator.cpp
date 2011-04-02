// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

#include <boost/thread/tss.hpp>

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeAllocator.h"
#include "vf/vf_LockFreeDelay.h"
#include "vf/vf_MemoryAlignment.h"

namespace LockFree {

// must come first
PageAllocator Allocator::m_pages (8192);

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

  inline void addref ()
  {
    m_refs.addref ();
  }

  inline bool release ()
  {
    vfassert (m_refs.get () > 0);

    return m_refs.release ();
  }

  enum Result
  {
    success,  // successful allocation
    ignore,   // disregard the block
    consumed  // block is consumed (1 thread sees this)
  };

  Result allocate (size_t bytes, void* pBlock)
  {
    vfassert (bytes > 0);

    Result result;

    for (;;)
    {
      char* base = m_free.get ();

      if (base)
      {
        char* p = Memory::pointerAdjustedForAlignment (base);
        char* free = p + bytes;

        if (free <= m_end)
        {
          // Try to commit the allocation
          if (m_free.compareAndSet (free, base))
          {
            *(reinterpret_cast <void**> (pBlock)) = p;
            result = success;
            break;
          }
          else
          {
            // Someone changed m_free, retry.
          }
        }
        else
        {
          // Mark the block consumed.
          if (m_free.compareAndSet (0, base))
          {
            // Only one caller sees this, the rest get 'ignore'
            result = consumed;
            break;
          }
          else
          {
            // Happens with another concurrent allocate(), retry.
          }
        }
      }
      else
      {
        // Block is consumed, ignore it.
        result = ignore;
        break;
      }
    }

    return result;
  }

private:
  Atomic::Counter m_refs;        // reference count
  Atomic::Pointer <char> m_free; // next free byte or 0 if inactive.
  char* m_end;                   // last free byte + 1
};

//------------------------------------------------------------------------------

inline Allocator::Block* Allocator::newBlock ()
{
  return new (m_pages.allocate ()) Block (m_pages.getPageBytes());
}

inline void Allocator::deleteBlock (Block* b)
{
  // It is critical that we do not call the destructor,
  // because due to the lock-free implementation, a Block
  // can be accessed for a short time after it is deleted.
  /* b->~Block (); */ // DO NOT CALL!!!

  PageAllocator::deallocate (b);
}

Allocator::Allocator (const size_t pageBytes)
  //: m_pages (pageBytes == 0 ? defaultPageBytes : pageBytes)
{
  if (m_pages.getPageBytes () < sizeof (Block) + 256)
    Throw (Error().fail (__FILE__, __LINE__, TRANS("the block size is too small")));

  m_active = newBlock ();
}

Allocator::~Allocator ()
{
  deleteBlock (m_active);
}

//------------------------------------------------------------------------------

namespace {

struct ThreadData
{
};

}

void* Allocator::allocate (const size_t bytes)
{
  boost::thread_specific_ptr <ThreadData> tsp;

  const size_t actual = sizeof (Header) + bytes;

  if (actual > m_pages.getPageBytes ())
    Throw (Error().fail (__FILE__, __LINE__, TRANS("the memory request was too large")));

  Header* h;

  for (;;)
  {
    // Get an active block.
    Block* b = m_active;
    while (!b)
    {
      CurrentThread::yield ();
      b = m_active;
    }

    // (*) It is possible for the block to get a final release here
    //     In this case it will have been put in the garbage, and
    //     m_active will not match.

    // Acquire a reference.
    b->addref ();

    // Is it still active?
    if (m_active == b)
    {
      // Yes so try to allocate from it.
      const Block::Result result = b->allocate (actual, &h);

      if (result == Block::success)
      {
        // Keep the reference and return the allocation.
        h->block = b;
        break;
      }
      else if (result == Block::consumed)
      {
        // Remove block from active.
        m_active = 0;

        // Take away the reference we added
        b->release ();

        // Take away the original active reference.
        if (b->release ())
          deleteBlock (b);

        // Install a fresh empty active block.
        m_active = newBlock ();
      }
      else
      {
        if (b->release ())
          deleteBlock (b);
      }

      // Try again.
    }
    else
    {
      // Block became inactive, so release our reference.
      b->release ();

      // (*) It is possible for this to be a duplicate final release.
    }
  }

  return h + 1;
}

//------------------------------------------------------------------------------

void Allocator::deallocate (void* p)
{
  Block* const b = (reinterpret_cast <Header*> (p) - 1)->block;

  if (b->release ())
    deleteBlock (b);
}

}

END_VF_NAMESPACE

