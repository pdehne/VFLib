// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_LOCKFREESTACK_VFHEADER
#define VF_LOCKFREESTACK_VFHEADER

#include "../memory/vf_AtomicPointer.h"

//==============================================================================
/** Multiple Producer, Multiple Consumer (MPMC) intrusive stack.

    This stack is implemented using the same intrusive interface as List. All
    operations are lock-free.

    The caller is responsible for preventing the "ABA" problem
      (http://en.wikipedia.org/wiki/ABA_problem)

    @param Tag  A type name used to distinguish lists and nodes, for
                putting objects in multiple lists. If this parameter is
                omitted, the default tag is used.
*/

#ifndef DOXYGEN
struct LockFreeStackDefaultTag { };
#endif

template <class Element, class Tag = LockFreeStackDefaultTag>
class LockFreeStack : Uncopyable
{
public:
#ifndef DOXYGEN
  class Node : Uncopyable
  {
  public:
    Node ()
	{
	}

	explicit Node (Node* next) : m_next (next)
	{
	}
  
  private:
    friend class LockFreeStack;

    AtomicPointer <Node> m_next;
  };
#endif

public:
  LockFreeStack () : m_head (0)
  {
  }

  /** Create a LockFreeStack from another stack.

      The contents of the other stack are atomically acquired.
      The other stack is cleared.

      @param other  The other stack to acquire.
  */
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

  /** Push a node onto the stack.

      The caller is responsible for preventing the ABA problem. This operation
      is lock-free. 

      @param node The node to push.

      @return     True if the stack was previously empty. If multiple threads
                  are attempting to push, only one will receive true.
  */
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

  /** Pop an element off the stack.

      The caller is responsible for preventing the ABA problem. This operation
      is lock-free. 

      @return   The element that was popped, or nullptr if the stack was empty.
  */
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

  /** Swap the contents of this stack with another stack.

      This call is not thread safe or atomic. The caller is responsible for
      synchronizing access.

      @param other  The other stack to swap contents with.
  */
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
