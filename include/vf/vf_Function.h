// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_FUNCTION_VFHEADER__
#define __VF_FUNCTION_VFHEADER__

#include "vf/vf_Throw.h"

//
// Lightweight but limited replacement for boost::function
//

class Function
{
public:
  typedef void result_type;

  struct None { typedef void result_type; void operator()() const { } };

  // Default constructor is a function that does nothing.
  Function()
  {
    constructCopyOf (None ());
  }

  Function (const Function& f)
  {
    f.getCall().constructCopyInto (m_storage);
  }

  template <class Functor>
  Function (const Functor& f)
  {
    constructCopyOf (f);
  }

  ~Function ()
  {
    getCall().~Call();
  }

  Function& operator= (const Function& f)
  {
    getCall().~Call();
    f.getCall().constructCopyInto (m_storage);
    return *this;
  }

  template <class Functor>
  Function& operator= (const Functor& f)
  {
    getCall().~Call();
    constructCopyOf (f);
    return *this;
  }

  void operator()()
  {
    getCall().operator()();
  }

private:
  static const int Max = 64;

  template <class Functor>
  void constructCopyOf (const Functor& f)
  {
    // If this generates a compile error it means that
    // your functor is too large for our static buffer.
    // This class was not designed as a general purpose
    // replacement for boost::function, it is only used
    // to provide the functionality needed for this
    // library without depending on boost.
    static_vfassert (sizeof (StoredCall <Functor>) <= Max);
    new (m_storage) StoredCall <Functor> (f);
  }

private:
  struct Call
  {
    virtual ~Call () {}
    virtual void constructCopyInto (void* p) const = 0;
    virtual void operator()() = 0;
  };

  template <class Functor>
  struct StoredCall : Call
  {
    explicit StoredCall (const Functor& f) : m_f (f) { }
    StoredCall (const StoredCall& c) : m_f (c.m_f) { }
    void constructCopyInto (void* p) const { new (p) StoredCall (m_f); }
    void operator()() { m_f(); }
  private:
    Functor m_f;
  };

  Call& getCall ()
  {
    return *reinterpret_cast <Call*> (&m_storage[0]);
  }

  const Call& getCall () const
  {
    return *reinterpret_cast <const Call*> (&m_storage[0]);
  }

  char m_storage [Max]; // should be enough
};

#endif
