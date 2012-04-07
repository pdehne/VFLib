// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_GUICALLQUEUE_VFHEADER
#define VF_GUICALLQUEUE_VFHEADER

#include "vf_CallQueue.h"

//==============================================================================
/** A CallQueue handled by the Juce Message Thread (Gui).

    To use this, put an instance of the class in your application object
    or other central location. Calls placed in the queue will automatically
    get processed by the Juce message loop, no further action is required
    to make it work.
*/
class GuiCallQueue
  : public CallQueue
  , private VF_JUCE::AsyncUpdater
{
public:
  /** Create a GuiCallQueue */
  GuiCallQueue ();

  /** Close the GuiCallQueue.

      This must be called manually by the application during exit as early
      as possible.
  */
  void close ();

  /** Synchronize the queue.

      This can be called manually if needed
  */
  bool process ();

private:
  void signal ();
  void reset ();
  void handleAsyncUpdate ();
};

#endif
