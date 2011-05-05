// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Atomic.h"
#include "vf/vf_LeakChecked.h"
#include "vf/vf_SharedSingleton.h"

//------------------------------------------------------------------------------

class SharedSingletonBase::PersistentReferences
{
public:
  static void push_front (SharedSingletonBase* instance)
  {
    SharedSingletonBase* next;

    do
    {
      next = s_head->get ();
      instance->m_next = next;
    }
    while (!s_head->compareAndSet (instance, next));
  }

private:
  class AtExit
  {
  public:
    ~AtExit ()
    {
      SharedSingletonBase* instance = PersistentReferences::s_head->get();

      while (instance)
      {
        SharedSingletonBase* next = instance->m_next;
        instance->removePersistentReference ();
        instance = next;
      }

      LeakCheckedBase::detectLeaks ();
    }
  };

  static Static::Storage <Atomic::Pointer <SharedSingletonBase>, PersistentReferences> s_head;
  static AtExit s_atexit;
};

Static::Storage <Atomic::Pointer <SharedSingletonBase>,
                 SharedSingletonBase::PersistentReferences>
                 SharedSingletonBase::PersistentReferences::s_head;

SharedSingletonBase::PersistentReferences::AtExit
                 SharedSingletonBase::PersistentReferences::s_atexit;

//------------------------------------------------------------------------------

void SharedSingletonBase::addPersistentReference ()
{
  PersistentReferences::push_front (this);
}

END_VF_NAMESPACE
