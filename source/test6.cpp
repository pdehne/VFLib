// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

//
// This file is used for detecting compile problems.
//
// Tests the Bind implementation
//

#include "vf/vf_StandardHeader.h"

BEGIN_VF_NAMESPACE

#include "vf/vf_Bind.h"

namespace {

struct R { };
struct P { };
namespace N {
  void v() { }
  void v(P) { }
  void v(P,P) { }
  void v(P,P,P) { }
  void v(P,P,P,P) { }
  void v(P,P,P,P,P) { }
  void v(P,P,P,P,P,P) { }
  void v(P,P,P,P,P,P,P) { }
  void v(P,P,P,P,P,P,P,P) { }
  R r() { return R(); }
  R r(P) { return R(); }
  R r(P,P) { return R(); }
  R r(P,P,P) { return R(); }
  R r(P,P,P,P) { return R(); }
  R r(P,P,P,P,P) { return R(); }
  R r(P,P,P,P,P,P) { return R(); }
  R r(P,P,P,P,P,P,P) { return R(); }
  R r(P,P,P,P,P,P,P,P) { return R(); }
}
struct S
{
  void v() { }
  void v(P) { }
  void v(P,P) { }
  void v(P,P,P) { }
  void v(P,P,P,P) { }
  void v(P,P,P,P,P) { }
  void v(P,P,P,P,P,P) { }
  void v(P,P,P,P,P,P,P) { }
  void v(P,P,P,P,P,P,P,P) { }
  void vc() const { }
  void vc(P) const { }
  void vc(P,P) const { }
  void vc(P,P,P) const { }
  void vc(P,P,P,P) const { }
  void vc(P,P,P,P,P) const { }
  void vc(P,P,P,P,P,P) const { }
  void vc(P,P,P,P,P,P,P) const { }
  void vc(P,P,P,P,P,P,P,P) const { }
  R r() { return R(); }
  R r(P) { return R(); }
  R r(P,P) { return R(); }
  R r(P,P,P) { return R(); }
  R r(P,P,P,P) { return R(); }
  R r(P,P,P,P,P) { return R(); }
  R r(P,P,P,P,P,P) { return R(); }
  R r(P,P,P,P,P,P,P) { return R(); }
  R r(P,P,P,P,P,P,P,P) { return R(); }
  R rc() const { return R(); }
  R rc(P) const { return R(); }
  R rc(P,P) const { return R(); }
  R rc(P,P,P) const { return R(); }
  R rc(P,P,P,P) const { return R(); }
  R rc(P,P,P,P,P) const { return R(); }
  R rc(P,P,P,P,P,P) const { return R(); }
  R rc(P,P,P,P,P,P,P) const { return R(); }
  R rc(P,P,P,P,P,P,P,P) const { return R(); }
  void ov() { }
  void ov(P) { }
  void ov(P,P) { }
  void ov(P,P,P) { }
  void ov(P,P,P,P) { }
  void ov(P,P,P,P,P) { }
  void ov(P,P,P,P,P,P) { }
  void ov(P,P,P,P,P,P,P) { }
  void ov(P,P,P,P,P,P,P,P) { }
  void ov() const { }
  void ov(P) const { }
  void ov(P,P) const { }
  void ov(P,P,P) const { }
  void ov(P,P,P,P) const { }
  void ov(P,P,P,P,P) const { }
  void ov(P,P,P,P,P,P) const { }
  void ov(P,P,P,P,P,P,P) const { }
  void ov(P,P,P,P,P,P,P,P) const { }
};

void test ()
{
  P p;
  S s;
  R r;
  const S* cs = &s;

  Bind (&N::v);
  Bind (&N::v,p);
  Bind (&N::v,p,p,p,p,p,p,p,p);

  r = Bind (&N::r)();
  r = Bind (&N::r,p)();
  r = Bind (&N::r,p,p,p,p,p,p,p,p)();

  Bind (&S::v,&s);
  Bind (&S::v,&s,p);

  Bind (&S::vc,&s);
  Bind (&S::vc,&s,p);

  r = Bind (&S::r,&s)();
  r = Bind (&S::r,&s,p)();

  r = Bind (&S::rc,&s)();
  r = Bind (&S::rc,&s,p)();

  // ambiguous overload resolution
  //Bind (&S::ov, cs); /* FAILS */

  // Test the external bind implementation
#if VF_HAVE_BIND
  bind (&N::v);
  bind (&N::v,p);
  bind (&N::v,p,p);
  bind (&N::v,p,p,p);
  bind (&N::v,p,p,p,p);
  bind (&N::v,p,p,p,p,p);
  bind (&N::v,p,p,p,p,p,p);
  bind (&N::v,p,p,p,p,p,p,p);
  bind (&N::v,p,p,p,p,p,p,p,p);

  r = bind (&N::r)();
  r = bind (&N::r,p)();
  r = bind (&N::r,p,p)();
  r = bind (&N::r,p,p,p)();
  r = bind (&N::r,p,p,p,p)();
  r = bind (&N::r,p,p,p,p,p)();
  r = bind (&N::r,p,p,p,p,p,p)();
  r = bind (&N::r,p,p,p,p,p,p,p)();
  r = bind (&N::r,p,p,p,p,p,p,p,p)();

  bind (&S::v,&s);
  bind (&S::v,&s,p);
  bind (&S::v,&s,p,p);
  bind (&S::v,&s,p,p,p);
  bind (&S::v,&s,p,p,p,p);
  bind (&S::v,&s,p,p,p,p,p);
  bind (&S::v,&s,p,p,p,p,p,p);
  bind (&S::v,&s,p,p,p,p,p,p,p);
  bind (&S::v,&s,p,p,p,p,p,p,p,p);

  bind (&S::vc,&s);
  bind (&S::vc,&s,p);
  bind (&S::vc,&s,p,p);
  bind (&S::vc,&s,p,p,p);
  bind (&S::vc,&s,p,p,p,p);
  bind (&S::vc,&s,p,p,p,p,p);
  bind (&S::vc,&s,p,p,p,p,p,p);
  bind (&S::vc,&s,p,p,p,p,p,p,p);
  bind (&S::vc,&s,p,p,p,p,p,p,p,p);

  r = bind (&S::r,&s)();
  r = bind (&S::r,&s,p)();
  r = bind (&S::r,&s,p,p)();
  r = bind (&S::r,&s,p,p,p)();
  r = bind (&S::r,&s,p,p,p,p)();
  r = bind (&S::r,&s,p,p,p,p,p)();
  r = bind (&S::r,&s,p,p,p,p,p,p)();
  r = bind (&S::r,&s,p,p,p,p,p,p,p)();
  r = bind (&S::r,&s,p,p,p,p,p,p,p,p)();

  r = bind (&S::rc,&s)();
  r = bind (&S::rc,&s,p)();
  r = bind (&S::rc,&s,p,p)();
  r = bind (&S::rc,&s,p,p,p)();
  r = bind (&S::rc,&s,p,p,p,p)();
  r = bind (&S::rc,&s,p,p,p,p,p)();
  r = bind (&S::rc,&s,p,p,p,p,p,p)();
  r = bind (&S::rc,&s,p,p,p,p,p,p,p)();
  r = bind (&S::rc,&s,p,p,p,p,p,p,p,p)();

  // ambiguous overload resolution
  bind (&S::ov,&s);
  bind (&S::ov,&s,p);
  bind (&S::ov,&s,p,p);
  bind (&S::ov,&s,p,p,p);
  bind (&S::ov,&s,p,p,p,p);
  bind (&S::ov,&s,p,p,p,p,p);
  bind (&S::ov,&s,p,p,p,p,p,p);
  bind (&S::ov,&s,p,p,p,p,p,p,p);
  bind (&S::ov,&s,p,p,p,p,p,p,p,p);
  bind (&S::ov,cs);
  bind (&S::ov,cs,p);
  bind (&S::ov,cs,p,p);
  bind (&S::ov,cs,p,p,p);
  bind (&S::ov,cs,p,p,p,p);
  bind (&S::ov,cs,p,p,p,p,p);
  bind (&S::ov,cs,p,p,p,p,p,p);
  bind (&S::ov,cs,p,p,p,p,p,p,p);
  bind (&S::ov,cs,p,p,p,p,p,p,p,p);
#endif
}

}

END_VF_NAMESPACE

