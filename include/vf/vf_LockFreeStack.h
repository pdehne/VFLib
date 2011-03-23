// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREESTACK_VFHEADER__
#define __VF_LOCKFREESTACK_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_LockFreeList.h"

#define STACK_USE_MUTEX 0

#if STACK_USE_MUTEX
#include "vf/vf_Mutex.h" // debugging
#endif

namespace LockFree {

//
// Lock-free intrusive stack
//
// This implementation requires that Nodes are never deleted, only re-used.
//
// As an added feature, push_front() returns true if it causes the stack
// to go from empty to non-empty. This greatly assists the implementation
// of other objects.
//

template <class Elem,
          class Tag = detail::List_default_tag>
class Stack
{
public:
  typedef typename List <Elem, Tag>::Node Node;

public:
  Stack () : m_head (0)
  {
  }

  // This constructor atomically acquires the contents
  // of the other stack. The other stack is cleared.
  explicit Stack (Stack& other)
  {
#if STACK_USE_MUTEX
    ScopedLock lock (other.m_mutex);
#endif

    Node* head;

    do
    {
      head = other.m_head.get();
    }
    while (!other.m_head.compareAndSet (0, head));

    m_head = head;
  }

  // Not thread safe.
  // Caller must synchronize.
  bool empty () const
  {
    return m_head.get() == 0;
  }

  // returns true if it pushed the first element
  bool push_front (Node* node)
  {
#if STACK_USE_MUTEX
    ScopedLock lock (m_mutex);
#endif

    bool first;
    Node* head;

    do
    {
      head = m_head.get();

      first = head == 0;

      node->m_next = head;
    }
    while (!m_head.compareAndSet (node, head));

    return first;
  }

  Elem* pop_front ()
  {
#if STACK_USE_MUTEX
    ScopedLock lock (m_mutex);
#endif

    Node* node;
    Node* head;

    do
    {
      node = m_head.get();

      if (node == 0)
        break;

      head = node->m_next.get();
    }
    while (!m_head.compareAndSet (head, node));

    return static_cast <Elem*> (node);
  }

  // Reverse the order of all items.
  // Not thread safe.
  // Caller must synchronize.
  void reverse ()
  {
    Stack s (*this);

    for (;;)
    {
      Node* node = s.pop_front ();
      if (node != 0)
        push_front (node);
      else
        break;
    }
  }

private:
  Atomic::Pointer <Node> m_head;

#if STACK_USE_MUTEX
  Mutex m_mutex;
#endif
};

}

#endif
