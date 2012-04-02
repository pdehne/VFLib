// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_MANUALCALLQUEUE_VFHEADER__
#define __VF_MANUALCALLQUEUE_VFHEADER__

#include "vf_CallQueue.h"

/****
  A CallQueue that requires periodic manual calls to process.

  This is useful for adding a call queue to things like the
  audioIODeviceCallback, which provides its own thread.
*/
class ManualCallQueue : public CallQueue
{
public:
  explicit ManualCallQueue (String name);

  void close ();
  bool process ();

private:
  void signal ();
  void reset ();
};

#endif
