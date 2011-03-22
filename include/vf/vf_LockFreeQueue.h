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
// This implementation requires that Nodes are never deleted, only re-used.
//
//
// TODO: THIS SEEMS FLAWED!
// AND push_back() needs to return a bool
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

  // This constructor atomically transfers the
  // entire contents of another queue into this.
  explicit Queue (Queue& other)
    : m_head (&m_null)
    , m_tail (&m_null)
    , m_null (0)
  {
    // worst-case implementation
    for(;;)
    {
      Node* node = other.pop_front();
      if (node)
        push_back (node);
      else
        break;
    }

    // TODO: O(1) thread-safe implementation
  }

  void push_back (Node* node)
  {
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
  }

  Elem* pop_front ()
  {
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
  }

private:
  Atomic::Pointer <Node> m_head;
  Atomic::Pointer <Node> m_tail;
  Node m_null;
};

}

#endif
