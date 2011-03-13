// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_GUIWORKER_VFHEADER__
#define __VF_GUIWORKER_VFHEADER__

#include "vf/vf_Worker.h"

#if VF_HAVE_JUCE

//
// A thread queue processed by a the Juce Message thread (gui thread)
//

class GuiWorker : public Worker,
                  private VF_JUCE::AsyncUpdater
{
public:
  GuiWorker ();

  void close () { Worker::close (); }

private:
  void signal ();
  void reset ();
  void handleAsyncUpdate ();
};

#else
  #pragma message(VF_LOC_"Missing GuiWorker")

#endif

#endif
