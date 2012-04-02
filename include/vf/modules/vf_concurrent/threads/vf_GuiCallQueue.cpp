// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

namespace
{

// This tries to solve the problem where continual streams of work
// cause some painting not to occur. This bug was fixed in JUCE.
//
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

GuiCallQueue::GuiCallQueue () : CallQueue ("GuiCallQueue")
{
  // This object must be created from the Juce Message Thread.
  //
  vfassert (VF_JUCE::MessageManager::getInstance()->isThisTheMessageThread());

  // Associate the CallQueue with the message thread right away.
  //
  process ();
}

void GuiCallQueue::close ()
{
  CallQueue::close ();
}

bool GuiCallQueue::process ()
{
  return CallQueue::process ();
}

void GuiCallQueue::signal ()
{
  triggerAsyncUpdate ();
}

void GuiCallQueue::reset ()
{
}

void GuiCallQueue::handleAsyncUpdate()
{
  process ();

  //updateAllTopLevelWindows ();
}
