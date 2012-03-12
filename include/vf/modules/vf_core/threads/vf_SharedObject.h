// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_SHAREDOBJECT_VFHEADER__
#define __VF_SHAREDOBJECT_VFHEADER__

#include "vf/modules/vf_core/memory/vf_Atomic.h"
#include "vf/modules/vf_core/diagnostic/vf_LeakChecked.h"
#include "vf/modules/vf_core/threads/vf_SharedObjectPtr.h"

// A copy of juce::ReferenceCountedObject,
// with the following features:
//
// - Shorter name
//
// - Derived class may override the behavior of destruction.
//
// - Default behavior performs the delete on a separate thread.
//

class SharedObject : NonCopyable
{
public:
  inline void incReferenceCount() noexcept
  {
    m_refs.addref ();
  }

  inline void decReferenceCount() noexcept
  {
    vfassert (m_refs.is_signaled ());

    if (m_refs.release ())
      destroySharedObject ();
  }

  // Caller must synchronize.
  inline bool isBeingReferenced () const
  {
    return m_refs.is_signaled ();
  }

protected:
  SharedObject() { }

  virtual ~SharedObject() { }

  // default implementation performs the delete
  // on a separate, provided thread that cleans up
  // after itself on exit.
  //
  virtual void destroySharedObject ();

protected:
  class Deleter;

private:
  friend class PerformedAtExit;

  static void performLibraryAtExit ();

private:
  Atomic::Counter m_refs;
};

#endif

