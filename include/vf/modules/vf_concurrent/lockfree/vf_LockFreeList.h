// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_LOCKFREELIST_VFHEADER__
#define __VF_LOCKFREELIST_VFHEADER__

namespace detail {

namespace LockFree {

struct List_default_tag { };

}

}

namespace LockFree {

//
// Lock-free intrusive singly linked list basics
//


template <class Elem,
  class Tag = detail::LockFree::List_default_tag>
struct List
{
  class Node : Uncopyable
  {
  public:
    Node () { }
    explicit Node (Node* next) : m_next (next) { }

    AtomicPointer <Node> m_next;
  };
};

}

#endif
