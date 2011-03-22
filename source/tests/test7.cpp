// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Bind.h"
#include "vf/vf_Function.h"

namespace {

void f1 (void) { }
int  f2 (void) { return 0; }
void f3 (int)  { }
int  f4 (int)  { return 1; }

void test ()
{
  vf::Function <void (void)> c1 (vf::bind (&f1));
  vf::Function <int (void)>  c2 (vf::bind (&f2));
  vf::Function <void (int)>  c3 (vf::bind (&f3, 1));
  vf::Function <int (int)>   c4 (vf::bind (&f4, 2));

  int result = c2();
}

}

END_VF_NAMESPACE
