// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_PAGEPOOL_VFHEADER__
#define __VF_PAGEPOOL_VFHEADER__

//
// UNFINISHED
//
class PagePool : NonCopyable
{
public:
  explicit PagePool (int bytesPerPage);
  ~PagePool ();

  int getBytesPerPage ();
  void* allocate ();
  void deallocate (void* const p);

private:
  int const m_bytesPerPage;
};

#endif
