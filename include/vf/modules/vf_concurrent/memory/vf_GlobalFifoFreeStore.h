// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_GLOBALFIFOFREESTORE_VFHEADER
#define VF_GLOBALFIFOFREESTORE_VFHEADER

#include "vf_FifoFreeStore.h"

/****
  A FifoFreeStore singleton.
*/
template <class Tag>
class GlobalFifoFreeStore : public ReferenceCountedSingleton <GlobalFifoFreeStore <Tag> >
{
public:
  inline void* allocate (size_t bytes)
  {
    return m_allocator.allocate (bytes);
  }

  static inline void deallocate (void* const p)
  {
    FifoFreeStoreType::deallocate (p);
  }

  static GlobalFifoFreeStore* createInstance () // @implementation
  {
    return new GlobalFifoFreeStore;
  }

private:
  GlobalFifoFreeStore ()
	: ReferenceCountedSingleton <GlobalFifoFreeStore <Tag> >
		(SingletonLifetime::persistAfterCreation)
  {
  }

  ~GlobalFifoFreeStore ()
  {
  }

private:
  FifoFreeStoreType m_allocator;
};

#endif
