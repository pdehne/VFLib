// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_NONCOPYABLE_VFHEADER__
#define __VF_NONCOPYABLE_VFHEADER__

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
