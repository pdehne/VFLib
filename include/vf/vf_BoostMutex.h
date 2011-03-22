// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BOOSTMUTEX_VFHEADER__
#define __VF_BOOSTMUTEX_VFHEADER__

#if VF_HAVE_BOOST

namespace Boost {

class Mutex : NonCopyable
{
public:
  inline void enter () { m_mutex.lock (); }
  inline void exit () { m_mutex.unlock (); }

private:
  boost::recursive_mutex m_mutex;
};

}

#endif

#endif
