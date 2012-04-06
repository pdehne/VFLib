// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_GUICALLQUEUE_VFHEADER
#define VF_GUICALLQUEUE_VFHEADER

#include "vf_CallQueue.h"

/***
  Provides a CallQueue handled by the Juce Message Thread (Gui).

  To use this, put an instance of the class in your application object
  or other central location. Calls placed in the queue will automatically
  get processed by the Juce message loop, no further action is required
  to make it work.
*/
class GuiCallQueue
  : public CallQueue
  , private VF_JUCE::AsyncUpdater
{
public:
  GuiCallQueue ();

  void close ();
  bool process ();

private:
  void signal ();
  void reset ();
  void handleAsyncUpdate ();
};

#endif
