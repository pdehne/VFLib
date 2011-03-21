// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_LEAKCHECKED_VFHEADER__
#define __VF_LEAKCHECKED_VFHEADER__

//
// Derived classes are automatically leak-checked on exit
//

#if VF_CHECK_LEAKS && VF_HAVE_JUCE && JUCE_CHECK_MEMORY_LEAKS

//
// Juce
//
template <class OwnerClass>
class LeakChecked : private VF_JUCE::LeakedObjectDetector <LeakChecked <OwnerClass> >
{
private:
  friend class VF_JUCE::LeakedObjectDetector <LeakChecked>;
  static const char* getLeakedObjectClassName() throw()
    { return typeid (OwnerClass).name (); }
};

#else

#if VF_CHECK_LEAKS
#pragma message(VF_LOC_"Missing class LeakChecked")
#endif

template <class OwnerClass> struct LeakChecked { };

#endif

template <class OwnerClass>
class LeakCheckedAndNonCopyable : LeakChecked <OwnerClass>, NonCopyable
{
};

#endif
