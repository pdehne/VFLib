// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEQUEUE_VFHEADER__
#define __VF_LOCKFREEQUEUE_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeList.h"

namespace LockFree {

//
// Lock-free intrusive queue
//

template <class Elem,
          class Tag = detail::List_default_tag>
class Queue
{
public:
  typedef typename List <Elem, Tag>::Node Node;

  Queue ()
    : m_head (&m_null)
    , m_tail (&m_null)
    , m_null (0)
  {
  }

  void push_back (Node* node)
  {
    node->m_next.set (0);

    Node* prev = m_head.exchange (node);

    // small window where producer could get caught here

    prev->m_next.set (node);
  }

  Elem* pop_front ()
  {
    Node* tail = m_tail;
    Node* next = tail->m_next.get ();

    if (tail == &m_null)
    {
      if (next == 0)
        return 0;

      m_tail = next;
      tail = next;
      next = next->m_next.get ();
    }

    if (next)
    {
      m_tail = next;

      return static_cast <Elem*> (tail);
    }

    Node* head = m_head.get ();
    
    if (tail != head)
      return 0;

    push_back (&m_null);

    next = tail->m_next.get();

    if (next)
    {
      m_tail = next;
      
      return static_cast <Elem*> (tail);
    }

    return 0;
  }

private:
  Atomic::Pointer <Node> m_head;
  Node* m_tail;
  Node m_null;
};

}

#endif
