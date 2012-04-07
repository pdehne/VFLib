// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CONCURRENTOBJECT_VFHEADER
#define VF_CONCURRENTOBJECT_VFHEADER

//==============================================================================
/** A reference counted object, deleted on a separate thread.

    This is a reference counted object compatible with
    ReferenceCountedObjectPtr. When the last reference is removed, the
    object is queued for deletion on a separate, provided thread. On
    program exit the thread will clean itself up - no other action is
    required.

    This class is useful for offloading the deletion work of "deep" objects
    shared by multiple threads: objects containing complex members, or a
    hierarchy of allocated structures. For example, a ValueTree. The problem
    of performing heavyweight memory or cleanup operations from either the
    audioDeviceIOCallback or the message thread is avoided.

    The deletion behavior can be overriden by providing a replacement
    for deleteConcurrentObject().
*/
class ConcurrentObject : Uncopyable
{
public:
  /** Increment the reference count. */
  inline void incReferenceCount() noexcept
  {
    m_refs.addref ();
  }

  /** Decrement the reference count. */
  inline void decReferenceCount() noexcept
  {
    vfassert (m_refs.is_signaled ());

    if (m_refs.release ())
      destroyConcurrentObject ();
  }

  /** Determine if the object is still being referenced.

      This is not thread safe, so the caller must synchronize.

      @return `true` if the object has one or more references.
  */
  inline bool isBeingReferenced () const
  {
    return m_refs.is_signaled ();
  }

protected:
#ifndef DOXYGEN
  ConcurrentObject();

  virtual ~ConcurrentObject();
#endif

  /** Delete the object.

      The default implementation performs the delete on a separate,
      provided thread that cleans up after itself on exit.
  */
  virtual void destroyConcurrentObject ();

protected:
  class Deleter;

private:
  AtomicCounter m_refs;
};

#endif

