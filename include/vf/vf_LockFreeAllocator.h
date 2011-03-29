// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEALLOCATOR_VFHEADER__
#define __VF_LOCKFREEALLOCATOR_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeReadWriteMutex.h"
#include "vf/vf_LockFreeStack.h"
#include "vf/vf_OncePerSecond.h"
#include "vf/vf_Type.h"

//#define ALLOCATOR_LOGGING VF_CHECK_LEAKS
#define ALLOCATOR_LOGGING 0

namespace LockFree {

enum
{
  //
  // This number should be as small as possible
  // without generating compile time assertions.
  //
  globalAllocatorBlockSize = 96
};

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
template <size_t BytesPerBlock>
class BlockAllocator : private OncePerSecond
{
public:
  enum
  {
    // If we exceed this limit on hard allocations
    // then an exception will be thrown.
    //
    // Typically this means that consumers cannot keep up
    // with producers and the app would be non-functional.
    //
    hardLimitMegaBytes = 256
  };

  BlockAllocator ()
    : m_hard ((hardLimitMegaBytes * 1024 * 1024) / BytesPerBlock)
#if ALLOCATOR_LOGGING
    , m_swaps (0)
#endif
  {
    m_hot = &m_pool[0];
    m_cold = &m_pool[1];

    startOncePerSecond ();
  }

  ~BlockAllocator ()
  {
    endOncePerSecond ();

    free (m_pool[0]);
    free (m_pool[1]);

#if ALLOCATOR_LOGGING
    vfassert (m_used.is_reset ());
    vfassert (m_total.is_reset ());
#endif
  }

  template <size_t Bytes>
  void* allocate ()
  {
    static_vfassert (Bytes <= BytesPerBlock - sizeof (Block));
    return allocate (Bytes);
  }

  void* allocate (size_t bytes)
  {
    Header* h;

    size_t actual = sizeof (Header) + bytes;

    Block* b;

    for (;;)
    {
      // Get the active block.
      b = m_hot->active;

      // Is there an active block?
      if (b)
      {
        // Try to allocate from the active block.
        const int result = b->allocate (actual, &h);

        if (result == Block::success)
        {
          break;
        }
        else if (result == Block::inactive)
        {
          // (At this point the block might have received a final release)

          // Block has references but should be removed from active.
          m_hot->active.compareAndSet (0, b);
        }
        else if (result == Block::garbage)
        {
          // Block got a final release
          m_hot->active.compareAndSet (0, b);

#if ALLOCATOR_LOGGING
          m_used.release ();
#endif

          pushGarbage (b);
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

        // No free blocks? Make a new one.
        if (!b)
          b = new_block ();

        // Try to set this new block as the active block.
        if (m_hot->active.compareAndSet (b, 0))
        {
          // Success, new block is active.
#if ALLOCATOR_LOGGING
          m_used.addref ();
#endif
        }
        else
        {
          // Failed, someone else already set a new active block.
          // Add this block to garbage.
          pushGarbage (b);
        }
      }
    }

    vfassert (h);

    h->b = b;
    
    return fromHeader (h);
  }

  void deallocate (void* p)
  {
    Block* b = toHeader (p)->b;

    if (b->release ())
    {
      // If this is the active block then remove it.
      m_hot->active.compareAndSet (0, b);

      // Reset it to full
      b->reset ();

#if ALLOCATOR_LOGGING
      m_used.release ();
#endif

      // Add this block to garbage.
      pushGarbage (b);
    }
  }

private:
  struct Block;
  typedef Stack <Block> Blocks;

  Block* new_block ()
  {
    Block* b = new (::operator new (BytesPerBlock, std::nothrow_t ())) Block (BytesPerBlock);
    if (!b)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("a memory allocation failed")));

    const bool exhausted = m_hard.release ();

    if (exhausted)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("the limit of memory allocations was reached")));

#if ALLOCATOR_LOGGING
    m_total.addref ();
