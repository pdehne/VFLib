// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_ONCEPERSECOND_VFHEADER__
#define __VF_ONCEPERSECOND_VFHEADER__

#include "vf/modules/vf_core/containers/vf_List.h"
#include "vf/modules/vf_core/threads/vf_SharedObject.h"

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
  typedef SharedObjectPtr <TimerSingleton> TimerPtr;

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
