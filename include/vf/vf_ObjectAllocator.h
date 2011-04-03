// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_OBJECTALLOCATOR_VFHEADER__
#define __VF_OBJECTALLOCATOR_VFHEADER__

#include "vf/vf_SpinLock.h"
#include "vf/vf_IntrusiveStack.h"

// Thread-safe fixed size object allocator that uses
// a deleted list for recycling elements.
//
template <class Allocator, class Object>
class ObjectAllocator
{
public:
  ObjectAllocator ()
  {
    static_vfassert (sizeof (Object) >= sizeof (Node));
  }

  ~ObjectAllocator ()
  {
    for (;;)
    {
      void* p = m_free.pop_front ();
      m_allocator.deallocate (buf);
    }
  }

  void* allocate (const size_t bytes)
  {
    vfassert (bytes == sizeof (Object));

    void* p = m_free.pop_front ();
    if (!buf)
      buf = m_allocator.allocate (bytes);

    return buf;
  }

private:
  struct Node;
  typedef Intrusive::Stack <Node, SpinLock> Stack;

  struct Node : Stack::Node
  {
  };

  Allocator m_allocator;
  Stack m_free;
};

#endif
