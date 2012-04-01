// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_LOCKFREESTACK_VFHEADER__
#define __VF_LOCKFREESTACK_VFHEADER__

struct LockFreeStackDefaultTag { };

/***
  Lock-free intrusive stack.

  The caller is responsible for preventing the "ABA" problem.
*/
template <class Element, class Tag = LockFreeStackDefaultTag>
class LockFreeStack : Uncopyable
{
public:
  class Node : Uncopyable
  {
  public:
    Node ()
	{
	}

	explicit Node (Node* next) : m_next (next)
	{
	}

    AtomicPointer <Node> m_next;
  };

public:
  LockFreeStack () : m_head (0)
  {
  }

  // This constructor atomically acquires the contents
  // of the other stack. The other stack is cleared.
  explicit LockFreeStack (LockFreeStack& other)
  {
    Node* head;

    do
    {
      head = other.m_head.get();
    }
    while (!other.m_head.compareAndSet (0, head));

    m_head = head;
  }

  // returns true if it pushed the first element
  bool push_front (Node* node)
  {
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

  Element* pop_front ()
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
    while (!m_head.compareAndSet (head, node));

    return node ? static_cast <Element*> (node) : nullptr;
  }

  // Swap contents with another stack.
  // Not thread safe or atomic.
  // Caller must synchronize.
  void swap (LockFreeStack& other)
  {
    Node* temp = other.m_head.get ();
    other.m_head.set (m_head.get ());
    m_head.set (temp);
  }

private:
  AtomicPointer <Node> m_head;
};

#endif
