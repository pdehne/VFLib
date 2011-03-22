// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEALLOCATOR_VFHEADER__
#define __VF_LOCKFREEALLOCATOR_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeStack.h"

namespace LockFree {

//
// Mostly Lock-free allocator
//
// Use of this allocator ensures the correctness of
// the containers in the LockFree implementation.
//
// It is required that Elem be derived from a LockFree::List::Node.
// If no tag is specified, the default tag is used.
//
template <class Elem,
          class Tag = detail::List_default_tag>
class Allocator
{
public:
  ~Allocator ()
  {
    for(;;)
    {
      Elem* elem = m_free.pop_front ();
      if (elem)
        ::operator delete (elem); // implicit global mutex
      else
        break;
    }
  }

  Elem* New ()
    { return new (alloc()) Elem(); }

  template <class T1>
  Elem* New (T1 t1)
    { return new (alloc()) Elem(t1); }

  template <class T1, class T2>
  Elem* New (T1 t1, T2 t2)
    { return new (alloc()) Elem(t1, t2); }

  template <class T1, class T2, class T3>
  Elem* New (T1 t1, T2 t2, T3 t3)
    { return new (alloc()) Elem(t1, t2, t3); }

  template <class T1, class T2, class T3, class T4>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4)
    { return new (alloc()) Elem(t1, t2, t3, t4); }

  template <class T1, class T2, class T3, class T4, class T5>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
    { return new (alloc()) Elem(t1, t2, t3, t4, t5); }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
    { return new (alloc()) Elem(t1, t2, t3, t4, t5, t6); }

  void Delete (Elem* e)
  {
    e->~Elem();
    free (e);
  }

private:
  void* alloc ()
  {
    void* p = m_free.pop_front();

    if (!p)
    {
      p = ::operator new (sizeof(Elem)); // implicit global mutex

#if 0
#if VF_DEBUG
      m_count.addref ();
      String s;
      s << "m_count = " << String (m_count.get_value());
      Logger::outputDebugString (s);
#endif
#endif
    }

    return p;
  }

  void free (Elem* elem)
  {
    m_free.push_front (elem);
  }

private:
  Stack <Elem, Tag> m_free;
#if 0
#if VF_DEBUG
  Atomic::UsageCounter m_count;
#endif
#endif
};

}

#endif
