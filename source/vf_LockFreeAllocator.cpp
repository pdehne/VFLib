// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFreeAllocator.h"

#define LOG_BLOCKS 0

namespace LockFree {

FixedAllocator <globalAllocatorBlockSize> globalAllocator;

//------------------------------------------------------------------------------

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

namespace {

// if bytesPerBlock is 0 on construction we will use this value.
//
static const size_t defaultBytesPerBlock = 8 * 1024;

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

//------------------------------------------------------------------------------
//
// If we exceed this limit on hard allocations then an exception will be thrown.
//
// Typically this means that consumers cannot keep up
// with producers and the app would be non-functional.
//
//
static const size_t hardLimitMegaBytes = 256;

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

  // One reference is always counted for a Block if it is active.
  inline void addref ()
  {
    m_refs.addref ();
  }

  inline bool consumed () const
  {
    return m_free.get () == 0;
  }

  // Possible results for allocate ()
  //
  enum Result
  {
    success,  // successful allocation
    remove,   // take the block out of active state
    garbage,  // block got a final release and is garbage
    ignore    // block should be completely ignored
  };

  Result allocate (size_t bytes, void* pBlock)
  {
#if 0
    /*
    for (;;)
    {
      // Get pointer to free bytes.
      char* p = m_free.get ();

      // Is the block availble?
      if (p)
      {
        // Is there enough space left?
        if (p + bytes <= m_end)
        {
          // Got uncomitted space. Increment reference
          // count to prevent a final release elsewhere.
          m_refs.addref ();

          // Commit the allocation.
          if (m_free.compareAndSet (p + bytes, p))
          {
            // Success, return the block.
            //
            *(reinterpret_cast <void**> (pBlock)) = p;

            result = success;
            break;
          }
          else
          {
            // Commit failed so roll back the reference. This could
            // cause a final release from a concurrent deallocate().
            //
            if (release())
            {
              // We got a final release. The block is already inactive.
              // Only one caller will see this state, the rest will see 'full'.
              //
              result = garbage;
              break;
            }
            else
            {
              // Release was not final so retry.
            }
          }
        }
        else
        {
          // No space so try to make this block unavailable
          //
          for (;;)
          {
            p = m_free.get ();

            if (p == 0)
            {
              // Someone else already deactivated it.
              //
              result = ignore;
              break;
            }
            else if (m_free.compareAndSet (0, p))
            {
              // We set the pointer so caller should remove from active.
              //
              result = remove;
              break;
            }
            else
            {
              // This happens when a smaller concurrent allocate() is either
              // successful, or gets a final release on rollback.
            }
          }

          break;
        }
      }
      else
      {
        // Someone else set the block to unavailable so ignore it.
        result = ignore;
        break;
      }
    }
    */
#endif
    vfassert (bytes > 0);

    Result result;

    for (;;)
    {
      char* base = m_free.get ();

      if (base)
      {
        char* p = aligned (base);

        if (p + bytes <= m_end)
        {
          m_free = p + bytes;
          m_refs.addref ();

          *(reinterpret_cast <void**> (pBlock)) = p;
          result = success;
          break;
        }
        else
        {
          // Mark the block consumed.
          m_free = 0;

          // Only one caller will see this, the rest should see 'ignore'.
          result = remove;
          break;
        }
      }
      else
      {
        // Block is consumed.
        result = ignore;
        break;
      }
    }

    return result;
  }

  bool release ()
  {
#if 0
    /*
    // Mark the free position.
    char* p = m_free.get ();

    // IS THIS NEEDED?
    //Atomic::memoryBarrier();

    bool final = m_refs.release ();

    // Was this a final release?
    if (final)
    {
      // Mark the block unavailable to prevent re-use.
      if (p)
        final = m_free.compareAndSet (0, p);

      // If we failed it means someone else got a reference.
      // This can happen if there is still space in the block.
      if (!final)
      {
        p = 0;
      }
    }

    return final;
    */
#endif

    vfassert (m_refs.get () > 0);

    const bool final = m_refs.release ();

    return final;
  }

  inline void finalRelease ()
  {
#if VF_DEBUG
    const bool final = release ();
    vfassert (final);
#else
    release ();
#endif
  }

