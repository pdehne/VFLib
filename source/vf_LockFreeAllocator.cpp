// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeAllocator.h"
#include "vf/vf_LockFreeDelay.h"

#define LOG_BLOCKS 0

namespace LockFree {

FixedAllocator <globalAllocatorBlockSize> GlobalAllocator::s_allocator;

BlockAllocator GlobalBlockAllocator::s_allocator;

namespace {

/* Implementation notes

There are two pools, one hot and one cold. All alloc and
free go through the hot pool. Every so often, we will atomically
swap these two pools. Before the swap takes place, the lists
in the cold pool are themselves swapped. This operation is deferred
as long as possible to reduce the effective chance of encountering
the ABA problem down to zero. This depends on a sufficiently large
garbage collection interval. As long as no call to allocate() or deallocate()
takes longer than the garbage collection interval (1 second)? We are safe.

- We always pop from the fresh list of the hot pool
- We always push to the garbage list of the hot pool

*/

// If bytesPerBlock is 0 on construction we will use this value instead.
//
static const size_t defaultBytesPerBlock = 8 * 1024;

// This is the upper limit on the amount of physical memory an instance of the
// allocator will allow. Going over this limit means that consumers cannot keep
// up with producers, and application logic should be re-examined.
//
static const size_t hardLimitMegaBytes = 2 * 4 * 256;

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

// This precedes every allocation
struct BlockAllocator::Header
{
  union
  {
    BlockAllocator::Block* block; // backpointer to the page

    char pad [ByteAlignment];
  };
};

//------------------------------------------------------------------------------

// Block is a chunk of allocatable storage.
//
// - m_free pointer pointers to the next available byte and gets advanced
//   on each successful allocation
//
//
class BlockAllocator::Block : public BlockAllocator::Blocks::Node
{
public:
  explicit Block (size_t bytes)
  {
    m_end = reinterpret_cast <char*> (this) + bytes;
    m_free = reinterpret_cast <char*> (this + 1);
    m_free = aligned (m_free.get());
  }

  ~Block ()
  {
    vfassert (m_refs.get() == 0);
  }

  inline bool isReferenced () const
  {
    return m_refs.get () > 0;
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

  // Reset the free storage pointer.
  inline void reset ()
  {
    m_free = reinterpret_cast <char*> (this+1);
  }

  enum Result
  {
    success,  // successful allocation
    ignore,   // disregard the block
    consumed  // block is consumed (1 thread see this)
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
          //ReadWriteMutex::ScopedReadLockType lock (mutex);
  
          // Try to commit the allocation
          if (m_free.compareAndSet (free, base))
          {
            *(reinterpret_cast <void**> (pBlock)) = p;
            result = success;
            break;
          }
          else
          {
            // Someone changed m_free so try again
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
            // Happens with another concurrent allocate()
          }
        }
      }
      else
      {
        // Block already consumed, ignore it.
        result = ignore;
        break;
      }
    }

    return result;
  }

private:
  char* m_end;                   // last free byte + 1
  Atomic::Counter m_refs;        // reference count
  Atomic::Pointer <char> m_free; // next free byte or 0 if inactive.
};

//------------------------------------------------------------------------------
//
//
//

inline void BlockAllocator::addGarbage (Block* b)
{
  // Any block that gets here must have incremented the use count.
#if LOG_BLOCKS
  vfassert (m_used.get() > 0);
  m_used.release ();
#endif

  m_hot->garbage.push_front (b);
}

BlockAllocator::BlockAllocator (size_t bytesPerBlock)
  : m_blockBytes (bytesPerBlock == 0 ? defaultBytesPerBlock : bytesPerBlock)
  , m_hard ((hardLimitMegaBytes * 1024 * 1024) / m_blockBytes)
#if LOG_BLOCKS
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

BlockAllocator::~BlockAllocator ()
{
  endOncePerSecond ();

  // Throw away the active block
  Block* b = m_active;
  vfassert (b);
  b->release ();
  addGarbage (b);

  free (m_pool[0]);
  free (m_pool[1]);

#if LOG_BLOCKS
  vfassert (m_used.is_reset ());
  vfassert (m_total.is_reset ());
#endif
}

//------------------------------------------------------------------------------

