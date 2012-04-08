// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

class PerformedAtExit::Performer
{
private:
  typedef Static::Storage <LockFreeStack <PerformedAtExit>, PerformedAtExit> Stack;
  
  ~Performer ()
  {
    PerformedAtExit* object = s_list->pop_front ();

    while (object != nullptr)
    {
      object->performAtExit ();

      object = s_list->pop_front ();
    }

    LeakCheckedBase::detectAllLeaks ();
  }

public:
  static void push_front (PerformedAtExit* object)
  {
    s_list->push_front (object);
  }

private:
  static Stack s_list;
  static Performer s_performer;
};

PerformedAtExit::Performer PerformedAtExit::Performer::s_performer;
PerformedAtExit::Performer::Stack PerformedAtExit::Performer::s_list;

PerformedAtExit::PerformedAtExit ()
{
  Performer::push_front (this);
}

