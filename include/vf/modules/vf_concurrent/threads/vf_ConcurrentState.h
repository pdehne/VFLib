// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_CONCURRENTSTATE_VFHEADER
#define VF_CONCURRENTSTATE_VFHEADER

#include "vf/modules/vf_concurrent/threads/vf_ReadWriteMutex.h"

//
// An object intended to be accessed from multiple threads
// simultaneously. Multiple readers may exist simultaneously.
// A writer will block readers and gain exclusive access.
//
// The treatment with respect to readers and writers is inherited
// from the behavior of the read/write mutex used.
//

//==============================================================================
/** Encapsulation for an object accessed by multiple threads.

    This template encloses an object typically consisting of public data members
    of primitive and/or class types. Access to the object is restricted through
    the use of accessor objects, based on the type of access required. There are
    three types of access:

    ReadAccess

      Allows read access to the underlying object as const. ReadAccess may be
      granted to one or more threads simultaneously. If one or more threads have 
      ReadAccess, requests to obtain WriteAccess are blocked.

    WriteAccess

      Allows exclusive read/write access the underlying object. A WriteAccess
      request blocks until all existing ReadAccess and WriteAccess requests are
      released. While a WriteAccess exists, requests for ReadAccess will block.

    UnlockedAccess

      Allows read access to the underlying object without using the lock. This
      can be helpful when designing concurrent structures through composition.
      It also makes it easier to search for places in code which use unlocked
      access.
*/
template <class Object>
class ConcurrentState : Uncopyable
{
public:
  typedef ReadWriteMutex ReadWriteMutexType;

  class ReadAccess;
  class WriteAccess;
  class UnlockedAccess;

  // Unlocked read access.
  //
  // DEPRECATED
  Object const* getObject () const { return const_cast <Object*> (&m_obj); }
  Object const& operator* () const { return *getObject(); }
  Object const* operator->() const { return getObject(); }

  ConcurrentState () { }

  template <class T1>
  explicit ConcurrentState (T1 t1) : m_obj (t1) { }

  template <class T1, class T2>
  ConcurrentState (T1 t1, T2 t2) : m_obj (t1, t2) { }

  template <class T1, class T2, class T3>
  ConcurrentState (T1 t1, T2 t2, T3 t3) : m_obj (t1, t2, t3) { }

  template <class T1, class T2, class T3, class T4>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4) : m_obj (t1, t2, t3, t4) { }

  template <class T1, class T2, class T3, class T4, class T5>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
               : m_obj (t1, t2, t3, t4, t5) { }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
               : m_obj (t1, t2, t3, t4, t5, t6) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4,
               T5 t5, T6 t6, T7 t7) : m_obj (t1, t2, t3, t4, t5, t6, t7) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7, class T8>
  ConcurrentState (T1 t1, T2 t2, T3 t3, T4 t4,
               T5 t5, T6 t6, T7 t7, T8 t8)
               : m_obj (t1, t2, t3, t4, t5, t6, t7, t8) { }

private:
  Object m_obj;
  ReadWriteMutexType m_mutex;
};

//------------------------------------------------------------------------------

template <class Object>
class ConcurrentState <Object>::UnlockedAccess : Uncopyable
{
public:
  explicit UnlockedAccess (ConcurrentState const& state)
    : m_state (state)
  {
  }

  Object const* getObject () const { return m_state.getObject (); }
  Object const& operator* () const { return *getObject(); }
  Object const* operator->() const { return getObject(); }

private:
  ConcurrentState const& m_state;
};

//------------------------------------------------------------------------------

template <class Object>
class ConcurrentState <Object>::ReadAccess : Uncopyable
{
public:
  explicit ReadAccess (ConcurrentState const volatile& state)
    : m_state (const_cast <ConcurrentState const&> (state))
    , m_lock (m_state.m_mutex)
  {
  }

  Object const* getObject () const { return m_state.getObject (); }
  Object const& operator* () const { return *getObject(); }
  Object const* operator->() const { return getObject(); }

private:
  ConcurrentState const& m_state;
  ReadWriteMutexType::ScopedReadLockType m_lock;
};

//------------------------------------------------------------------------------

template <class Object>
class ConcurrentState <Object>::WriteAccess : Uncopyable
{
public:
  explicit WriteAccess (ConcurrentState& state)
    : m_state (state)
    , m_lock (m_state.m_mutex)
  {
  }

  Object const* getObject () const { return m_state.getObject (); }
  Object const& operator* () const { return *getObject(); }
  Object const* operator->() const { return getObject(); }

  Object* getObject () { return &m_state.m_obj; }
  Object& operator* () { return *getObject(); }
  Object* operator->() { return getObject(); }

private:
  ConcurrentState& m_state;
  ReadWriteMutexType::ScopedWriteLockType m_lock;
};

#endif
