// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_ONCEPERSECOND_VFHEADER
#define VF_ONCEPERSECOND_VFHEADER

#include "../containers/vf_List.h"

// Class derived from OncePerSecond have their override
// called once per second
//
class OncePerSecond : NonCopyable
{
public:
  OncePerSecond ();
  ~OncePerSecond ();

  // Derived class must call this to insert itself.
  // We avoid doing this in our constructor to prevent
  // getting called with a partially-constructed object.
  //
  void startOncePerSecond ();
  void endOncePerSecond ();

protected:
  virtual void doOncePerSecond () = 0;

private:
  class TimerSingleton;
  typedef ReferenceCountedObjectPtr <TimerSingleton> TimerPtr;

  struct Elem;
  typedef List <Elem> List;
  struct Elem : List::Node
  {
    TimerPtr instance;
    OncePerSecond* object;
  };

  Elem m_elem;
};

#endif
