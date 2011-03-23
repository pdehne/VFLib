// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREEQUEUE_VFHEADER__
#define __VF_LOCKFREEQUEUE_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeList.h"

#define QUEUE_USE_MUTEX 0
#if QUEUE_USE_MUTEX
#include "vf/vf_Mutex.h"
#endif

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
// It is the responsibility of the caller to
// synchronize access to pop_front().
//

template <class Elem,
          class Tag = detail::List_default_tag>
class Queue
{
public:
#if QUEUE_USE_MUTEX
  Mutex m_mutex;
#endif

  typedef typename List <Elem, Tag>::Node Node;

  Queue ()
    : m_head (&m_null)
    , m_tail (&m_null)
    , m_null (0)
  {
  }

  // Not thread safe.
  // Caller must synchronize.
  bool empty () const
  {
    return (m_head.get () == m_tail);
  }

  void push_back (Node* node)
  {
#if QUEUE_USE_MUTEX
    ScopedLock lock (m_mutex);
#endif

    node->m_next.set (0);

    Node* prev = m_head.exchange (node);

    // (*) If we get pre-empted here, then
    //     pop_front() might not see this element.
    //
    // This only happens when the list is empty.

    prev->m_next.set (node);
  }

  Elem* pop_front ()
  {
#if QUEUE_USE_MUTEX
    ScopedLock lock (m_mutex);
#endif

    Node* tail = m_tail;
    Node* next = tail->m_next.get ();

    if (tail == &m_null)
    {
      if (next == 0)
      {
        // (*) If push_back() is at the magic
        //     spot, we might not see it's element.

        return 0;
      }

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
  // Note that items are pushed on to the head
  // popped from the tail, the reverse of expected.
  Atomic::Pointer <Node> m_head;
  Node* m_tail;
  Node m_null;
};

}

#endif
