// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_BIND_VFHEADER__
#define __VF_BIND_VFHEADER__

#ifdef VF_HAVE_BIND
#error "VF_HAVE_BIND already set"
#endif

#if VF_HAVE_BOOST

//
// Boost
//
using ::boost::bind;
using ::boost::ref;
using ::boost::cref;
//using   ::boost::protect;
using ::_1;
using ::_2;
using ::_3;
using ::_4;
using ::_5;
using ::_6;
using ::_7;
using ::_8;
using ::_9; // boost limit as of 1.45
#define VF_HAVE_BIND 1

#elif defined (_MSC_VER) && defined (_HAS_TR1)

//
// std::tr1
//
using std::tr1::bind;
using std::tr1::ref;
using std::tr1::cref;
using std::tr1::placeholders::_1;
using std::tr1::placeholders::_2;
using std::tr1::placeholders::_3;
using std::tr1::placeholders::_4;
using std::tr1::placeholders::_5;
using std::tr1::placeholders::_6;
using std::tr1::placeholders::_7;
using std::tr1::placeholders::_8;
using std::tr1::placeholders::_9;
#define VF_HAVE_BIND 1

#else

#define VF_HAVE_BIND 0

#endif

//
// Bind
//
// Simplified bind to be used internally to
// remove the dependence on boost or tr1.
//

namespace detail {

template <class R>
struct BindF0
{
  typedef typename R result_type;
  typedef R (*f_t)();
  BindF0 (f_t f):m_f(f){}
  R operator()(){return m_f();}
private:
  f_t m_f;
};

template <class R, class T1>
struct BindF1 { typedef typename R result_type; typedef R (*f_t)
  (T1); BindF1 (f_t f, const T1& t1):m_f(f), m_t1(t1) { }
  R operator()(){return m_f(m_t1);}
private:
  f_t m_f; T1 m_t1;
};

template <class R, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
struct BindF8 { typedef typename R result_type; typedef R (*f_t)
  (T1,T2,T3,T4,T5,T6,T7,T8); BindF8 (f_t f,
  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
  const T5& t5, const T6& t6, const T7& t7, const T8& t8) : m_f(f),
  m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5), m_t6(t6), m_t7(t7), m_t8(t8) { }
  R operator()(){return m_f(m_t1,m_t2,m_t3,m_t4,m_t5,m_t6,m_t7,m_t8);}
private:
  f_t m_f; T1 m_t1; T2 m_t2; T3 m_t3; T4 m_t4; T5 m_t5; T6 m_t6; T7 m_t7; T8 m_t8; 
};

template <class R,class P>
struct BindMf0
{
  typedef typename R result_type;
  typedef R (P::*mf_t)();
  BindMf0 (mf_t mf,P*p):m_mf(mf),m_p(p){}
  R operator()(){return(m_p->*m_mf)();}
private:
  mf_t m_mf;P*m_p;
};

template <class R,class P>
struct BindMfc0
{
  typedef typename R result_type;
  typedef R (P::*mf_t)() const;
  BindMfc0 (mf_t mf, const P* p):m_mf(mf),m_p(p){}
  R operator()(){return(m_p->*m_mf)();}
private:
  mf_t m_mf; const P* m_p;
};

template <class R, class P, class T1>
struct BindMf1 { typedef typename R result_type; typedef R (P::*mf_t)(T1);
  BindMf1 (mf_t mf, P* p, T1 t1) : m_mf (mf), m_p (p),
    m_t1(t1) { }
  R operator()() { return (m_p->*m_mf)(m_t1); } private:
  mf_t m_mf; P* m_p; T1 m_t1; };

template <class R, class P, class T1>
struct BindMfc1 { typedef typename R result_type; typedef R (P::*mf_t)(T1) const;
  BindMfc1 (mf_t mf, const P* p, T1 t1) : m_mf (mf), m_p (p),
    m_t1(t1) { }
  R operator()() { return (m_p->*m_mf)(m_t1); } private:
  mf_t m_mf; const P* m_p; T1 m_t1; };
}

template <class R>
detail::BindF0 <R> Bind (R (*f)()) { return
  detail::BindF0 <R> (f); }

template <class R, class T1>
detail::BindF1 <R,T1> Bind (R (*f)(T1), const T1& t1) { return
  detail::BindF1 <R,T1> (f,t1); }

template <class R, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
detail::BindF8 <R,T1,T2,T3,T4,T5,T6,T7,T8> Bind (R (*f)(T1,T2,T3,T4,T5,T6,T7,T8),
  const T1& t1, const T2& t2, const T3& t3, const T4& t4,
  const T5& t5, const T6& t6, const T7& t7, const T8& t8) { return
  detail::BindF8 <R,T1,T2,T3,T4,T5,T6,T7,T8> (f,t1,t2,t3,t4,t5,t6,t7,t8); }

template <class R, class P>
detail::BindMf0 <R,P> Bind (R (P::*f)(), P* p) { return
  detail::BindMf0 <R, P> (f,p); }

template <class R, class P>
detail::BindMfc0 <R,P> Bind (R (P::*f)() const, const P* p) { return
  detail::BindMfc0 <R, P> (f,p); }

template <class R, class P, class T1>
detail::BindMf1 <R,P,T1> Bind (R (P::*f)(T1), P* p,
  T1 t1) { return
  detail::BindMf1 <R,P,T1> (f,p,t1); }

template <class R, class P, class T1>
detail::BindMfc1 <R,P,T1> Bind (R (P::*f)(T1) const, P const* p,
  T1 t1) { return
  detail::BindMfc1 <R,P,T1> (f,p,t1); }

#endif
