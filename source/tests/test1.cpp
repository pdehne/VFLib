// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

// This file is used for detecting compile problems.

//
// Checks that the public library header compiles
//

#include "vf/vf.h"

//#include <boost/bind/bind_template.hpp>

namespace { void dummmy () { } } // hides the linker warning

namespace {

struct Foo
{
  void bar ();
  void bar1 (int);
};

template <typename return_type>
return_type doit ()
{
  return return_type();
}

void test ()
{
  Foo f;

  doit <void> ();

  //vf::Bind (&Foo::bar, &f);
  vf::Bind (&Foo::bar1, &f, 1);
}

}