// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.

#ifndef VF_NONCOPYABLE_VFHEADER
#define VF_NONCOPYABLE_VFHEADER

// Prevents warnings about missing copy
// constructors and assignment operators.

namespace detail {

// Ideas based on boost

class Uncopyable
{
protected:
  inline Uncopyable() { }
  inline ~Uncopyable() { }

private:
  Uncopyable (Uncopyable const&);
  Uncopyable const& operator= (Uncopyable const&);
};

}

typedef detail::Uncopyable NonCopyable;
typedef detail::Uncopyable Uncopyable;

#endif
