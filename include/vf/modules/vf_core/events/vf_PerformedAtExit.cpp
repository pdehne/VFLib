// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

typedef Static::Storage <AtomicPointer <PerformedAtExit>, PerformedAtExit> Head;

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
  LeakCheckedBase::performLibraryAtExit ();
}

