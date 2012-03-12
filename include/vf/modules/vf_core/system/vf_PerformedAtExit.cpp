// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/modules/vf_core/memory/vf_Atomic.h"
#include "vf/modules/vf_core/diagnostic/vf_LeakChecked.h"
#include "vf/modules/vf_core/system/vf_PerformedAtExit.h"
#include "vf/modules/vf_core/threads/vf_SharedObject.h"
#include "vf/modules/vf_core/memory/vf_StaticObject.h"

typedef Static::Storage <Atomic::Pointer <PerformedAtExit>, PerformedAtExit> Head;

class PerformedAtExit::Performer
{
private:
  ~Performer ()
  {
    PerformedAtExit* object = s_head->get ();

    while (object != nullptr)
    {
      PerformedAtExit* next = object->m_next;
      object->performAtExit ();
      object = next;
    }

    PerformedAtExit::performLibraryAtExit ();
  }

public:
  static void push_front (PerformedAtExit* object)
  {
    PerformedAtExit* next;

    do
    {
      next = s_head->get ();
      object->m_next = next;
    }
    while (!s_head->compareAndSet (object, next));
  }

private:
  static Head s_head;
  static Performer s_performer;
};

PerformedAtExit::Performer PerformedAtExit::Performer::s_performer;
Head PerformedAtExit::Performer::s_head;

PerformedAtExit::PerformedAtExit (bool perform)
{
  if (perform)
    Performer::push_front (this);
}

void PerformedAtExit::performLibraryAtExit ()
{
  SharedObject::performLibraryAtExit ();

  LeakCheckedBase::performLibraryAtExit ();
}

END_VF_NAMESPACE
