// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_GUICALLQUEUE_VFHEADER__
#define __VF_GUICALLQUEUE_VFHEADER__

#include "vf_CallQueue.h"

/***
  Provides a CallQueue handled by the Juce Message Thread (Gui).
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
