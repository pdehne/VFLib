// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SHAREDSTATE_VFHEADER__
#define __VF_SHAREDSTATE_VFHEADER__

#include "vf/vf_LockFreeReadWriteMutex.h"

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
  class UnlockedAccess : NonCopyable
  {
  public:
    UnlockedAccess (const SharedState& state) : m_state (state) { }
    ~UnlockedAccess () { }

    const Object* getObject () const { return &m_state.m_object; }
    const Object& operator* () const { return *getObject(); }
    const Object* operator->() const { return getObject(); }
  
  private:
    const SharedState& m_state;
  };

  class ReadAccess : NonCopyable
  {
  public:
    ReadAccess (const SharedState& state) : m_state (state)
      { m_state.m_mutex.enter_read (); }
    ~ReadAccess ()
      { m_state.m_mutex.exit_read (); }

    const Object* getObject () const { return &m_state.m_object; }
    const Object& operator* () const { return *getObject(); }
    const Object* operator->() const { return getObject(); }
  
  private:
    const SharedState& m_state;
  };

  class WriteAccess : NonCopyable
  {
  public:
    WriteAccess (SharedState& state) : m_state (state)
    {
      m_state.m_mutex.enter_write ();
    }

    ~WriteAccess ()
    {
      m_state.m_mutex.exit_write ();
    }

    const Object* getObject () const { return &m_state.m_object; }
    const Object& operator* () const { return *getObject(); }
    const Object* operator->() const { return getObject(); }

    Object* getObject () { return &m_state.m_object; }
    Object& operator* () { return *getObject(); }
    Object* operator->() { return getObject(); }

  private:
    SharedState& m_state;
  };

private:
  LockFree::ReadWriteMutex m_mutex;
  Object m_object;
};

template <class Object>
class SharedAccess
{
public:
  using SharedState <Object>::ReadAccess;
  using SharedState <Object>::WriteAccess;
};

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
