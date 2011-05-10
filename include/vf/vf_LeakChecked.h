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

private:
  friend class PerformedAtExit;

  static void performLibraryAtExit ();
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
    static Counter* volatile s_instance;
    static Static::Initializer s_initializer;

    if (s_initializer.begin ())
    {
      static Counter s_object;
      s_instance = &s_object;
      s_initializer.end ();
    }

    return *s_instance;
  }
};

#else

struct LeakCheckedBase
{
  static inline void detectLeaks () { }
};

template <class Object>
struct LeakChecked : LeakCheckedBase
{
};

#endif

template <class Object>
class LeakCheckedAndNonCopyable : LeakChecked <Object>, NonCopyable
{
};

#endif
