// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeAllocator.h"
#include "vf/vf_LockFreeDelay.h"

namespace LockFree {

FixedAllocator <globalFixedAllocatorBlockSize> GlobalFixedAllocator::s_allocator;

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

// If bytesPerBlock is 0 on construction we will use this value instead.
//
static const size_t defaultBytesPerBlock = 8 * 1024;

// This is the upper limit on the amount of physical memory an instance of the
// allocator will allow. Going over this limit means that consumers cannot keep
// up with producers, and application logic should be re-examined.
//
static const size_t hardLimitMegaBytes = 256;

// Allocations are always aligned to this byte value.
//
static const int ByteAlignment = 8;

// template<typename T> struct A { char x; T t; }; sizeof(A<T>)/-sizeof(T) 

// Returns the number of bytes needed to advance p to the correct alignment
template <typename P>
size_t align (P* p)
{
  uintptr_t v = reinterpret_cast <uintptr_t> (p);
  uintptr_t n = ((v + ByteAlignment - 1) / ByteAlignment) * ByteAlignment;
  return n - v;
}

// Returns a pointer with alignment added.
template <typename P>
P* aligned (P* p)
{
  return reinterpret_cast <P*> (reinterpret_cast <char*> (p) + align (p));
}

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

    char pad [ByteAlignment];
  };
};

//------------------------------------------------------------------------------

class Allocator::Block : public Allocator::Blocks::Node
{
public:
  Block (size_t bytes, Allocator* allocator)
    : m_allocator (*allocator)
  {
    m_end = reinterpret_cast <char*> (this) + bytes;
    m_free = aligned (reinterpret_cast <char*> (this + 1));
  }

  ~Block ()
  {
    vfassert (m_refs.get() == 0);
  }

  inline Allocator& getAllocator () const
  {
    return m_allocator;
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

  inline void reset ()
  {
    m_free = reinterpret_cast <char*> (this+1);
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
        char* p = aligned (base);
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
  const char* m_end;                   // last free byte + 1
  Allocator& m_allocator;
};

//------------------------------------------------------------------------------

inline void Allocator::addGarbage (Block* b)
{
  // Any block that gets here must have incremented the use count.
#if LOCKFREE_ALLOCATOR_LOGGING
  vfassert (m_used.get() > 0);
  m_used.release ();
#endif

  m_hot->garbage.push_front (b);
}

Allocator::Allocator (const size_t bytesPerBlock)
  : m_blockBytes (bytesPerBlock == 0 ? defaultBytesPerBlock : bytesPerBlock)
  , m_hard ((hardLimitMegaBytes * 1024 * 1024) / m_blockBytes)
#if LOCKFREE_ALLOCATOR_LOGGING
  , m_swaps (0)
#endif
{
  if (m_blockBytes < sizeof (Block) + 256)
    Throw (Error().fail (__FILE__, __LINE__, TRANS("the block size is too small")));

  m_hot = &m_pool[0];
  m_cold = &m_pool[1];

  m_active = newBlock ();

  startOncePerSecond ();
}

Allocator::~Allocator ()
{
  endOncePerSecond ();

  Block* b;

  // trash active
  b = m_active;
  vfassert (b);
  b->release ();
  addGarbage (b);

  free (m_pool[0]);
  free (m_pool[1]);

#if LOCKFREE_ALLOCATOR_LOGGING
  vfassert (m_used.is_reset ());
  vfassert (m_total.is_reset ());
#endif
}

//------------------------------------------------------------------------------

struct Random
{
  Random () : m_x (0), m_a (1664525), m_c (1013904223) { }

  unsigned long next ()
  {
    m_x = m_a * m_x + m_c;
    return m_x;
  }

private:
  unsigned long m_x;
  unsigned long m_a;
  unsigned long m_c;
};

static Random s_rand;

void* Allocator::allocate (const size_t bytes)
{
  const size_t actual = sizeof (Header) + bytes;

  if (actual > m_blockBytes)
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
          addGarbage (b);

        // Install a fresh empty active block.
        m_active = newBlock ();
      }
      else if (b->release ())
      {
        addGarbage (b);
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
    b->getAllocator().addGarbage (b);
}

//------------------------------------------------------------------------------

Allocator::Block* Allocator::newBlock ()
{
  Block* b;

  // Try to recycle a fresh block.
  b = m_hot->fresh.pop_front ();

  if (b)
  {
    // Reset the storage so it can be re-used.
    b->reset ();
  }
  else
  {
    // Perform hard allocation since we're out of fresh blocks.

    b = new (::operator new (m_blockBytes, std::nothrow_t ())) Block (m_blockBytes, this);
    if (!b)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("a memory allocation failed")));

    const bool exhausted = m_hard.release ();

    if (exhausted)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("the limit of memory allocations was reached")));

#if LOCKFREE_ALLOCATOR_LOGGING
    m_total.addref ();
#endif
  }

  // Give it the active reference.
  b->addref ();

#if LOCKFREE_ALLOCATOR_LOGGING
  m_used.addref ();
#endif

  return b;
}

inline void Allocator::deleteBlock (Block* b)
{
  b->~Block ();
  ::operator delete (b); // global mutex

#if LOCKFREE_ALLOCATOR_LOGGING
  m_total.release ();
#endif
}

void Allocator::doOncePerSecond ()
{
  // Perform the deferred swap of reused
  // and garbage in the cold pool.
  m_cold->fresh.swap (m_cold->garbage);

  // Now swap hot and cold.
  // This is atomic with respect to m_hot.
  Pool* temp = m_hot;
  m_hot = m_cold; // atomic
  m_cold = temp;

#if LOCKFREE_ALLOCATOR_LOGGING
  String s;
  s << "swap " << String (++m_swaps);
  s << " (" << String (m_used.get ()) << "/"
    << String (m_total.get ()) << " of "
    << String (m_hard.get ()) << ")";
  Logger::outputDebugString (s);
#endif
}

void Allocator::free (Blocks& list)
{
  for (;;)
  {
    Block* b = list.pop_front ();
    if (b)
      deleteBlock (b);
    else
      break;
  }
}

void Allocator::free (Pool& pool)
{
  free (pool.fresh);
  free (pool.garbage);
}

}

END_VF_NAMESPACE

