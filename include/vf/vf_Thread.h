// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREAD_VFHEADER__
#define __VF_THREAD_VFHEADER__

#if VF_HAVE_BOOST

class Thread
{
public:
  typedef boost::thread::id id;
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return boost::this_thread::get_id ();
}

}

#elif VF_HAVE_JUCE

class Thread
{
public:
  typedef VF_JUCE::Thread::ThreadID id;
};

namespace CurrentThread {

inline Thread::id getId ()
{
  return VF_JUCE::Thread::getCurrentThreadId ();
}

}

#else
  #pragma message(VF_LOC_"Missing Thread")

#endif

#endif

