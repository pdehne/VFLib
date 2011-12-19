// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

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
