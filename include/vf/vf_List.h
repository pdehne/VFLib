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
  class Node : NonCopyable
  {
  public:
    Node () { }

  private:
    friend class List;
    friend typename Features;
    Node* m_next;
    Node* m_prev;
  };

private:
  template <class ElemType, class NodeType>
  class iterator_base
    : public boost::iterator_facade <
      iterator_base <ElemType, NodeType>,
      ElemType,
      boost::bidirectional_traversal_tag
    >
  {
  public:
    iterator_base (NodeType* node = 0)
      : m_node (node)
      { }
    
    iterator_base (ElemType* e)
      : m_node (static_cast <NodeType*> (e))
      { }
 
    template <class OtherElemType, class OtherNodeType>
    iterator_base (iterator_base <OtherElemType, OtherNodeType> const& other)
      : m_node (other.m_node)
      { }

    template <class OtherElemType, class OtherNodeType>
    iterator_base& operator= (iterator_base <OtherElemType, OtherNodeType> const& other)
      { m_node = other.m_node; return *this; }

    template <class OtherElemType, class OtherNodeType>
    bool operator == (iterator_base <OtherElemType, OtherNodeType> const& other) const
      { return m_node == other.m_node; }

    template <class OtherElemType, class OtherNodeType>
    bool operator != (iterator_base <OtherElemType, OtherNodeType> const& other) const
      { return m_node != other.m_node; }

    operator ElemType* () const
      { return static_cast <ElemType*> (m_node); }
    
  private:
    friend class boost::iterator_core_access;

    reference dereference () const
    {
      return *static_cast <ElemType*> (m_node);
    }

    bool equal (NodeType *const *node) const
    {
      return m_node == node;
    }

    void increment ()
    {
      vfassert (m_node->m_next);
      m_node = m_node->m_next;
    }

    void decrement ()
    {
      vfassert (m_node->m_prev && m_node->m_prev->m_prev != 0);
      m_node = m_node->m_prev;
    }

  private:
    NodeType* m_node;
  };

public:
  typedef iterator_base <Elem, Node> iterator;
  typedef iterator_base <Elem const, Node const> const_iterator;

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

  bool empty () const           { return m_head.m_next == &m_tail; }
  iterator begin ()             { return m_head.m_next; }
  iterator end ()               { return &m_tail; }
  const_iterator begin () const { return m_head.m_next; }
  const_iterator end () const   { return &m_tail; }
  Elem* front () const          { return empty() ? 0 : static_cast <Elem*> (m_head.m_next); }
  Elem* back () const           { return empty() ? 0 : static_cast <Elem*> (m_tail.m_prev); }
  Elem* pop_front ()            { Elem* e = front ();
                                  if (e) erase (e); return e; }
  Elem* pop_back ()             { Elem* e = back ();
                                  if (e) erase (e); return e; }
  void push_front (Elem* e)     { insert (begin(), e); }
  void push_back (Elem* e)      { insert (end(), e); }
  void prepend (List& list)     { insert (begin(), list); }
  void append (List& list)      { insert (end(), list); }
  void to_front (Elem* e)       { erase (e); push_front (e); }
  void to_back (Elem* e)        { erase (e); push_back (e); }
 
  iterator iterator_to (Elem& elem) const
  {
    return static_cast <Node*> (&elem);
  }

  const_iterator const_iterator_to (Elem& elem) const
  {
    return static_cast <Node*> (&elem);
  }

  void clear ()
  {
    m_head.m_next = &m_tail;
    m_tail.m_prev = &m_head;
    Features::reset_size ();
  }

  // DEPRECATED
  inline void reset ()
  {
    clear ();
  }

  iterator insert (iterator pos, Elem* e)
  {
    Node* node = static_cast <Node*> (e);
    node->m_next = pos;
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
      Node* before = pos;
      other.m_head.m_next->m_prev = before->m_prev;
      before->m_prev->m_next = other.m_head.m_next;
      other.m_tail.m_prev->m_next = before;
      before->m_prev = other.m_tail.m_prev;
      Features::add_size (other);
      other.reset ();
    }
  }

  iterator erase (iterator pos)
  {
    Elem* e = pos++;
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
