// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_LIST_VFHEADER
#define VF_LIST_VFHEADER

struct ListDefaultTag { };

/***
  Intrusive doubly linked list.

  Elements must derive from one or more List<>::Node. "Tag" is used when an
  element is derived from more than one Node, this allows it to exist in
  multiple lists simultaneously.
*/

template <class Element, class Tag = ListDefaultTag>
class List : Uncopyable
{
public:
  typedef int size_type;

  typedef Element        value_type;
  typedef Element&       reference;
  typedef Element const& const_reference;
  typedef Element*       pointer;
  typedef Element const* const_pointer;

  class Node : Uncopyable
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
  class iterator_base : public std::iterator <
	std::bidirectional_iterator_tag, int>
  {
  public:
	typedef Value value_type;
    typedef ElemType* pointer;
	typedef ElemType& reference;
	
    reference operator*() const
	{
	  return dereference ();
	}

    pointer operator->() const
	{
	  return &dereference ();
	}

	iterator_base& operator++()
	{
	  increment ();
	  return *this;
	}

	iterator_base operator++(int)
	{
	  iterator_base result (*this);
	  increment ();
	  return result;
	}

	iterator_base& operator--()
	{
	  decrement ();
	  return *this;
	}

    iterator_base operator--(int)
	{
	  iterator_base result (*this);
	  decrement ();
	  return result;
	}

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
    
  private:
    friend class List;

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
  typedef iterator_base <Element, Node> iterator;
  typedef iterator_base <Element const, Node const> const_iterator;

public:
  List () : m_size (0)
  {
    m_head.m_prev = nullptr; // identifies the head
    m_tail.m_next = nullptr; // identifies the tail
    clear ();
  }

  size_type size () const
  {
	return m_size;
  }

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

  iterator insert (iterator pos, Element& elem)
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

  void push_front (Element& elem)
  {
    insert (begin(), elem);
  }

  void pop_front ()
  {
    if (empty ())
      Throw (Error().fail (__FILE__, __LINE__, Error::noMoreData));
    erase (begin ());
  }

  void push_back (Element& elem)
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

  iterator iterator_to (Element& elem) const
  {
    return iterator (static_cast <Node*> (&elem));
  }

  const_iterator const_iterator_to (Element const& elem) const
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

  inline Node* node_from (Element& elem)
  {
    return static_cast <Node*> (&elem);
  }

  inline Node const* node_from (Element const& elem) const
  {
    return static_cast <Node const*> (&elem);
  }

private:
  size_type m_size;
  Node m_head;
  Node m_tail;
};

#endif
