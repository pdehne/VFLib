// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LEAKCHECKED_VFHEADER__
#define __VF_LEAKCHECKED_VFHEADER__

#include "vf/vf_Error.h"
#include "vf/vf_Static.h"
#include "vf/vf_Throw.h"

//
// Derived classes are automatically leak-checked on exit
//

#if VF_CHECK_LEAKS

class LeakCheckedBase
{
public:
  static void detectLeaks ();

protected:
  class CounterBase
  {
  public:
    CounterBase ();

    static void detectLeaks ();

  private:
    virtual void detectLeak () = 0;

  private:
    class Singleton;

    CounterBase* m_next;
  };
};

//------------------------------------------------------------------------------

template <class Object>
class LeakChecked : private LeakCheckedBase
{
protected:
  LeakChecked () noexcept
  {
    if (getLeakCheckedCounter().increment () == 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName ());
      vf::Throw (Error().fail (__FILE__, __LINE__));
    }
  }

  LeakChecked (const LeakChecked&) noexcept
  {
    if (getLeakCheckedCounter().increment() == 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName ());
      vf::Throw (Error().fail (__FILE__, __LINE__));
    }
  }

  ~LeakChecked()
  {
    if (getLeakCheckedCounter().decrement () < 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName ());
      vf::Throw (Error().fail (__FILE__, __LINE__));
    }
  }

private:
  class Counter : private CounterBase
  {
  public:
    Counter ()
    {
    }

    inline int increment ()
    {
      return ++m_count;
    }

    inline int decrement ()
    {
      return --m_count;
    }
    
    void detectLeak ()
    {
      const int count = m_count.get ();
      if (count > 0)
      {
        DBG ("[LEAK] " << count << " of " << getLeakCheckedName());
        jassertfalse;
      }
    }

  private:
    VF_JUCE::Atomic <int> m_count;
  };

  static const char* getLeakCheckedName ()
  {
    return typeid (Object).name ();
  }

  static Counter& getLeakCheckedCounter() noexcept
  {
    if (s_initializer.begin ())
    {
      s_counter.construct ();
      s_initializer.end ();
    }

    return *s_counter;
  }

  static Static::Initializer <LeakChecked <Object> > s_initializer;
  static Static::Storage <Counter, LeakChecked <Object> > s_counter;
};

template <class Object>
Static::Initializer <LeakChecked <Object> > LeakChecked <Object>::s_initializer;

template <class Object>
Static::Storage <typename LeakChecked <Object>::Counter,
                 LeakChecked <Object> > LeakChecked <Object>::s_counter;

#else

template <class Object>
struct LeakChecked
{
};

#endif

template <class Object>
class LeakCheckedAndNonCopyable : LeakChecked <Object>, NonCopyable
{
};

#endif
