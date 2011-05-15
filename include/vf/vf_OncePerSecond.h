// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_ONCEPERSECOND_VFHEADER__
#define __VF_ONCEPERSECOND_VFHEADER__

#include "vf/vf_List.h"
#include "vf/vf_SharedObject.h"

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
