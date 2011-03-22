// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LOCKFREELIST_VFHEADER__
#define __VF_LOCKFREELIST_VFHEADER__

#include "vf/vf_Atomic.h"

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

}

#endif
