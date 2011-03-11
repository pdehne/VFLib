// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LEAKCHECKED_VFHEADER__
#define __VF_LEAKCHECKED_VFHEADER__

//
// Derived classes are automatically leak-checked on exit
//

#if VF_USE_JUCE && JUCE_CHECK_MEMORY_LEAKS

//
// Uses Juce to track the leaks
//
template <class OwnerClass>
class LeakChecked
{
  friend class VF_JUCE::LeakedObjectDetector <OwnerClass>;
  VF_JUCE::LeakedObjectDetector <OwnerClass> m_leakDetector;
  static const char* getLeakedObjectClassName() throw()
    { return typeid (OwnerClass).name (); }
};

#else

//
// No leak tracking
//

template <class OwnerClass>
struct LeakChecked
{
};

#endif

#endif
