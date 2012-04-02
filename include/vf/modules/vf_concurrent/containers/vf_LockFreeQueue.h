// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_LOCKFREEQUEUE_VFHEADER
#define VF_LOCKFREEQUEUE_VFHEADER

struct LockFreeQueueDefaultTag { };

/***
  Mostly wait-free MPSC (Multiple Producer, Single Consumer) intrusive FIFO.

  Invariants:
	#1 Any thread may call push_back() at any time (Multiple Producer).

	#2 Only one thread may call try_pop_front() at a time (Single Consumer)

	#3 The queue is signaled if there are one or more elements.
*/
template <class Element, class Tag = LockFreeQueueDefaultTag>
class LockFreeQueue
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
  LockFreeQueue ()
    : m_head ((Node*)m_null)
    , m_tail ((Node*)m_null)
    , m_null (nullptr)
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
  Element* pop_front ()
  {
    Element* elem;

    // Crafted to sometimes avoid the Delay ctor.
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

  // Returns true on success.
  // Wait-free.
  //
  bool try_pop_front (Element** pElem)
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
  CacheLine::Unpadded <AtomicPointer <Node> > m_head;
  CacheLine::Unpadded <Node*> m_tail;
  CacheLine::Unpadded <Node> m_null;
};

#endif
