// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_GuiWorker.h"

#if VF_HAVE_JUCE

namespace {

void updateAllTopLevelWindows ()
{
#if JUCE_WINDOWS
  // This screws up
  static bool inUpdate = false;

  if (!inUpdate)
  {
    inUpdate = true;

    int n = VF_JUCE::TopLevelWindow::getNumTopLevelWindows();
    for (int i = 0; i < n; ++i)
    {
      VF_JUCE::TopLevelWindow* w = VF_JUCE::TopLevelWindow::getTopLevelWindow (i);
      VF_JUCE::ComponentPeer* peer = w->getPeer ();
      if (peer)
        peer->performAnyPendingRepaintsNow ();
    }

    inUpdate = false;
  }
#endif
}

}

GuiWorker::GuiWorker () : Worker ("JuceWorker")
{
  vfassert (VF_JUCE::MessageManager::getInstance()->isThisTheMessageThread());

  // HACK! trick the Worker into getting the thread
  // id so that calls become synchronous from the beginning.
  associateWithCurrentThread ();
}

void GuiWorker::signal ()
{
  triggerAsyncUpdate ();
}

void GuiWorker::reset ()
{
}

void GuiWorker::handleAsyncUpdate()
{
  process ();

  // This tries to solve the problem where continual
  // streams of work cause some painting not to occur.
  // FIXED in the latest Juce tip
  //updateAllTopLevelWindows ();
}

#else

#endif

END_VF_NAMESPACE

