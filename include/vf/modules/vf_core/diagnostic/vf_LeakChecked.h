// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file README.md for licensing information.

#ifndef VF_LEAKCHECKED_VFHEADER
#define VF_LEAKCHECKED_VFHEADER

#include "vf_Error.h"
#include "vf_Throw.h"
#include "../memory/vf_StaticObject.h"
#include "../containers/vf_LockFreeStack.h"

//
// Derived classes are automatically leak-checked on exit
//

#if VF_CHECK_LEAKS

class LeakCheckedBase
{
public:
  static void detectAllLeaks ();

protected:
  class CounterBase : LockFreeStack <CounterBase>::Node
  {
  public:
    CounterBase ();

    static void detectAllLeaks ();

  private:
    virtual void detectLeaks () = 0;

  private:
    class Singleton;
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
      Throw (Error().fail (__FILE__, __LINE__));
    }
  }

  LeakChecked (const LeakChecked&) noexcept
  {
    if (getLeakCheckedCounter().increment() == 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName ());
      Throw (Error().fail (__FILE__, __LINE__));
    }
  }

  ~LeakChecked()
  {
    if (getLeakCheckedCounter().decrement () < 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName ());
      Throw (Error().fail (__FILE__, __LINE__));
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
    
    void detectLeaks ()
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

class LeakCheckedBase
{
private:
  friend class PerformedAtExit;

  static void detectAllLeaks () { }
};

template <class Object>
struct LeakChecked : LeakCheckedBase
{
};

#endif

#endif
