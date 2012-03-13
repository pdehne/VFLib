// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_MANUALWORKER_VFHEADER__
#define __VF_MANUALWORKER_VFHEADER__

#include "vf/modules/vf_concurrent/queue/vf_Worker.h"

//
// A Worker that requires periodic manual calls to process.
//

class ManualWorker : public Worker
{
public:
  explicit ManualWorker (const char* szName = "")
    : Worker (szName)
  {
  }

  void close ()   { Worker::close (); }
  bool process () { return Worker::process (); }

private:
  void signal ()  { }
  void reset ()   { }
};

#endif
