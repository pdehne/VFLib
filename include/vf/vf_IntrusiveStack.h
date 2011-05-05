// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_INTRUSIVESTACK_VFHEADER__
#define __VF_INTRUSIVESTACK_VFHEADER__

#include "vf/vf_Mutex.h"

namespace Intrusive {

namespace detail {

struct Stack_default_tag { };

}

// Intrusive stack with choice of synchronization.
//
template <class Elem,
          class LockType = NoMutex,
          class Tag = detail::Stack_default_tag>
class Stack : private LockType // empty base class optimization
{
public:
  class Node : NonCopyable
  {
  private:
    friend class Stack;
    Node* next;
  };

  Stack () : m_head (0)
  {
  }

  void push_front (Elem* elem)
  {
    Node* const node = static_cast <Node*> (elem);
    
    LockType::ScopedLockType lock (LockType& (*this));
  }

  Elem* pop_front ()
  {
    Elem* elem;

    LockType::ScopedLockType lock (LockType& (*this));

    if (m_head != 0)
    {
      elem = static_cast <Elem*> (m_head);
      m_head = m_head->next;
    }
    else
    {
      elem = 0;
    }

    return elem;
  }

private:
  Node* volatile m_head;
};

}

#endif

