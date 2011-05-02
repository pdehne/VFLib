// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LEAKCHECKED_VFHEADER__
#define __VF_LEAKCHECKED_VFHEADER__

//
// Derived classes are automatically leak-checked on exit
//

#if VF_CHECK_LEAKS

template <class Object>
class LeakChecked
{
public:
  LeakChecked() noexcept
  {
    if (++getLeakCheckedCounter() == 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName());
      jassertfalse;
    }
  }

  LeakChecked (const LeakChecked&) noexcept
  {
    if (++getLeakCheckedCounter() == 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName());
      jassertfalse;
    }
  }

  ~LeakChecked()
  {
    const int newValue = --getLeakCheckedCounter();
    if (newValue < 0)
    {
      DBG ("[LOGIC] " << getLeakCheckedName());
      jassertfalse;
    }
  }

private:
  class Counter
  {
  public:
    ~Counter()
    {
      const int count = getCounter().get ();
      if (count > 0)
      {
        DBG ("[LEAK] " << count << " of " << getLeakCheckedName());
        jassertfalse;
      }
    }

    inline VF_JUCE::Atomic <int>& getCounter ()
    {
      return *reinterpret_cast <VF_JUCE::Atomic <int>*> (m_storage);
    }

  private:
    char m_storage [sizeof (VF_JUCE::Atomic <int>)];
  };

  static const char* getLeakCheckedName ()
  {
    return typeid (Object).name ();
  }

  static VF_JUCE::Atomic <int>& getLeakCheckedCounter() noexcept
  {
    return s_counter.getCounter ();
  }

  static Counter s_counter;
};

template <class Object>
typename LeakChecked <Object>::Counter LeakChecked <Object>::s_counter;

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
