// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREE_VFHEADER__
#define __VF_LOCKFREE_VFHEADER__

//
// Lock-free data structure implementations
//

namespace LockFree {

namespace detail {

struct List_default_tag { };

}

//------------------------------------------------------------------------------

//
// Intrusive singly linked list basics
//

template <class Elem,
          class Tag = detail::List_default_tag>
struct List
{
  class Node : NonCopyable
  {
  public:
    Node () { }
    explicit Node (Node* next) : m_next (next) { }

    VF_JUCE::Atomic <Node*> m_next;
  };
};

//------------------------------------------------------------------------------

//
// Lock-free intrusive stack
//
// This implementation requires that Nodes are never deleted, only re-used.
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

  // This constructor atomically transfers the
  // entire contents of another stack into this.
  explicit Stack (Stack& other)
  {
    Node* head;

    do
    {
      head = other.m_head.get();
    }
    while (!other.m_head.compareAndSetBool (0, head));

    m_head = head;
  }

  void push_front (Node* node)
  {
    Node* head;

    do
    {
      head = m_head.get();

      node->m_next = head;
    }

    while (!m_head.compareAndSetBool (node, head));
  }

  Elem* pop_front ()
  {
    Node* node;
    Node* head;

    do
    {
      node = m_head.get();

      if (node == 0)
        break;

      head = node->m_next.get();
    }
    while (!m_head.compareAndSetBool (node, head));

    return static_cast <Elem*> (node);
  }

  // Reverse the order of all items.
  // This is not thread-safe.
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
  VF_JUCE::Atomic <Node*> m_head;
};

//------------------------------------------------------------------------------

//
// Lock-free intrusive queue
//
// This implementation requires that Nodes are never deleted, only re-used.
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
          if (m_tail.get()->m_next.compareAndSetBool (0, node))
            break;
        }
        else
        {
          m_tail.compareAndSetBool (tail, next);
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
            m_tail.compareAndSetBool (tail, next);
          }
        }
        else if (m_head.compareAndSetBool (head, next))
        {
          break;
        }
      }
    }

    return static_cast <Elem*> (next);
  }

private:
  VF_JUCE::Atomic <Node*> m_head;
  VF_JUCE::Atomic <Node*> m_tail;
  Node m_null;
};

}

#endif
