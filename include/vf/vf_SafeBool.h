// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SAFEBOOL_VFHEADER__
#define __VF_SAFEBOOL_VFHEADER__

//
// From http://www.artima.com/cppsource/safebool.html
//
// Allows evaluation of an object as bool,
// without the usual harmful side effects.
//

class SafeBoolBase
{
private:
  void disallowed () const { }

public:
  void allowed () const { }

protected:
  typedef void (SafeBoolBase::*boolean_t)() const;

  SafeBoolBase () { }
  SafeBoolBase (SafeBoolBase const&) { }
  SafeBoolBase& operator= (SafeBoolBase const&) { return *this; }
  ~SafeBoolBase () { }
};

template <typename T = void>
class SafeBool : public SafeBoolBase
{
public:
  operator boolean_t () const
  {
    return (static_cast <const T*> (this))->asBoolean ()
      ? &SafeBoolBase::allowed : 0;
  }

protected:
  ~SafeBool() { }
};

template <typename T, typename U> 
void operator== (SafeBool <T> const& lhs, SafeBool <U> const& rhs)
{
  lhs.disallowed ();
}

template <typename T,typename U> 
void operator!= (SafeBool <T> const& lhs, SafeBool <U> const& rhs)
{
  lhs.disallowed ();
}

#endif


