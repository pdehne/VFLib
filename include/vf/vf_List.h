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

}

//------------------------------------------------------------------------------

// intrusive doubly linked list

template <class Elem,
          class Tag = detail::List_default_tag>
class List : NonCopyable
{
public:
  typedef int size_type;

  typedef Elem        value_type;
  typedef Elem&       reference;
  typedef Elem const& const_reference;
  typedef Elem*       pointer;
  typedef Elem const* const_pointer;

  class Node : NonCopyable
  {
  public:
    Node () { }

  private:
    friend class List;
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
  private:
    explicit iterator_base (NodeType* node) : m_node (node)
    {
    }

  public:
    iterator_base () : m_node (0)
    {
    }

    template <class OtherElemType, class OtherNodeType>
    iterator_base (iterator_base <OtherElemType, OtherNodeType> const& other)
      : m_node (other.m_node)
    {
    }

    template <class OtherElemType, class OtherNodeType>
    iterator_base& operator= (iterator_base <OtherElemType, OtherNodeType> const& other)
    {
      m_node = other.m_node;
      return *this;
    }

    template <class OtherElemType, class OtherNodeType>
    bool operator == (iterator_base <OtherElemType, OtherNodeType> const& other) const
    {
      return m_node == other.m_node;
    }

    template <class OtherElemType, class OtherNodeType>
    bool operator != (iterator_base <OtherElemType, OtherNodeType> const& other) const
    {
      return ! this->operator== (other);
    }

    /*
    operator ElemType* () const
    {
      return static_cast <ElemType*> (m_node);
    }
    */
    
  private:
    friend class List;
    friend class boost::iterator_core_access;

    NodeType* get_node ()
    {
      return m_node;
    }

    NodeType const* get_node () const
    {
      return m_node;
    }

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
    : m_size (0)
  {
    m_head.m_prev = 0; // identifies the head
    m_tail.m_next = 0; // identifies the tail
    clear ();
  }

  size_type size () const { return m_size; }

  reference front ()
  {
    if (empty ())
      Throw (Error().fail (__FILE__, __LINE__, Error::noMoreData));
    return element_from (m_head.m_next);
  }

  const_reference front () const
  {
    if (empty ())
      Throw (Error().fail (__FILE__, __LINE__, Error::noMoreData));
    return element_from (m_head.m_next);
  }

  reference back ()
  {
    if (empty ())
      Throw (Error().fail (__FILE__, __LINE__, Error::noMoreData));
    return element_from (m_tail.m_prev);
  }

  const_reference back () const
  {
    if (empty ())
      Throw (Error().fail (__FILE__, __LINE__, Error::noMoreData));
    return element_from (m_tail.m_prev);
  }

  iterator begin ()
  {
    return iterator (m_head.m_next);
  }

  const_iterator begin () const
  {
    return const_iterator (m_head.m_next);
  }

  const_iterator cbegin () const
  {
    return const_iterator (m_head.m_next);
  }

  iterator end ()
  {
    return iterator (&m_tail);
  }

  const_iterator end () const
  {
    return const_iterator (&m_tail);
  }

  const_iterator cend () const
  {
    return const_iterator (&m_tail);
  }

  bool empty () const
  {
    return m_head.m_next == &m_tail;
  }

  void clear ()
  {
    m_head.m_next = &m_tail;
    m_tail.m_prev = &m_head;
    m_size = 0;
  }

  iterator insert (iterator pos, Elem& elem)
  {
    Node* node = node_from (elem);
    node->m_next = pos.get_node ();
    node->m_prev = node->m_next->m_prev;
    node->m_next->m_prev = node;
    node->m_prev->m_next = node;
    ++m_size;
    return iterator (node);
  }

  void insert (iterator pos, List& other)
  {
    if (!other.empty ())
    {
      Node* before = pos.get_node ();
      other.m_head.m_next->m_prev = before->m_prev;
      before->m_prev->m_next = other.m_head.m_next;
      other.m_tail.m_prev->m_next = before;
      before->m_prev = other.m_tail.m_prev;
      m_size += other.m_size;
      other.clear ();
    }
  }

  iterator erase (iterator pos)
  {
    Node* node = pos.get_node ();
    ++pos;
    node->m_next->m_prev = node->m_prev;
    node->m_prev->m_next = node->m_next;
    --m_size;
    return pos;
  }

  void push_front (Elem& elem)
  {
    insert (begin(), elem);
  }

  void pop_front ()
  {
    if (empty ())
      Throw (Error().fail (__FILE__, __LINE__, Error::noMoreData));
    erase (begin ());
  }

  void push_back (Elem& elem)
  {
    insert (end(), elem);
  }

  void pop_back ()
  {
    if (empty ())
      Throw (Error().fail (__FILE__, __LINE__, Error::noMoreData));
    erase (--end ());
  }

  void swap (List& other)
  {
    List temp;
    temp.append (other);
    other.append (*this);
    append (temp);
  }

  void prepend (List& list)
  {
    insert (begin (), list);
  }

  void append (List& list)
  {
    insert (end(), list);
  }

  iterator iterator_to (Elem& elem) const
  {
    return iterator (static_cast <Node*> (&elem));
  }

  const_iterator const_iterator_to (Elem const& elem) const
  {
    return const_iterator (static_cast <Node const*> (&elem));
  }

private:
  inline reference element_from (Node* node)
  {
    return *(static_cast <pointer> (node));
  }

  inline const_reference element_from (Node const* node) const
  {
    return *(static_cast <const_pointer> (node));
  }

  inline Node* node_from (Elem& elem)
  {
    return static_cast <Node*> (&elem);
  }

  inline Node const* node_from (Elem const& elem) const
  {
    return static_cast <Node const*> (&elem);
  }

private:
  size_type m_size;
  Node m_head;
  Node m_tail;
};

//------------------------------------------------------------------------------

// intrusive doubly linked list

template <class Elem,
          class Tag = detail::List_default_tag>
class ListOld
{
public:
  class Node : NonCopyable
  {
  public:
    Node () { }

  private:
    friend class ListOld;
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
  typedef int size_type;

  ListOld ()
    : m_size (0)
  {
    m_head.m_prev = 0; // identifies the head
    m_tail.m_next = 0; // identifies the tail
    reset ();
  }

  explicit ListOld (ListOld& other)
  {
    m_head.m_prev = 0; // identifies the head
    m_tail.m_next = 0; // identifies the tail
    reset ();

    append (other);
  }

  size_type size () const       { return m_size; }
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
  void prepend (ListOld& list)     { insert (begin(), list); }
  void append (ListOld& list)      { insert (end(), list); }
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
    m_size = 0;
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
    ++m_size;
    return node;
  }

  void insert (iterator pos, ListOld& other)
  {
    if (!other.empty())
    {
      Node* before = pos;
      other.m_head.m_next->m_prev = before->m_prev;
      before->m_prev->m_next = other.m_head.m_next;
      other.m_tail.m_prev->m_next = before;
      before->m_prev = other.m_tail.m_prev;
      m_size += other.m_size;
      other.reset ();
    }
  }

  iterator erase (iterator pos)
  {
    Elem* e = pos++;
    Node* node = static_cast <Node*> (e);
    node->m_next->m_prev = node->m_prev;
    node->m_prev->m_next = node->m_next;
    --m_size;
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

  void swap (ListOld& other)
  {
    ListOld temp;
    temp.append (other);
    other.append (*this);
    append (temp);
  }

private:
  ListOld& operator= (const ListOld& other);

  size_type m_size;
  Node m_head;
  Node m_tail;
};

#endif
