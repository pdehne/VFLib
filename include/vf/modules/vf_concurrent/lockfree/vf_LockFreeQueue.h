// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEQUEUE_VFHEADER__
#define __VF_LOCKFREEQUEUE_VFHEADER__

#include "vf/modules/vf_core/memory/vf_Atomic.h"
#include "vf/modules/vf_core/memory/vf_CacheLine.h"
#include "vf/modules/vf_concurrent/lockfree/vf_LockFreeDelay.h"
#include "vf/modules/vf_concurrent/lockfree/vf_LockFreeList.h"

namespace LockFree {

//
// Wait-free intrusive queue.
//
// Supports Multiple Producer, Single Consumer (MPSC):
//
// - Any thread may call push_back() at any time.
//
// - Only one thread may call pop_front() at a time.
//
// - Caller must synchronize access to pop_front() and try_pop_front().
//
// - The queue is considered signaled if there are one or more elements.
//
template <class Elem,
  class Tag = detail::LockFree::List_default_tag>
class Queue
{
public:
  typedef typename List <Elem, Tag>::Node Node;

  Queue ()
    : m_head ((Node*)m_null)
    , m_tail ((Node*)m_null)
    , m_null ((Node*)0)
  {
  }

  // Not thread safe.
  // Caller must synchronize.
  //
  bool empty () const
  {
    return (m_head->get () == m_tail);
  }

  // Returns true if the queue became signaled from the push.
  // Wait-free.
  //
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

  // Tries to pop an element until it succeeds, or the queue is empty.
  // Lock-free.
  //
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
  // Wait-free.
  //
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
  // Elements are pushed on to the head and popped from the tail.
  CacheLine::Unpadded <Atomic::Pointer <Node> > m_head;
  CacheLine::Unpadded <Node*> m_tail;
  CacheLine::Unpadded <Node> m_null;
};

}

#endif