#endif

    return b;
  }

  void pushGarbage (Block* b)
  {
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

#if ALLOCATOR_LOGGING
      String s;
      s << "swap " << String (++m_swaps);
      s << " (" << String (m_used.get()) << "/"
        << String (m_total.get()) << ")";
      Logger::outputDebugString (s);
#endif
    }
  }

  void doOncePerSecond ()
  {
    //m_collect.trySet ();
  }

  class Block : public Blocks::Node
  {
  public:
    explicit Block (size_t bytes)
    {
      m_end = reinterpret_cast <char*> (this) + bytes;
      m_free = reinterpret_cast <char*> (this + 1);
    }

    ~Block ()
    {
      vfassert (m_refs.get() == 0);
    }

    enum
    {
      success,    // successful allocation
      inactive,   // take the block out of active state
      garbage,    // block got a final release and is garbage
      ignore      // block should be completely ignored
    };

    int allocate (size_t bytes, void* pBlock)
    {
      vfassert (bytes > 0);

      int result;

      char* p;

      for (;;)
      {
        // Get pointer to free bytes.
        p = m_free.get ();

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
                // We set the pointer so caller should deactivate.
                //
                result = inactive;
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

      return result;
    }

    bool release ()
    {
      // Mark the free position.
      char* p = m_free.get ();

      // IS THIS NEEDED?
      //Atomic::memoryBarrier();

      bool final = m_refs.release ();

      // Was this a final release?
      if (final)
      {
        // Mark the block unavailable to prevent re-use.
        final = m_free.compareAndSet (0, p);

        // If we failed it means someone else got a reference.
        // This can happen if there is still space in the block.
      }

      return final;
    }

    inline void reset ()
    {
      // Reset the free pointer
      m_free = reinterpret_cast <char*> (this+1);
    }

  private:
    const char* m_end;             // last free byte + 1
    Atomic::Counter m_refs;        // reference count
    Atomic::Pointer <char> m_free; // next free byte or 0 if inactive.
  };

private:
  struct Header
  {
    enum { Alignment = 8 };
    union
    {
      Block* b; // backpointer to the page
      char pad [Alignment];
    };
  };

  // A pool contains the recycle list, from which we allocate,
  // and the garbage list, into which we free. This avoids the
  // ABA problem in the underlying Stack object.
  //
  struct Pool
  {
    Atomic::Pointer <Block> active;
    Blocks fresh;   // fresh nodes for re-use
    Blocks garbage; // garbage that needs to cool
  };

  static inline void* fromHeader (Header* header)
  {
    return header + 1;
  }

  static inline Header* toHeader (void* p)
  {
    return reinterpret_cast <Header*> (p) - 1;
  }

  void free (Blocks& list)
  {
    for (;;)
    {
      Block* b = list.pop_front ();

      if (b)
      {
        b->~Block ();
        ::operator delete (b); // global mutex

#if ALLOCATOR_LOGGING
        m_total.release ();
#endif
      }
      else
      {
        break;
      }
    }
  }

  void free (Pool& pool)
  {
    Block* b = pool.active;

    if (b)
    {
      pool.active = 0; // ???

      b->~Block ();
      ::operator delete (b); // global mutex

#if ALLOCATOR_LOGGING
      m_used.release ();
      m_total.release ();
#endif
    }

    free (pool.fresh);
    free (pool.garbage);
  }

private:
  // There are two pools, one hot and one cold. All alloc and
  // free go through the hot pool. Every so often, we will atomically
  // swap these two pools. Before the swap takes place, the lists
  // in the cold pool are themselves swapped. This operation is deferred
  // as long as possible to reduce the effective chance of encountering
  // the ABA problem down to zero. This depends on a sufficiently large
  // value of "defaultKiloBytes" for the problem domain.
  //
  Pool m_pool[2];                 // pair of pools
  Pool* m_cold;                   // pool which is cooling down
  Atomic::Pointer <Pool> m_hot;   // pool we are currently using
  Atomic::Flag m_collect;         // flag telling us to do gc
  Atomic::Counter m_hard;         // limit of system allocations

#if ALLOCATOR_LOGGING
  int m_swaps;
  Atomic::Counter m_total;
  Atomic::Counter m_used;
#endif
};

// Wait-free allocator for fixed size memory blocks
//
// - Any thread may allocate and free concurrently.
//
// - Freed blocks are re-used in future allocations after a certain amount
//   of time has passed, in order to prevent the ABA problem for callers.
//
// - When there are no free blocks available, the algorithm becomes a blocking
//   algorithm since we request memory from the standard library.
//
template <size_t BlockSize>
class FixedAllocator : private OncePerSecond
{
public:
  enum
  {
    // This allocator will time its garbage collections to
    // try to keep the memory utilization at or below the byteLimit.
    //
    // This is something of a soft limit but we should be close.
    //
    defaultKiloBytes = 64 * 1024,

    // If we exceed this limit on hard allocations
    // then an exception will be thrown.
    //
    // Typically this means that consumers cannot keep up
    // with producers and the app would be non-functional.
    //
    hardLimitMegaBytes = 1 * 1024
  };

  enum
  {
    blockSize = BlockSize
  };

  FixedAllocator (int byteLimit = defaultKiloBytes * 1024)
    : m_hard ((hardLimitMegaBytes * 1024 * 1024) / (BlockSize + sizeof(Node)))
#if ALLOCATOR_LOGGING
    , m_swaps (0)
#endif
  {
    m_hot = &m_pool[0];
    m_cold = &m_pool[1];

    startOncePerSecond ();
  }

  ~FixedAllocator ()
  {
    endOncePerSecond ();

#if ALLOCATOR_LOGGING
    vfassert (m_used.is_reset ());
#endif

    free (m_pool[0]);
    free (m_pool[1]);

#if ALLOCATOR_LOGGING
    vfassert (m_total.is_reset ());
#endif
  }

  template <size_t Bytes>
  void* allocate ()
  {
    static_vfassert (Bytes <= BlockSize);
    return recycle_block ();
  }

  void deallocate (void* p)
  {
    Node* node = toNode (p);

    m_hot->garbage.push_front (node);

#if ALLOCATOR_LOGGING
    m_used.release ();
#endif

    // See if we need to perform garbage collection.
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

#if ALLOCATOR_LOGGING
      String s;
      s << "swap " << String (++m_swaps);
      s << " (" << String (m_used.get()) << "/"
        << String (m_total.get()) << ")";
      Logger::outputDebugString (s);
#endif
    }
  }

