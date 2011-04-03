// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LIST_VFHEADER__
#define __VF_LIST_VFHEADER__

// Intrusive list

namespace detail {

struct List_default_tag
{
};

class List_all_features
{
public:
  typedef int size_type;

  size_type size () const { return m_size; }

protected:
  void reset_size () { m_size = 0; }
  void add_size (size_type amount) { m_size += amount; }
  void add_size (const List_all_features& other) { m_size += other.m_size; }

private:
  size_type m_size;
};

}

//------------------------------------------------------------------------------

// intrusive doubly linked list

template <class Elem,
          class Tag = detail::List_default_tag,
          class Features = detail::List_all_features>
class List : public Features
{
public:
  class Node
  {
  public:
    Node () { }

  private:
    Node (const Node& other);
    Node& operator= (const Node& other);

    friend class List;
    friend typename Features;
    Node* m_next;
    Node* m_prev;
  };

public:
  // WHERE IS THE CONST_ITERATOR ???
  class iterator
  {
  public:
    iterator (Node* node = 0) : m_node (node) { }
    iterator (Elem* e) : m_node (static_cast <Node*> (e)) { }
    iterator (const iterator& other) : m_node (other.m_node) { }
    iterator& operator= (const iterator& other) { m_node = other.m_node; return *this; }
    bool operator == (const iterator& other) const { return m_node == other.m_node; }
    bool operator != (const iterator& other) const { return m_node != other.m_node; }

    inline Elem* operator* () const
    {
      vfassert (m_node);
      vfassert (m_node->m_next);
      vfassert (m_node->m_prev);
      return static_cast <Elem*> (m_node);
    }

    inline Elem* operator-> () const
    {
      vfassert (m_node);
      vfassert (m_node->m_next);
      vfassert (m_node->m_prev);
      return static_cast <Elem*> (m_node);
    }

    inline iterator& operator++ ()
    {
      vfassert (m_node);
      vfassert (m_node->m_next);
      m_node = m_node->m_next;
      return *this;
    }

    inline iterator operator++ (int) // postfix
    {
      vfassert (m_node);
      vfassert (m_node->m_next);
      m_node = m_node->m_next;
      return m_node->m_prev;
    }

    inline iterator& operator-- ()
    {
      vfassert (m_node);
      vfassert (m_node->m_prev);
      vfassert (m_node->m_prev->m_prev);
      m_node = m_node->m_prev;
      return *this;
    }

    inline iterator operator-- (int) // postfix
    {
      vfassert (m_node);
      vfassert (m_node->m_prev);
      vfassert (m_node->m_prev->m_prev);
      m_node = m_node->m_prev;
      return m_node->m_next;
    }

    /*@implementation*/
    inline Node* node () const { return m_node; }

  private:
    Node* m_node;
  };

public:
  List ()
  {
    m_head.m_prev = 0; // identifies the head
    m_tail.m_next = 0; // identifies the tail
    reset ();
  }

  explicit List (List& other)
  {
    m_head.m_prev = 0; // identifies the head
    m_tail.m_next = 0; // identifies the tail
    reset ();

    append (other);
  }

  bool empty () const       { return m_head.m_next == &m_tail; }
  iterator begin ()         { return m_head.m_next; }
  iterator end ()           { return &m_tail; }
  Elem* front ()            { return empty() ? 0 : static_cast <Elem*> (m_head.m_next); }
  Elem* back ()             { return empty() ? 0 : static_cast <Elem*> (m_tail.m_prev); }
  Elem* pop_front ()        { Elem* e = front (); if (e) remove (e); return e; }
  Elem* pop_back ()         { Elem* e = back ();  if (e) remove (e); return e; }
  void push_front (Elem* e) { insert (begin(), e); }
  void push_back (Elem* e)  { insert (end(), e); }
  void prepend (List& list) { insert (begin(), list); }
  void append (List& list)  { insert (end(), list); }
  void to_front (Elem* e)   { remove (e); push_front (e); }
  void to_back (Elem* e)    { remove (e); push_back (e); }
 
  void reset ()
  {
    m_head.m_next = &m_tail;
    m_tail.m_prev = &m_head;
    Features::reset_size ();
  }

  iterator insert (iterator pos, Elem* e)
  {
    Node* node = static_cast <Node*> (e);
    node->m_next = pos.node();
    node->m_prev = node->m_next->m_prev;
    node->m_next->m_prev = node;
    node->m_prev->m_next = node;
    Features::add_size (1);
    return node;
  }

  void insert (iterator pos, List& other)
  {
    if (!other.empty())
    {
      Node* before = pos.node();
      other.m_head.m_next->m_prev = before->m_prev;
      before->m_prev->m_next = other.m_head.m_next;
      other.m_tail.m_prev->m_next = before;
      before->m_prev = other.m_tail.m_prev;
      Features::add_size (other);
      other.reset ();
    }
  }

  iterator remove (iterator pos)
  {
    Elem* e = *pos++;
    Node* node = static_cast <Node*> (e);
    node->m_next->m_prev = node->m_prev;
    node->m_prev->m_next = node->m_next;
    Features::add_size (-1);
    return pos;
  }

  // messes up size()
  /*
  static void remove (Elem* e)
  {
    Node* node = static_cast <Node*> (e);
    node->m_next->m_prev = node->m_prev;
    node->m_prev->m_next = node->m_next;
  }
  */

  void swap (List& other)
  {
    List temp;
    temp.append (other);
    other.append (*this);
    append (temp);
  }

private:
  List& operator= (const List& other);

  Node m_head;
  Node m_tail;
};

#endif
