// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREE_VFHEADER__
#define __VF_LOCKFREE_VFHEADER__

#include "vf/vf_Atomic.h"
#include "vf/vf_Intrinsics.h"
#include "vf/vf_Mutex.h"
#include "vf/vf_Thread.h"

//
// Lock-free data structure implementations
//

namespace LockFree {

//
// Lock-free intrusive singly linked list basics
//

namespace detail {

struct List_default_tag { };

}

template <class Elem,
          class Tag = detail::List_default_tag>
struct List
{
  class Node : NonCopyable
  {
  public:
    Node () { }
    explicit Node (Node* next) : m_next (next) { }

    Atomic::Pointer <Node> m_next;
  };
};

//------------------------------------------------------------------------------

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
    while (!m_head.compareAndSet (head, node));

    return static_cast <Elem*> (node);
  }

  // Not thread safe.
  // Caller must synchronize.
  bool empty () const
  {
    return m_head.get() == 0;
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

//------------------------------------------------------------------------------

//
// Lock-free intrusive queue
//
// This implementation requires that Nodes are never deleted, only re-used.
//

//
// TODO: THIS SEEMS FLAWED!
// AND push_back() needs to return a bool
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
          if (m_tail.get()->m_next.compareAndSet (node, 0))
            break;
        }
        else
        {
          m_tail.compareAndSet (next, tail);
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
            m_tail.compareAndSet (next, tail);
          }
        }
        else if (m_head.compareAndSet (next, head))
        {
          break;
        }
      }
    }

    return static_cast <Elem*> (next);
  }

private:
  Atomic::Pointer <Node> m_head;
  Atomic::Pointer <Node> m_tail;
  Node m_null;
};

//------------------------------------------------------------------------------

//
// Mostly Lock-free allocator
//
// Use of this allocator ensures the correctness of
// the containers in the LockFree implementation.
//
// It is required that Elem be derived from a LockFree::List::Node.
// If no tag is specified, the default tag is used.
//
template <class Elem,
          class Tag = detail::List_default_tag>
class Allocator
{
public:
  ~Allocator ()
  {
    for(;;)
    {
      Elem* elem = m_free.pop_front ();
      if (elem)
        ::operator delete (elem); // implicit global mutex
      else
        break;
    }
  }

  Elem* New ()
    { return new (alloc()) Elem(); }

  template <class T1>
  Elem* New (T1 t1)
    { return new (alloc()) Elem(t1); }

  template <class T1, class T2>
  Elem* New (T1 t1, T2 t2)
    { return new (alloc()) Elem(t1, t2); }

  template <class T1, class T2, class T3>
  Elem* New (T1 t1, T2 t2, T3 t3)
    { return new (alloc()) Elem(t1, t2, t3); }

  template <class T1, class T2, class T3, class T4>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4)
    { return new (alloc()) Elem(t1, t2, t3, t4); }

  template <class T1, class T2, class T3, class T4, class T5>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
    { return new (alloc()) Elem(t1, t2, t3, t4, t5); }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
    { return new (alloc()) Elem(t1, t2, t3, t4, t5, t6); }

  void Delete (Elem* e)
  {
    e->~Elem();
    free (e);
  }

private:
  void* alloc ()
  {
    void* p = m_free.pop_front();

    if (!p)
    {
      p = ::operator new (sizeof(Elem)); // implicit global mutex

#if 0
#if VF_DEBUG
      m_count.addref ();
      String s;
      s << "m_count = " << String (m_count.get_value());
      Logger::outputDebugString (s);
#endif
#endif
    }

    return p;
  }

  void free (Elem* elem)
  {
    m_free.push_front (elem);
  }

private:
  Stack <Elem, Tag> m_free;
#if 0
#if VF_DEBUG
  Atomic::UsageCounter m_count;
#endif
#endif
};

//------------------------------------------------------------------------------

//
// Standard allocator has the same interface as the
// Lock-Free allocator but doesn't use a deleted list.
// It is not compatible with lock-free containers.
//
// AVOID
/* 
template <class Elem,
          class Tag = detail::List_default_tag>
class StandardAllocator
{
public:
  StandardAllocator ()
  {
  }

  Elem* New ()
    { return new Elem(); }

  template <class T1>
  Elem* New (T1 t1)
    { return new Elem(t1); }

  template <class T1, class T2>
  Elem* New (T1 t1, T2 t2)
    { return new Elem(t1, t2); }

  template <class T1, class T2, class T3>
  Elem* New (T1 t1, T2 t2, T3 t3)
    { return new Elem(t1, t2, t3); }

  template <class T1, class T2, class T3, class T4>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4)
    { return new Elem(t1, t2, t3, t4); }

  template <class T1, class T2, class T3, class T4, class T5>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
    { return new Elem(t1, t2, t3, t4, t5); }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  Elem* New (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
    { return new Elem(t1, t2, t3, t4, t5, t6); }

  void Delete (Elem* e)
  {
    delete e;
  }
};
*/

//------------------------------------------------------------------------------

//
// Synchronization delay element which avoids kernel blocking
//
class Delay
{
public:
  Delay ()
    : m_backoff (0)
  {
  }

#if 0
  ~Spinner ()
  {
    if (m_backoff > 1)
    {
      String s;
      s << "m_backoff = " << String (m_backoff);
      Logger::outputDebugString (s);
    }
  }
#endif

  inline void spin ()
  {
    if (m_backoff < 10)
    {
      Intrinsic::mm_pause <1> ();
    }
    else if (m_backoff < 20)
    {
      Intrinsic::mm_pause <50> ();
    }
    else if (m_backoff < 22)
    {
      CurrentThread::yield();
    }
    else if (m_backoff < 24)
    {
      CurrentThread::sleep (0);
    }
    else if (m_backoff < 26)
    {
      CurrentThread::sleep (1);
    }
    else
    {
      CurrentThread::sleep (10);
    }

    ++m_backoff;
  }

private:
  int m_backoff;
};

//------------------------------------------------------------------------------

// Multiple-reader, single writer, write preferenced
// recursive mutex with a lock-free fast path.
class ReadWriteMutex
{
public:
  ReadWriteMutex ();
  ~ReadWriteMutex ();

  // recursive
  void enter_read ();
  void exit_read ();

  // recursive
  void enter_write ();
  void exit_write ();

private:
  Mutex m_mutex;
  Atomic::UsageCounter m_writes;
  Atomic::UsageCounter m_readers;
};

}

#endif
