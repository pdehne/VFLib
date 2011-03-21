// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_LockFree.h"

namespace {

struct Data;

typedef LockFree::Stack <Data> DataStack;
typedef LockFree::Queue <Data> DataQueue;

struct Data : LockFree::List<Data>::Node
{
  int i;
};

void test ()
{
  Data d1, d2, d3;

  DataStack s;
  s.push_front (&d1);
  s.push_front (&d2);
  s.push_front (&d3);
  s.pop_front ();
  DataStack s2 (s);
  s2.reverse();
  s2.pop_front ();
  s2.pop_front ();

  DataQueue q;
  q.push_back (&d1);
  q.push_back (&d2);
  q.push_back (&d3);
  q.pop_front ();
  q.pop_front ();
  q.pop_front ();

  LockFree::Allocator<Data> a;
  a.New ();
}

}

END_VF_NAMESPACE
