// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_JUCETHREADQUEUE_VFHEADER__
#define __VF_JUCETHREADQUEUE_VFHEADER__

#include "vf/vf_Threads.h"

#if VF_HAVE_THREADS

#include "vf/vf_ThreadQueue.h"

//
// A thread queue processed by a the Juce Message thread (gui thread)
//

class JuceThreadQueue : public ThreadQueue,
                        private VF_JUCE::AsyncUpdater
{
public:
  JuceThreadQueue ();

  void close ()         { ThreadQueue::close (); }

private:
  void signal ();
  void reset ();
  void handleAsyncUpdate();
};

#endif

#endif
