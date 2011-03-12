// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SYNCHRONIZEDOBJECT_VFHEADER__
#define __VF_SYNCHRONIZEDOBJECT_VFHEADER__

#include "vf/vf_Mutex.h"

// An object that can only be accessed by one
// thread at a time. Access to the object
// is only possible through a ScopedAccess.
template <class Object>
class SynchronizedObject
{
public:
  // This is dangerous. But in the process of using it to convert
  // old code, it can be useful for later search and replacing.
  class ManualAccess : NonCopyable
  {
  private:
    SynchronizedObject& m_object;

  public:
    ManualAccess (SynchronizedObject& object) : m_object (object) { }
    void enter () { m_object.m_mutex.enter (); }
    void exit () { m_object.m_mutex.exit (); }
    Object* getObject  () { return &m_object.m_object; }
    Object& operator*  () { return *getObject(); }
    Object* operator-> () { return getObject(); }
    Object const* getObject  () const { return &m_object.m_object; }
    Object const& operator*  () const { return *getObject(); }
    Object const* operator-> () const { return getObject(); }
  };

  class ScopedAccess : ManualAccess
  {
  public:
    ScopedAccess (SynchronizedObject& object)
      : ManualAccess (object)
    {
      enter ();
    }

    ~ScopedAccess ()
    {
      exit ();
    }

    Object* getObject  () { return ManualAccess::getObject(); }
    Object& operator*  () { return *ManualAccess::getObject(); }
    Object* operator-> () { return ManualAccess::getObject(); }

    Object const* getObject  () const { return ManualAccess::getObject(); }
    Object const& operator*  () const { return *ManualAccess::getObject(); }
    Object const* operator-> () const { return ManualAccess::getObject(); }
  };

  // This is total trash
  Object& unsynchronizedAccess() { return m_object; }
  Object const& unsynchronizedAccess() const { return m_object; }

private:
  Mutex m_mutex;
  Object m_object;
};

#endif
