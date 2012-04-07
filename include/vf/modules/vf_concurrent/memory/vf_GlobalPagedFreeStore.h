// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_GLOBALPAGEDFREESTORE_VFHEADER
#define VF_GLOBALPAGEDFREESTORE_VFHEADER

#ifndef DOXYGEN

#include "vf_PagedFreeStore.h"

/** A PagedFreeStore singleton. */
class GlobalPagedFreeStore
  : public ReferenceCountedSingleton <GlobalPagedFreeStore>
  , LeakChecked <GlobalPagedFreeStore>
{
private:
  GlobalPagedFreeStore ();
  ~GlobalPagedFreeStore ();

public:
  inline size_t getPageBytes ()
  {
    return m_allocator.getPageBytes ();
  }

  inline void* allocate ()
  {
    return m_allocator.allocate ();
  }

  static inline void deallocate (void* const p)
  {
    PagedFreeStore::deallocate (p);
  }

  static GlobalPagedFreeStore* createInstance (); // @implementation

private:
  PagedFreeStore m_allocator;
};

#endif

#endif
