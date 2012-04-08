// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_PERFORMEDATEXIT_VFHEADER
#define VF_PERFORMEDATEXIT_VFHEADER

#include "../containers/vf_LockFreeStack.h"

//==============================================================================
/** Provides an opportunity to perform an action at exit

    To use, derive your class from PerformedAtExit, and override performAtExit().
    The call will be made during the destruction of objects with static storage
    duration, before LeakChecked performs its diagnostics.
*/
class PerformedAtExit : LockFreeStack <PerformedAtExit>::Node
{
protected:
  explicit PerformedAtExit ();

private:
  virtual void performAtExit () = 0;

private:
  class Performer;

  PerformedAtExit* m_next;
};

#endif
