// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_GUIWORKER_VFHEADER__
#define __VF_GUIWORKER_VFHEADER__

#include "vf_CallQueue.h"

//
// A thread queue processed by a the Juce Message thread (gui thread)
//

class GuiWorker : public CallQueue,
                  private VF_JUCE::AsyncUpdater
{
public:
  GuiWorker ();

  void close () { CallQueue::close (); }
  bool process () { return CallQueue::process (); }

private:
  void signal ();
  void reset ();
  void handleAsyncUpdate ();
};

#endif
