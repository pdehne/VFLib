// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_PERFORMEDATEXIT_VFHEADER__
#define __VF_PERFORMEDATEXIT_VFHEADER__

class PerformedAtExit : NonCopyable
{
protected:
  explicit PerformedAtExit (bool perform);

private:
  virtual void performAtExit () = 0;

private:
  static void performLibraryAtExit ();

  class Performer;

  PerformedAtExit* m_next;
};

#endif
