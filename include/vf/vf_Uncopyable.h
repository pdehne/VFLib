// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_UNCOPYABLE_VFHEADER__
#define __VF_UNCOPYABLE_VFHEADER__

// borrowed to remove the dependency on boost

namespace detail {

class Uncopyable
{
protected:
  Uncopyable() {}
  ~Uncopyable() {}
private:
  Uncopyable (const Uncopyable&);
  const Uncopyable& operator= (const Uncopyable&);
};

}

typedef detail::Uncopyable Uncopyable;

#endif
