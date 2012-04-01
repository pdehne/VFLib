// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_PERFORMEDATEXIT_VFHEADER
#define VF_PERFORMEDATEXIT_VFHEADER

class PerformedAtExit : Uncopyable
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
