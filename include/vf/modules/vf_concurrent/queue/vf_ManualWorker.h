// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_MANUALWORKER_VFHEADER__
#define __VF_MANUALWORKER_VFHEADER__

#include "vf_CallQueue.h"

//
// A CallQueue that requires periodic manual calls to process.
//

class ManualWorker : public CallQueue
{
public:
  explicit ManualWorker (const char* szName = "")
    : CallQueue (szName)
  {
  }

  void close ()   { CallQueue::close (); }
  bool process () { return CallQueue::process (); }

private:
  void signal ()  { }
  void reset ()   { }
};

#endif
