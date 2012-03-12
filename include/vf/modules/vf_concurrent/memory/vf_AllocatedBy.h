// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_ALLOCATEDBY_VFHEADER__
#define __VF_ALLOCATEDBY_VFHEADER__

// Derived classes will use the specified allocator for new and delete
//
template <class AllocatorType>
class AllocatedBy
{
public:
  inline void* operator new (size_t bytes, AllocatorType& allocator) noexcept
  {
    return allocator.allocate (bytes);
  }

  inline void* operator new (size_t bytes, AllocatorType* allocator) noexcept
  {
    return allocator->allocate (bytes);
  }

  inline void operator delete (void* p, AllocatorType&) noexcept
  {
    AllocatorType::deallocate (p);
  }

  inline void operator delete (void* p, AllocatorType*) noexcept
  {
    AllocatorType::deallocate (p);
  }

  inline void operator delete (void* p) noexcept
  {
    AllocatorType::deallocate (p);
  }
};

#endif
