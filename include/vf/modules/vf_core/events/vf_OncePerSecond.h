// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_ONCEPERSECOND_VFHEADER
#define VF_ONCEPERSECOND_VFHEADER

#include "../containers/vf_List.h"

//==============================================================================
/** Provides a once per second notification.

    Derive your class from OncePerSecond and override doOncePerSecond(). Then,
    call startOncePerSecond() to begin receiving the notifications. No clean-up
    or other actions are required.
*/
class OncePerSecond : Uncopyable
{
public:
#ifndef DOXYGEN
  OncePerSecond ();
  ~OncePerSecond ();
#endif

  /** Begin receiving notifications. */
  void startOncePerSecond ();

  /** Stop receiving notifications. */
  void endOncePerSecond ();

protected:
  /** Called once per second. */
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
