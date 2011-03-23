// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREADLOCAL_VFHEADER__
#define __VF_THREADLOCAL_VFHEADER__

//
// A ThreadLocal wraps an object that is considered "owned"
// by one particular thread. It is an error to access the object
// from any thread other than the owning thread. This object
// is used mainly for diagnostics
//
// The object must have a default constructor
//

/*
 * UNFINISHED
 *
 */
template <class Object, class ThreadType = Thread>
class ThreadLocal : NonCopyable
{
public:
  ThreadLocal ()
  {
  }

  ~ThreadLocal ()
  {
  }

  inline Object* operator-> ()
  {
    return &m_object;
  }

  inline Object* getObject ()
  {
    return &m_object;
  }

  inline const Object* getObject () const
  {
    return &m_object;
  }

private:
  Object m_object;
};

#endif
