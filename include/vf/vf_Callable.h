// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CALLABLE_VFHEADER__
#define __VF_CALLABLE_VFHEADER__

#include "vf/vf_Throw.h"

//
// Strong replacement for boost::function with bounded
// memory requirements.
//

namespace detail {

template <int Bytes>
class CallableBase
{
protected:
  char m_storage [Bytes];
};

}

template <typename Signature, int Bytes = 64>
class Callable;

template <typename R, int Bytes>
class Callable <R (void), Bytes>
{
public:
  typedef R result_type;
  typedef Callable self_type;

  struct None
  {
    typedef typename R result_type;
    R operator() () const
    {
      return R();
    } 
  };

  Callable()
  {
    constructCopyOf (None ());
  }

  Callable (const Callable& f)
  {
    f.getCall().constructCopyInto (m_storage);
  }

  template <class Functor>
  Callable (const Functor& f)
  {
    constructCopyOf (f);
  }

  ~Callable ()
  {
    getCall().~Call();
  }

  Callable& operator= (const Callable& f)
  {
    getCall().~Call();
    f.getCall().constructCopyInto (m_storage);
    return *this;
  }

  template <class Functor>
  Callable& operator= (const Functor& f)
  {
    getCall().~Call();
    constructCopyOf (f);
    return *this;
  }

  R operator()()
  {
    return getCall().operator()();
  }

private:
  template <class Functor>
  void constructCopyOf (const Functor& f)
  {
    // If this generates a compile error it means that
    // the functor is too large for the static buffer.
    // Increase the storage template parameter until
    // the error message goes away. This might cause
    // changes throughout the application with other
    // template classes that depend on the size.
    static_vfassert (sizeof (StoredCall <Functor>) <= Bytes);
    new (m_storage) StoredCall <Functor> (f);
  }

private:
  struct Call
  {
    virtual ~Call () {}
    virtual void constructCopyInto (void* p) const = 0;
    virtual R operator()() = 0;
  };

  template <class Functor>
  struct StoredCall : Call
  {
    explicit StoredCall (const Functor& f) : m_f (f) { }
    StoredCall (const StoredCall& c) : m_f (c.m_f) { }
    void constructCopyInto (void* p) const { new (p) StoredCall (m_f); }
    R operator() () { return m_f (); }
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

  char m_storage [Bytes]; // should be enough
};

//------------------------------------------------------------------------------

template <typename R, typename T1, int Bytes>
class Callable <R (T1 t1), Bytes>
{
public:
  typedef R result_type;
  typedef Callable self_type;

  struct None
  {
    typedef typename R result_type;
    R operator() (T1) const
    {
      return R();
    } 
  };

  Callable()
  {
    constructCopyOf (None ());
  }

  Callable (const Callable& f)
  {
    f.getCall().constructCopyInto (m_storage);
  }

  template <class Functor>
  Callable (const Functor& f)
  {
    constructCopyOf (f);
  }

  ~Callable ()
  {
    getCall().~Call();
  }

  Callable& operator= (const Callable& f)
  {
    getCall().~Call();
    f.getCall().constructCopyInto (m_storage);
    return *this;
  }

  template <class Functor>
  Callable& operator= (const Functor& f)
  {
    getCall().~Call();
    constructCopyOf (f);
    return *this;
  }

  R operator() (T1 t1)
  {
    return getCall().operator() (t1);
  }

private:
  template <class Functor>
  void constructCopyOf (const Functor& f)
  {
    // If this generates a compile error it means that
    // the functor is too large for the static buffer.
    // Increase the storage template parameter until
    // the error message goes away. This might cause
    // changes throughout the application with other
    // template classes that depend on the size.
    static_vfassert (sizeof (StoredCall <Functor>) <= Bytes);
    new (m_storage) StoredCall <Functor> (f);
  }

private:
  struct Call
  {
    virtual ~Call () {}
    virtual void constructCopyInto (void* p) const = 0;
    virtual R operator() (T1 t1) = 0;
  };

  template <class Functor>
  struct StoredCall : Call
  {
    explicit StoredCall (const Functor& f) : m_f (f) { }
    StoredCall (const StoredCall& c) : m_f (c.m_f) { }
    void constructCopyInto (void* p) const { new (p) StoredCall (m_f); }
    R operator() (T1 t1) { return m_f (t1); }
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

  char m_storage [Bytes]; // should be enough
};

#endif
