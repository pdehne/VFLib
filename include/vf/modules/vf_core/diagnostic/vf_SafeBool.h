// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef __VF_SAFEBOOL_VFHEADER__
#define __VF_SAFEBOOL_VFHEADER__

//
// From http://www.artima.com/cppsource/safebool.html
//
// Allows evaluation of an object as bool,
// without the usual harmful side effects.
//

//------------------------------------------------------------------------------

namespace detail {

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

}

//------------------------------------------------------------------------------

template <typename T = void>
class SafeBool : public detail::SafeBoolBase
{
public:
  operator boolean_t () const
  {
    return (static_cast <const T*> (this))->asBoolean ()
      ? &detail::SafeBoolBase::allowed : 0;
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

