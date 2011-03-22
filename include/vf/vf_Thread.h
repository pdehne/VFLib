// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_THREAD_VFHEADER__
#define __VF_THREAD_VFHEADER__

namespace detail {

namespace Thread {

// Used to throw generic thread interruptions
// when using the exception interruption model.
class Interruption
{
};

// This is the flag used to indicate if an interruption
// occurred when using the polling model. It is designed
// to detect improper usage (specifically, not checking
// the flag, which would result in incorrect behavior).

class Interrupted
{
public:
  Interrupted ()
   : m_interrupted (false)
   , m_checked (false)
  {
  }

  explicit Interrupted (bool interrupted)
   : m_interrupted (interrupted)
   , m_checked (false)
  {
  }

  Interrupted (const Interrupted& other)
    : m_interrupted (other.m_interrupted)
    , m_checked (false)
  {
    other.m_checked = true;
  }

  ~Interrupted ()
  {
    vfassert (!m_interrupted || m_checked);
  }

  Interrupted& operator= (const Interrupted& other)
  {
    m_interrupted = other.m_interrupted;
    m_checked = false;
    other.m_checked = true;
    return *this;
  }

  // TODO: safe bool idiom
  operator bool () const
  {
    m_checked = true;
    return m_interrupted;
  }

private:
  bool m_interrupted;
  mutable bool m_checked;
};

}

}

#endif
