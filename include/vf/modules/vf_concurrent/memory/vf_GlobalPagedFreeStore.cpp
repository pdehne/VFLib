// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

namespace
{

// Size of a page
//
static const size_t globalPageBytes = 8 * 1024;

}

GlobalPagedFreeStore::GlobalPagedFreeStore ()
  : ReferenceCountedSingleton <GlobalPagedFreeStore> (SingletonLifetime::persistAfterCreation)
  , m_allocator (globalPageBytes)
{
}

GlobalPagedFreeStore::~GlobalPagedFreeStore ()
{
}

GlobalPagedFreeStore* GlobalPagedFreeStore::createInstance ()
{
  return new GlobalPagedFreeStore;
}
