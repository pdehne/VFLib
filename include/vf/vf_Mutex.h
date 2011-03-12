// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_MUTEX_VFHEADER__
#define __VF_MUTEX_VFHEADER__

// Simple recursive mutex

#if VF_USE_JUCE

//
// Juce
//
class Mutex
{
public:
  void enter () { m_mutex.enter (); }
  void exit () { m_mutex.exit (); }
private:
  VF_JUCE::CriticalSection m_mutex;
};

#elif VF_USE_BOOST

//
// Boost
//
class Mutex
{
public:
  void enter () { m_mutex.lock (); }
  void exit () { m_mutex.unlock (); }
private:
  boost::recursive_mutex m_mutex;
};

#else

//
// (none)
//
#pragma message(VF_LOC_"Missing class Mutex")
class Mutex
{
public:
  void enter () {}
  void exit () {}
};

#endif

#endif
