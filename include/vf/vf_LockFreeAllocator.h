// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEALLOCATOR_VFHEADER__
#define __VF_LOCKFREEALLOCATOR_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeStack.h"

namespace LockFree {

//
// Mostly Lock-free allocator for fixed size memory blocks
//

template <int Bytes>
class FixedAllocator
{
public:
  FixedAllocator ()
  {
  }

  ~FixedAllocator ()
  {
#if VF_CHECK_LEAKS
    vfassert (m_used.is_reset ());
#endif

    for(;;)
    {
      Node* node = m_free.pop_front ();
      if (node)
      {
        ::operator delete (fromNode (node)); // implicit global mutex
#if VF_CHECK_LEAKS
        m_total.release ();
#endif
      }
      else
      {
        break;
      }
    }
  }

  void* alloc ()
  {
    void* p;

    Node* node = m_free.pop_front ();

    if (!node)
    {
      p = ::operator new (Bytes + sizeof (Node)); // implicit global mutex

#if VF_CHECK_LEAKS
      m_total.addref ();
#endif
    }
    else
    {
      p = fromNode (node);
    }

#if VF_CHECK_LEAKS
    m_used.addref ();
#endif

    return p;
  }

  void free (void* p)
  {
    Node* node = toNode (p);

    m_free.push_front (node);

#if VF_CHECK_LEAKS
    m_used.release ();
#endif
  }

  size_t element_size () const
  {
    return Bytes;
  }

private:
  // In order to assure that allocated elements have the
  // same alignment as what we would get from a straight
  // new, the List::Node is placed just after the allocated
  // storage, rather than before. However this might cause
  // alignment issues for the List::Node itslf (which contains
  // an Atomic::Pointer that may need more strict alignment)

  struct Node;
  typedef Stack <Node> List;
  struct Node : List::Node
  {
  };

  static inline void* fromNode (Node* node)
  {
    return reinterpret_cast <char *> (node) - Bytes;
  }

  static inline Node* toNode (void* p)
  {
    return reinterpret_cast <Node*> (reinterpret_cast <char *> (p) + Bytes);
  }

  List m_free;

#if VF_CHECK_LEAKS
  Atomic::Counter m_total;
  Atomic::Counter m_used;
#endif
};

//
// Mostly Lock-free allocator
//
// Use of this allocator ensures the correctness of
// the containers in the LockFree implementation.
//
template <class Elem>
class Allocator
{
public:
  Elem* New ()
    { return new (m_storage.alloc()) Elem(); }

  template <class T1>
  Elem* New (T1 t1)
    { return new (m_storage.alloc()) Elem(t1); }

  template <class T1, class T2>
  Elem* New (T1 t1, T2 t2)
    { return new (m_storage.alloc()) Elem(t1, t2); }

  template <class T1, class T2, class T3>
  Elem* New (T1 t1, T2 t2, T3 t3)
    { return new (m_storage.alloc()) Elem(t1, t2, t3); }

  template <class T1, class T2, class T3, class T4>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4)
    { return new (m_storage.alloc()) Elem(t1, t2, t3, t4); }

  template <class T1, class T2, class T3, class T4, class T5>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
    { return new (m_storage.alloc()) Elem(t1, t2, t3, t4, t5); }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
    { return new (m_storage.alloc()) Elem(t1, t2, t3, t4, t5, t6); }

  void Delete (Elem* e)
  {
    e->~Elem();
    m_storage.free (e);
  }

private:
  FixedAllocator <sizeof (Elem)> m_storage;
};

}

#endif
