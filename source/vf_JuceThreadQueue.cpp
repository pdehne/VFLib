// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#if VF_HAVE_JUCE

#include "vf/vf_JuceThreadQueue.h"

namespace {

void updateAllTopLevelWindows ()
{
#if JUCE_WIN32
  static bool inUpdate = false;

  if (!inUpdate)
  {
    inUpdate = true;

    int n = TopLevelWindow::getNumTopLevelWindows();
    for (int i = 0; i < n; ++i)
    {
      TopLevelWindow* w = TopLevelWindow::getTopLevelWindow (i);
      ComponentPeer* peer = w->getPeer ();
      if (peer)
        peer->performAnyPendingRepaintsNow ();
    }
  }
#endif
}

}

JuceThreadQueue::JuceThreadQueue () : Worker ("JuceWorker")
{
  open ();
}

void JuceThreadQueue::signal ()
{
  triggerAsyncUpdate ();
}

void JuceThreadQueue::reset ()
{
}

void JuceThreadQueue::handleAsyncUpdate()
{
  process ();

  // This tries to solve the problem where continual streams
  // of updates from the deck and mixer prevents the drag
  // image from drawing.
  updateAllTopLevelWindows ();
}

#endif

END_VF_NAMESPACE

