// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_LOCKFREESTACK_VFHEADER__
#define __VF_LOCKFREESTACK_VFHEADER__

#include "vf/modules/vf_core/memory/vf_Atomic.h"
#include "vf/modules/vf_concurrent/lockfree/vf_LockFreeList.h"

#define STACK_USE_DELAY 0

namespace LockFree {

//
// Lock-free intrusive stack
//
// Caller is responsible for preventing the ABA problem.
//
// The use of LockFree::Allocator for stack elements will
// fulfill this requirement.
//

template <class Elem,
  class Tag = detail::LockFree::List_default_tag>
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
    bool first;
    Node* head;

#if STACK_USE_DELAY
    head = m_head.get();
    first = head == 0;
    node->m_next = head;

    if (!m_head.compareAndSet (node, head))
    {
      Delay delay;
      do
      {
        head = m_head.get();
        first = head == 0;
        node->m_next = head;
      }
      while (!m_head.compareAndSet (node, head));
    }

#else
    do
    {
      head = m_head.get();
      first = head == 0;
      node->m_next = head;
    }
    while (!m_head.compareAndSet (node, head));

#endif

    return first;
  }

  Elem* pop_front ()
  {
    Node* node;
    Node* head;

#if STACK_USE_DELAY
    node = m_head.get();
    if (node != 0)
    {
      head = node->m_next.get();
      if (!m_head.compareAndSet (head, node))
      {
        do
        {
          node = m_head.get();
          if (node == 0)
            break;
          head = node->m_next.get();
        }
        while (!m_head.compareAndSet (head, node));
      }
    }

#else
    do
    {
      node = m_head.get();
      if (node == 0)
        break;
      head = node->m_next.get();
    }
    while (!m_head.compareAndSet (head, node));

#endif

    return node ? static_cast <Elem*> (node) : 0;
  }

  // Push another stack onto this stack. The
  // other stack is emptied. This operation is
  // atomic with respect to the other stack.
  // Not thread safe for this stack.
  // Caller must synchronize.
  void push_front (Stack& other)
  {
    Stack copy (other);
    for(;;)
    {
      Elem* elem = copy.pop_front ();
      if (elem)
        push_front (elem);
      else
        break;
    }
  }

  // Swap contents with another stack.
  // Not thread safe or atomic.
  // Caller must synchronize.
  void swap (Stack& other)
  {
    Node* temp = other.m_head.get ();
    other.m_head.set (m_head.get ());
    m_head.set (temp);
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
};

}

#endif
