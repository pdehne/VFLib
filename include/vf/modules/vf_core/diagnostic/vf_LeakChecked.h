// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_LEAKCHECKED_VFHEADER
#define VF_LEAKCHECKED_VFHEADER

#include "vf/modules/vf_core/diagnostic/vf_Error.h"
#include "vf/modules/vf_core/memory/vf_StaticObject.h"
#include "vf/modules/vf_core/diagnostic/vf_Throw.h"

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

class LeakCheckedBase
{
private:
  friend class PerformedAtExit;

  static void performLibraryAtExit () { }
};

template <class Object>
struct LeakChecked : LeakCheckedBase
{
};

#endif

#if 0
template <class Object>
class LeakCheckedAndNonCopyable : LeakChecked <Object>, Uncopyable
{
};
#endif

#endif
