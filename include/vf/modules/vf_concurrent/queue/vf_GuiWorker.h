// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_GUIWORKER_VFHEADER__
#define __VF_GUIWORKER_VFHEADER__

#include "vf/modules/vf_concurrent/queue/vf_Worker.h"

//
// A thread queue processed by a the Juce Message thread (gui thread)
//

class GuiWorker : public Worker,
                  private VF_JUCE::AsyncUpdater
{
public:
  GuiWorker ();

  void close () { Worker::close (); }
  bool process () { return Worker::process (); }

private:
  void signal ();
  void reset ();
  void handleAsyncUpdate ();
};

#endif