  inline void reset ()
  {
    // Reset the free pointer
    m_free = reinterpret_cast <char*> (this+1);
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

  startOncePerSecond ();
}

BlockAllocator::~BlockAllocator ()
{
  endOncePerSecond ();

  // Throw the active block in the garbage to get a common code path.
  Block* b = m_active;
  if (b)
  {
    b->finalRelease ();

    addGarbage (b);
  }

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
#if 0
  /*
  for (;;)
  {
    // Get the active block if there is one.
    b = m_hot->active;

    if (b)
    {
      // Try to allocate from the active block.
      const Block::Result result = b->allocate (actual, &h);

      if (result == Block::success)
      {
        break;
      }
      else if (result == Block::remove)
      {
        // (At this point the block might have received a final release)

        // Block has references but should be removed from active.
        m_hot->active.compareAndSet (0, b);
      }
      else if (result == Block::garbage)
      {
        // Block got a final release
        m_hot->active.compareAndSet (0, b);

        // Reset it to full
        b->reset ();

#if LOG_BLOCKS
        vfassert (m_used.get() > 0);
        m_used.release ();
#endif

        addGarbage (b);
      }
      else
      {
        // Ignore the block and retry.
      }
    }
    else
    {
      // No active block so try to recycle one.
      b = m_hot->fresh.pop_front ();

      // No fresh blocks? Make a new one.
      if (!b)
        b = new_block ();

      // Try to set this new block as the active block.
      if (m_hot->active.compareAndSet (b, 0))
      {
        // Success, new block is active.
#if LOG_BLOCKS
        m_used.addref ();
#endif
      }
      else
      {
        // Failed, someone else already set a new active block.
        // Add this block to garbage.
        addGarbage (b);
      }
    }
  }

  h->block = b;
  */
#endif

//  Mutex::ScopedLockType lock0 (m_lock0);

  const size_t actual = sizeof (Header) + bytes;

  if (actual > m_blockBytes)
    Throw (Error().fail (__FILE__, __LINE__, TRANS("the memory request was too large")));

  Block* b;
  Header* h;

  for (;;)
  {
    // Get the active block.
    b = m_active;

    // Is there an active block with some space?
    if (b && !b->consumed ())
    {
      // Yes so try to allocate from it.
      Block::Result result;

      {
        Mutex::ScopedLockType lock (m_lock);
        result = b->allocate (actual, &h);
      }

      if (result == Block::success)
      {
        // Guaranteed to have a reference to the block here.
        b->addref ();
        bool final = b->release ();
        vfassert (!final);

        break;
      }
      else if (result == Block::remove)
      {
        // Block is consumed.

        // Take away the initial reference.
        bool final;
        {
          Mutex::ScopedLockType lock (m_lock);
          final = b->release ();
        }

        // Unset active if it matches.
        m_active.compareAndSet (0, b);

        if (final)
          addGarbage (b);
      }
      else if (result == Block::ignore)
      {
        // Try again.
      }
      else
      {
        vfassert(0);
      }
    }
    else
    {
      // Get an empty block. It will come with a reference.
      Block* empty = newBlock ();

      // Try to install it as the active block.
      if (m_active.compareAndSet (empty, b))
      {
        // It worked, so try the loop again.
      }
      else
      {
#if 0
        // TODO: CAN WE PUSH IT BACK ONTO fresh ??? OR IS THIS ABA?

        // Failed, someone already set an active block.
        // Remove the initial reference.
        empty->finalRelease ();

        // Add this block to garbage.
        addGarbage (empty);
#else
        empty->finalRelease ();
        m_hot->fresh.push_front (empty);
#endif
      }
    }
  }

  h->block = b;

  return h + 1;
}

//------------------------------------------------------------------------------

void BlockAllocator::deallocate (void* p)
{
  Mutex::ScopedLockType lock1 (m_lock1);

  Block* b = (reinterpret_cast <Header*> (p) - 1)->block;

  bool final;

  {
    Mutex::ScopedLockType lock (m_lock);
    final = b->release ();
  }

  // Is this block final?
  if (final)
  {
    // Take this out of action if it's active.
    m_active.compareAndSet (0, b);

    // Put it in the garbage.
    // This might trigger a collection.
    addGarbage (b);
  }
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

void BlockAllocator::addGarbage (Block* b)
{
  // Any block that gets here must have incremented the use count.
#if LOG_BLOCKS
  vfassert (m_used.get() > 0);
  m_used.release ();
#endif

  m_hot->garbage.push_front (b);

  // Garbage collect if it's time, and we're first.
  //
  if (m_collect.isSet () && m_collect.tryClear ())
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
    s << " (" << String (m_used.get()) << "/"
      << String (m_total.get()) << ")";
    Logger::outputDebugString (s);
#endif
  }
}

void BlockAllocator::doOncePerSecond ()
{
  // Signal garbage collection
  m_collect.trySet ();
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

