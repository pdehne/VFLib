// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LEAKCHECKED_VFHEADER__
#define __VF_LEAKCHECKED_VFHEADER__

#include "vf/vf_Static.h"

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
    if (++getCounter() == 0)
    {
      DBG ("*** Instruction reordering! Class: " << getLeakedObjectClassName());
      jassertfalse;
    }
  }

  LeakChecked (const LeakChecked&) noexcept
  {
    if (++getCounter() == 0)
    {
      DBG ("*** Instruction reordering! Class: " << getLeakedObjectClassName());
      jassertfalse;
    }
  }

  ~LeakChecked()
  {
    const int newValue = --getCounter();
    if (newValue < 0)
    {
      DBG ("*** Dangling pointer deletion! Class: " << getLeakedObjectClassName());
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
        DBG ("*** Leaked objects detected: " <<
             count << " instance(s) of class " <<
             getLeakedObjectClassName());
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

  static const char* getLeakedObjectClassName()
  {
    return typeid (Object).name ();
  }

  static VF_JUCE::Atomic <int>& getCounter() noexcept
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
