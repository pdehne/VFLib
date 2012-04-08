// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_ALLOCATEDBY_VFHEADER
#define VF_ALLOCATEDBY_VFHEADER

#ifndef DOXYGEN

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

#endif
