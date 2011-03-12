// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LEAKCHECKED_VFHEADER__
#define __VF_LEAKCHECKED_VFHEADER__

//
// Derived classes are automatically leak-checked on exit
//

#if VF_CHECK_LEAKS

#if VF_USE_JUCE && JUCE_CHECK_MEMORY_LEAKS

//
// Juce
//
template <class OwnerClass>
class LeakChecked
{
  struct Detector
  {
    friend class VF_JUCE::LeakedObjectDetector <Detector>;
    VF_JUCE::LeakedObjectDetector <Detector> m_leakDetector;
    static const char* getLeakedObjectClassName() throw()
      { return typeid (OwnerClass).name (); }
  };
  Detector m_detector;
};

#else

//
// (missing)
//
#pragma message(VF_LOC_"Missing class LeakChecked")
template <class OwnerClass> struct LeakChecked { };

#endif

#else

//
// (inactive)
//
template <class OwnerClass> struct LeakChecked { };

#endif

#endif
