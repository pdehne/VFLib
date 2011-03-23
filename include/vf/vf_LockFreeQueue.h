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
#if 1
    node->m_next.set (0);

    Node* prev = m_head.exchange (node);

    // small window where producer could get caught here

    prev->m_next.set (node);

#else
    Node* tail;
    Node* next;

    for (;;)
    {
      tail = m_tail.get();
      next = tail->m_next.get();

      if (tail == m_tail.get ())
      {
        if (next == 0)
        {
          if (m_tail.get()->m_next.compareAndSet (node, 0))
            break;
        }
        else
        {
          m_tail.compareAndSet (next, tail);
        }
      }
    }
#endif
  }

  Elem* pop_front ()
  {
#if 1
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
#else
    Node* head;
    Node* next;
    Node* tail;

    for (;;)
    {
      head = m_head.get();
      next = head->m_next.get();
      tail = m_tail.get();

      if (head == m_head.get())
      {
        if (head == tail)
        {
          if (next == 0)
          {
            break;
          }
          else
          {
            m_tail.compareAndSet (next, tail);
          }
        }
        else if (m_head.compareAndSet (next, head))
        {
          break;
        }
      }
    }

    return static_cast <Elem*> (next);
#endif
  }

private:
  Atomic::Pointer <Node> m_head;
  Node* m_tail;
  Node m_null;
};

}

#endif
