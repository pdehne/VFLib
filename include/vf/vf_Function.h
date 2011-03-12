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

  Function()
  {
    copyConstructFrom (NoCall());
  }

  Function (const Function& f)
  {
    f.getCall().copyConstructTo (m_storage);
  }

  template <class Functor>
  Function (const Functor& f)
  {
    copyConstructFrom (f);
  }

  ~Function ()
  {
    getCall().~Call();
  }

  Function& operator= (const Function& f)
  {
    getCall().~Call();
    f.getCall().copyConstructTo (m_storage);
    return *this;
  }

  template <class Functor>
  Function& operator= (const Functor& f)
  {
    getCall().~Call();
    copyConstructFrom (f);
  }

  void operator()()
  {
    getCall().operator()();
  }

private:
  static const int Max = 128;

  static inline void checkSize (const int bytes)
  {
    if (bytes > Max)
      Throw (std::bad_alloc ());
  }

  template <class Functor>
  void copyConstructFrom (const Functor& f)
  {
    checkSize (sizeof (StoredCall <Functor>));
    new (m_storage) StoredCall <Functor> (f);
  }

private:
  struct Call
  {
    virtual ~Call () {}
    virtual void copyConstructTo (void* p) const = 0;
    virtual void operator()() = 0;
  };

  struct NoCall
  {
    NoCall() { }
    NoCall (const NoCall& c) { }
    void copyConstructTo (void* p) const { new (p) NoCall (); }
    void operator()() { }
  };

  template <class Functor>
  struct StoredCall : Call
  {
    explicit StoredCall (const Functor& f) : m_f (f) { }
    StoredCall (const StoredCall& c) : m_f (c.m_f) { }
    void copyConstructTo (void* p) const { new (p) StoredCall (m_f); }
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
