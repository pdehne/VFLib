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
#if JUCE_WIN32
  // This screws up
  static bool inUpdate = true;

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

    inUpdate = false;
  }
#endif
}

}

GuiWorker::GuiWorker () : Worker ("JuceWorker")
{
  open ();
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
  updateAllTopLevelWindows ();
}

#else

#endif

END_VF_NAMESPACE

