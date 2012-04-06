// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_LOCKFREEQUEUE_VFHEADER
#define VF_LOCKFREEQUEUE_VFHEADER

struct LockFreeQueueDefaultTag { };

/***
  Multiple Producer, Single Consumer (MPSC) intrusive FIFO.

  This implementation is wait-free for producers and lock-free
  for consumers.

  Invariants:

  #1 Any thread may call push_back() at any time (Multiple Producer).

  #2 Only one thread may call try_pop_front() at a time (Single Consumer)

  #3 The queue is signaled if there are one or more elements.

  The caller is responsible for preventing the ABA problem.
*/
template <class Element, class Tag = LockFreeQueueDefaultTag>
class LockFreeQueue
{
public:
  /* Base class for objects which may be placed into this queue.
     
     Derive your elements from this class. To put objects into more
     than one queue at once, use different Tag types and inherit
     from each Node.
   */
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
  LockFreeQueue ()
    : m_head ((Node*)m_null)
    , m_tail ((Node*)m_null)
    , m_null (nullptr)
  {
  }

  /** Returns true if the queue is empty.

      This is not thread safe, and the caller must synchronize.
  */
  bool empty () const
  {
    return (m_head->get () == m_tail);
  }

  /** Place an element at the back of the queue, returning true if
      the queue was previously empty.

      The call is wait-free.
  */
  bool push_back (Node* node)
  {
    node->m_next.set (0);

    Node* prev = m_head->exchange (node);

    // (*) If a try_pop_front() happens at this point, it might not see the
    //     element we are pushing. This only happens when the list is empty,
    //     and furthermore it is detectable.

    prev->m_next.set (node);

    return prev == m_null;
  }

  /** Pop an element from the front of the queue, returning nullptr
      if the queue is empty.

      The call is lock-free.
  */
  Element* pop_front ()
  {
    Element* elem;

    // Avoid the SpinDelay ctor if possible
    if (!try_pop_front (&elem))
    {
      SpinDelay delay;
      do
      {
        delay.pause ();
      }
      while (!try_pop_front (&elem));
    }

    return elem;
  }

  /** Attempt to pop an element from the front of the queue, returning
      true if the operation was successful.

      A pop operation is successful if there is no contention for the queue.
      On success, the value of *pElem is set to the popped element if it
      exists, or nullptr if the queue was empty. On failure, the value
      of *pElem is undefined.

      This call is wait-free.
  */
  bool try_pop_front (Element** pElem)
  {
    Node* tail = m_tail;
    Node* next = tail->m_next.get ();

    if (tail == m_null)
    {
      if (next == 0)
      {
        // (*) If a push_back() happens at this point,
        //     we might not see the element.

        if (m_head->get() == tail)
        {
          *pElem = nullptr;
          return true; // success, but queue empty
        }
        else
        {
          return false; // failure: a push_back() caused contention
        }
      }

      m_tail = next;
      tail = next;
      next = next->m_next.get ();
    }

    if (next)
    {
      m_tail = next;
      *pElem = static_cast <Element*> (tail);
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
        *pElem = static_cast <Element*> (tail);
        return true;
      }
    }

    // (*) If a push_back() happens at this point,
    //     we might not see the element.

    if (head == m_tail)
    {
      *pElem = nullptr;
      return true; // success, but queue empty
    }
    else
    {
      return false; // failure: a push_back() caused contention
    }
  }

private:
  // Elements are pushed on to the head and popped from the tail.
  CacheLine::Unpadded <AtomicPointer <Node> > m_head;
  CacheLine::Unpadded <Node*> m_tail;
  CacheLine::Unpadded <Node> m_null;
};

#endif
