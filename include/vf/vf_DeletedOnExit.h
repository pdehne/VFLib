// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_DELETEDONEXIT_VFHEADER__
#define __VF_DELETEDONEXIT_VFHEADER__

class DeletedOnExit : NonCopyable
{
public:
  DeletedOnExit ();

protected:
  virtual ~DeletedOnExit ();

private:
  class Singleton;

private:
  DeletedOnExit* m_next;
};

#endif
