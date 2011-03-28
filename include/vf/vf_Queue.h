// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_QUEUE_VFHEADER__
#define __VF_QUEUE_VFHEADER__

#include "vf/vf_LockFreeList.h"
#include "vf/vf_Mutex.h"

//
// Intrusive singly linked fifo
//

template <class Elem,
          class Tag = LockFree::detail::List_default_tag>
class Queue
{
public:
  typedef typename LockFree::List <Elem, Tag>::Node Node;

  Queue ()
    : m_head ((Node*)m_null)
    , m_tail ((Node*)m_null)
    , m_null ((Node*)0)
  {
  }

  // Not thread safe.
  // Caller must synchronize.
  bool empty () const
  {
    return (m_head->get () == m_tail);
  }

  // Returns true if the queue became signaled from the push.
  bool push_back (Node* node)
  {
    node->m_next.set (0);

    Node* prev = m_head->exchange (node);

    // (*) If we get pre-empted here, then
    //     pop_front() might not see this element.
    //
    // This only happens when the list is empty.

    prev->m_next.set (node);

    return prev == m_null;
  }

  Elem* pop_front ()
  {
    Elem* elem;

    // Crafted to sometimes avoid the Delay ctor.
    if (!try_pop_front (&elem))
    {
      Delay delay;
      do
      {
        delay.spin ();
      }
      while (!try_pop_front (&elem));
    }

    return elem;
  }

  // Returns true on success.
  bool try_pop_front (Elem** pElem)
  {
    Node* tail = m_tail;
    Node* next = tail->m_next.get ();

    if (tail == m_null)
    {
      if (next == 0)
      {
        // (*) If push_back() is at the magic
        //     spot, we might not see it's element.
        //     This situation is detectable, and counts
        //     as a 'failure'. The caller decides what to do.

        if (m_head->get() == tail)
        {
          *pElem = 0;
          return true;
        }
        else
        {
          return false; // busy
        }
      }

      m_tail = next;
      tail = next;
      next = next->m_next.get ();
    }

    if (next)
    {
      m_tail = next;
      *pElem = static_cast <Elem*> (tail);
      return true;
    }

    Node* head = m_head->get ();

    if (tail == head)
    {
      push_back (m_null);
      next = tail->m_next.get();
      if (next)
      {
        m_tail = next;
        *pElem = static_cast <Elem*> (tail);
        return true;
      }
    }

    // (*)

    if (head == m_tail)
    {
      *pElem = 0;
      return true;
    }
    else
    {
      return false; // busy
    }
  }

private:
  Mutex m_mutex;
  NoCacheLinePadding <Atomic::Pointer <Node> > m_head;
  NoCacheLinePadding <Node*> m_tail;
  NoCacheLinePadding <Node> m_null;
};

}

#endif