private:
  void* hard_alloc ()
  {
    void* p;

    // implicit global mutex
    p = ::operator new (BlockSize + sizeof (Node), std::nothrow_t());
    if (!p)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("a memory allocation failed")));

    const bool exhausted = m_hard.release ();

    if (exhausted)
      Throw (Error().fail (__FILE__, __LINE__, TRANS("the limit of memory allocations was reached")));

#if ALLOCATOR_LOGGING
    m_total.addref ();
#endif

    return p;
  }

  void* recycle_block ()
  {
    Node* node = m_hot->fresh.pop_front ();

    void* p;

    if (!node)
    {
      p = hard_alloc ();
    }
    else
    {
      p = fromNode (node);
    }

#if ALLOCATOR_LOGGING
    m_used.addref ();
#endif

    return p;
  }

  void doOncePerSecond ()
  {
    m_collect.trySet ();
  }

private:
  // In order to assure that allocated elements have the
  // same alignment as what we would get from a straight
  // new, the List::Node is placed just after the allocated
  // storage, rather than before. However this might cause
  // alignment issues for the List::Node itself (which contains
  // an Atomic::Pointer that may need more strict alignment)
  //
  struct Node;
  typedef Stack <Node> List;
  struct Node : List::Node
  {
    //Page* page; // backpointer to the page
  };

  // A pool contains the recycle list, from which we allocate,
  // and the garbage list, into which we free. This avoids the
  // ABA problem in the underlying Stack object.
  //
  struct Pool
  {
    List fresh;   // fresh nodes for re-use
    List garbage; // garbage that needs to cool
  };

  static inline void* fromNode (Node* node)
  {
    return reinterpret_cast <char *> (node) - BlockSize;
  }

  static inline Node* toNode (void* p)
  {
    return reinterpret_cast <Node*> (reinterpret_cast <char *> (p) + BlockSize);
  }

  void free (List& list)
  {
    for (;;)
    {
      Node* node = list.pop_front ();

      if (node)
      {
        ::operator delete (fromNode (node)); // implicit global mutex

#if ALLOCATOR_LOGGING
        m_total.release ();
#endif
      }
      else
      {
        break;
      }
    }
  }

  void free (Pool& pool)
  {
    free (pool.fresh);
    free (pool.garbage);
  }

private:
  // There are two pools, one hot and one cold. All alloc and
  // free go through the hot pool. Every so often, we will atomically
  // swap these two pools. Before the swap takes place, the lists
  // in the cold pool are themselves swapped. This operation is deferred
  // as long as possible to reduce the effective chance of encountering
  // the ABA problem down to zero. This depends on a sufficiently large
  // value of "defaultKiloBytes" for the problem domain.
  //
  Pool m_pool[2];                 // pair of pools
  Pool* m_cold;                   // pool which is cooling down
  Atomic::Pointer <Pool> m_hot;   // pool we are currently using
  Atomic::Flag m_collect;         // flag telling us to do gc
  Atomic::Counter m_hard;         // limit of system allocations

#if ALLOCATOR_LOGGING
  int m_swaps;
  Atomic::Counter m_total;
  Atomic::Counter m_used;
#endif
};

typedef FixedAllocator <globalAllocatorBlockSize> GlobalFixedAllocator;
extern GlobalFixedAllocator globalAllocator;

typedef BlockAllocator <1024> GlobalBlockAllocator;

class GlobalAllocator
{
public:
  template <int Bytes>
  void* allocate ()
  {
    return globalAllocator.allocate <Bytes> ();
  }

  void deallocate (void* p)
  {
    globalAllocator.deallocate (p);
  }
};

// Sugar hack since the compiler cannot infer
// template arguments based on the return type.
//
// Usage:
//
//  C* c = globalAlloc<C>::New (arg1, arg2,...);
//
template <class C>
struct globalAlloc
{
  static C* New ()
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C;
  }

  template <class T1>
  static C* New (const T1& t1)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1);
  }

  template <class T1, class T2>
  static C* New (const T1& t1, const T2& t2)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1, t2);
  }

  template <class T1, class T2, class T3>
  static C* New (const T1& t1, const T2& t2, const T3& t3)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1, t2, t3);
  }

  template <class T1, class T2, class T3, class T4>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1, t2, t3, t4);
  }

  template <class T1, class T2, class T3, class T4, class T5>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
                 const T5& t5, const T6& t6)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5, t6);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
                 const T5& t5, const T6& t6, const T7& t7)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5, t6, t7);
  }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  static C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
                 const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  {
    static_vfassert (sizeof (C) <= GlobalFixedAllocator::blockSize);
    return new (globalAllocator.allocate <sizeof (C)> ()) C (t1, t2, t3, t4, t5, t6, t7, t8);
  }
};

template <class C>
void globalDelete (C* c)
{
  c->~C();
  globalAllocator.deallocate (c);
}

}

#endif
