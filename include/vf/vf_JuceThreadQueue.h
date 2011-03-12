// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCETHREADQUEUE_VFHEADER__
#define __VF_JUCETHREADQUEUE_VFHEADER__

#if !VF_USE_JUCE
#error "Requires Juce"
#endif

#include "vf/ThreadQueue.h"

//
// A thread queue processed by a the Juce Message thread (gui thread)
//

class JuceThreadQueue : public ThreadQueue,
                        private VF_JUCE::AsyncUpdater
{
public:
  JuceThreadQueue ();

private:
  void handleAsyncUpdate();
  void signal ();
  void reset ();
};

#endif
