// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_SAFEBOOL_VFHEADER__
#define __VF_SAFEBOOL_VFHEADER__

//
// http://www.artima.com/cppsource/safebool.html
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

/*
template<>
class SafeBool <void> : public SafeBoolBase
{
public:
  operator boolean_t () const
  {
    return asBoolean() == true ? &SafeBoolBase::allowed : 0;
  }

protected:
  virtual bool asBoolean () const = 0;
  virtual ~SafeBool() { }
};
*/

template <typename T, typename U> 
void operator== (SafeBool <T> const& lhs, SafeBool <U> const& rhs)
{
  lhs.disallowed ();
  return false;
}

template <typename T,typename U> 
void operator!= (SafeBool <T> const& lhs, SafeBool <U> const& rhs)
{
  lhs.disallowed ();
  return false;	
}

#endif