void* BlockAllocator::allocate (const size_t bytes)
{
  const size_t actual = sizeof (Header) + bytes;

  if (actual > m_blockBytes)
    Throw (Error().fail (__FILE__, __LINE__, TRANS("the memory request was too large")));

  Header* h;

  for (;;)
  {
    // Get an active block
    Block* b = m_active;
    if (!b)
    {
      Delay delay;
      do
      {
        delay.spin ();
        Atomic::memoryBarrier ();
        b = m_active;
      }
      while (!b);
    }

    // Acquire a reference
    vfassert (b->isReferenced ());
    b->addref ();

    // Is it still active?
    if (m_active == b)
    {
      //ReadWriteMutex::ScopedWriteLockType lock (m_lock);
  
      // Yes so try to allocate from it.
      Block::Result result;

      {
        //ReadWriteMutex::ScopedReadLockType lock (m_lock);
        //ReadWriteMutex::ScopedWriteLockType lock (m_lock);
        result = b->allocate (actual, &h);
      }

      if (result == Block::success)
      {
#if 0
        m_lock.enter_write ();
        m_lock.exit_write ();
#endif

        // Keep the reference and return the allocation.
        h->block = b;
        break;
      }
      else if (result == Block::consumed)
      {
        bool success;

        success = m_active.compareAndSet (0, b);
        vfassert (success);

        //m_lock.enter_write ();

        // Take away the reference we added
        bool final = b->release ();
        vfassert (!final);

        //m_lock.exit_write ();

        // Now take away the active block reference.
        final = b->release ();
        if (final)
          addGarbage (b);

        // Get an empty block. It will come with a reference.
        b = newBlock ();

        // Install it as the active block.
        success = m_active.compareAndSet (b, 0);
        vfassert (success);
      }
      else
      {
        //m_lock.enter_write ();
        //m_lock.exit_write ();

        const bool final = b->release ();

        if (final)
          addGarbage (b);

        // Try again.
      }
    }
    else
    {
      // Block became inactive, so release our reference.
#if 0
      if (b->release ())
      {
        // It was a final release
        addGarbage (b);
      }
#else
      b->release ();
#endif

      // Try again.
    }
  }

  return h + 1;
}

//------------------------------------------------------------------------------

void BlockAllocator::deallocate (void* p)
{
  Block* b = (reinterpret_cast <Header*> (p) - 1)->block;

  bool final;

  {
    //ReadWriteMutex::ScopedWriteLockType lock (m_lock);
    //ReadWriteMutex::ScopedReadLockType lock (m_lock);
    //Mutex::ScopedLockType lock (m_lock0);
    final = b->release ();
  }

  if (final)
    addGarbage (b);
}

//------------------------------------------------------------------------------

BlockAllocator::Block* BlockAllocator::newBlock ()
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

    b = new (::operator new (m_blockBytes, std::nothrow_t ())) Block (m_blockBytes);
    if (!b)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("a memory allocation failed")));

    const bool exhausted = m_hard.release ();

    if (exhausted)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("the limit of memory allocations was reached")));

#if LOG_BLOCKS
    m_total.addref ();
#endif
  }

  // Give it the active reference.
  b->addref ();

#if LOG_BLOCKS
  m_used.addref ();
#endif

  return b;
}

inline void BlockAllocator::deleteBlock (Block* b)
{
  b->~Block ();
  ::operator delete (b); // global mutex

#if LOG_BLOCKS
  m_total.release ();
#endif
}

void BlockAllocator::doOncePerSecond ()
{
  // Perform the deferred swap of reused
  // and garbage in the cold pool.
  m_cold->fresh.swap (m_cold->garbage);

  // Now swap hot and cold.
  // This is atomic with respect to m_hot.
  Pool* temp = m_hot;
  m_hot = m_cold; // atomic
  m_cold = temp;

#if LOG_BLOCKS
  String s;
  s << "swap " << String (++m_swaps);
  s << " (" << String (m_used.get ()) << "/"
    << String (m_total.get ()) << " of "
    << String (m_hard.get ()) << ")";
  Logger::outputDebugString (s);
#endif
}

void BlockAllocator::free (Blocks& list)
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

void BlockAllocator::free (Pool& pool)
{
  free (pool.fresh);
  free (pool.garbage);
}

}

END_VF_NAMESPACE

