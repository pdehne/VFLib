// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SHAREDSTATE_VFHEADER__
#define __VF_SHAREDSTATE_VFHEADER__

#include "vf/vf_ReadWriteMutex.h"

//
// An object intended to be accessed from multiple threads
// simultaneously. Multiple readers may exist simultaneously.
// A writer will block readers and gain exclusive access.
//
// The treatment with respect to readers and writers is inherited
// from the behavior of the read/write mutex used.
//

template <class Object>
class SharedState : NonCopyable
{
public:
  typedef ReadWriteMutex ReadWriteMutexType;

  class ReadAccess;
  class WriteAccess;

  //
  // DEPRECATED
  // Just use operator-> on the SharedState instead.
  //
  class UnlockedAccess;

  // Unlocked read access
  const Object* getObject () const { return &m_obj; }
  const Object& operator* () const { return *getObject(); }
  const Object* operator->() const { return getObject(); }

  SharedState () { }

  template <class T1>
  explicit SharedState (T1 t1) : m_obj (t1) { }

  template <class T1, class T2>
  SharedState (T1 t1, T2 t2) : m_obj (t1, t2) { }

  template <class T1, class T2, class T3>
  SharedState (T1 t1, T2 t2, T3 t3) : m_obj (t1, t2, t3) { }

  template <class T1, class T2, class T3, class T4>
  SharedState (T1 t1, T2 t2, T3 t3, T4 t4) : m_obj (t1, t2, t3, t4) { }

  template <class T1, class T2, class T3, class T4, class T5>
  SharedState (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5)
               : m_obj (t1, t2, t3, t4, t5) { }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  SharedState (T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6)
               : m_obj (t1, t2, t3, t4, t5, t6) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7>
  SharedState (T1 t1, T2 t2, T3 t3, T4 t4,
               T5 t5, T6 t6, T7 t7) : m_obj (t1, t2, t3, t4, t5, t6, t7) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7, class T8>
  SharedState (T1 t1, T2 t2, T3 t3, T4 t4,
               T5 t5, T6 t6, T7 t7, T8 t8)
               : m_obj (t1, t2, t3, t4, t5, t6, t7, t8) { }

private:
  Object m_obj;
  ReadWriteMutexType m_mutex;
};

//------------------------------------------------------------------------------

// DEPRECATED
template <class Object>
class SharedState <Object>::UnlockedAccess : NonCopyable
{
public:
  UnlockedAccess (const SharedState& state) : m_state (state) { }
  ~UnlockedAccess () { }

  const Object* getObject () const { return &m_state.m_obj; }
  const Object& operator* () const { return *getObject(); }
  const Object* operator->() const { return getObject(); }

private:
  const SharedState& m_state;
};

//------------------------------------------------------------------------------

template <class Object>
class SharedState <Object>::ReadAccess : NonCopyable
{
public:
  ReadAccess (const SharedState& state)
    : m_state (state), m_lock (m_state.m_mutex)
  {
  }

  const Object* getObject () const { return &m_state.m_obj; }
  const Object& operator* () const { return *getObject(); }
  const Object* operator->() const { return getObject(); }

private:
  const SharedState& m_state;
  ReadWriteMutexType::ScopedReadLockType m_lock;
};

//------------------------------------------------------------------------------

template <class Object>
class SharedState <Object>::WriteAccess : NonCopyable
{
public:
  WriteAccess (SharedState& state)
    : m_state (state), m_lock (m_state.m_mutex)
  {
  }

  const Object* getObject () const { return &m_state.m_obj; }
  const Object& operator* () const { return *getObject(); }
  const Object* operator->() const { return getObject(); }

  Object* getObject () { return &m_state.m_obj; }
  Object& operator* () { return *getObject(); }
  Object* operator->() { return getObject(); }

private:
  SharedState& m_state;
  ReadWriteMutexType::ScopedWriteLockType m_lock;
};

//------------------------------------------------------------------------------

// ?? What's this for ??
template <class Object>
class SharedAccess
{
public:
  using SharedState <Object>::ReadAccess;
  using SharedState <Object>::WriteAccess;
};

//------------------------------------------------------------------------------

/* Usage example:

struct MyWorker
{
  struct State
  {
    State ()
    {
      protectedValue = 0;
    }

    int64 protectedValue;
  };

  typedef vf::SharedState <State> SharedState;

  SharedState m_state;

  void addListener (Listener* listener)
  {
    SharedState::ReadAccess state (m_state);

    m_listeners.add (listener);
    m_listeners.queue (&Listener::onUpdateValue, state->protectedValue);
  }

  void threadFunction ()
  {
    SharedState::UnlockedAccess state (m_state);

    if (state->protectedValue == 0)
    {
      SharedState::WriteAccess writeState (m_state);
      writeState->protectedValue = 1;
      m_listeners.call (&Listener::onUpdateValue, writeState->protectedValue);
    }
  }
};

*/

#endif
