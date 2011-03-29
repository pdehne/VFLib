// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_NONCOPYABLE_VFHEADER__
#define __VF_NONCOPYABLE_VFHEADER__

// Prevents warnings about missing copy
// constructors and assignment operators.

namespace detail {

// Ideas based on boost

class NonCopyable
{
protected:
  inline NonCopyable() { }
  inline ~NonCopyable() { }
private:
  NonCopyable (const NonCopyable&);
  const NonCopyable& operator= (const NonCopyable&);
};

}

typedef detail::NonCopyable NonCopyable;

#endif
