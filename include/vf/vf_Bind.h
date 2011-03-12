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
// Simple version of boost::bind that works only for free functions
// or class member functions taking 0 or 1 arguments.
//

namespace detail {

template <typename R, class P>
struct Bind0 { typedef typename R result_type; typedef R (P::*mf_t)();
  Bind0 (mf_t mf, P* p) : m_mf (mf), m_p (p) { }
  void operator()() { return (m_p->*m_mf)(); } private:
  mf_t m_mf; P* m_p; };

template <typename R, class P,typename T1>
struct Bind1 { typedef typename R result_type; typedef R (P::*mf_t)(T1);
  Bind1 (mf_t mf, P* p, T1 t1) : m_mf (mf), m_p (p),
    m_t1(t1) { }
  void operator()() { return (m_p->*m_mf)(m_t1); } private:
  mf_t m_mf; P* m_p; T1 m_t1; };

template <class P,typename T1,typename T2>
struct Bind2 { typedef void (P::*mf_t)(T1,T2);
  Bind2 (mf_t mf, P* p, T1 t1, T2 t2) : m_mf (mf), m_p (p),
    m_t1(t1), m_t2(t2) { }
  void operator()() { (m_p->*m_mf)(m_t1, m_t2); } private:
  mf_t m_mf; P* m_p; T1 m_t1; T2 m_t2; };

template <class P,typename T1,typename T2,typename T3>
struct Bind3 { typedef void (P::*mf_t)(T1,T2,T3);
  Bind3 (mf_t mf, P* p, T1 t1, T2 t2, T3 t3) : m_mf (mf), m_p (p),
    m_t1(t1), m_t2(t2), m_t3(t3) { }
  void operator()() { (m_p->*m_mf)(m_t1, m_t2, m_t3); } private:
  mf_t m_mf; P* m_p; T1 m_t1; T2 m_t2; T3 m_t3; };

template <class P,typename T1,typename T2,typename T3,typename T4>
struct Bind4 { typedef void (P::*mf_t)(T1,T2,T3,T4);
  Bind4 (mf_t mf, P* p, T1 t1, T2 t2, T3 t3, T4 t4) : m_mf (mf), m_p (p),
    m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4) { }
  void operator()() { (m_p->*m_mf)(m_t1, m_t2, m_t3, m_t4); } private:
  mf_t m_mf; P* m_p; T1 m_t1; T2 m_t2; T3 m_t3; T4 m_t4; };

template <class P,typename T1,typename T2,typename T3,typename T4,typename T5>
struct Bind5 { typedef void (P::*mf_t)(T1,T2,T3,T4,T5);
  Bind5 (mf_t mf, P* p, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) : m_mf (mf), m_p (p),
    m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5) { }
  void operator()() { (m_p->*m_mf)(m_t1, m_t2, m_t3, m_t4, m_t5); } private:
  mf_t m_mf; P* m_p; T1 m_t1; T2 m_t2; T3 m_t3; T4 m_t4; T5 m_t5; };

template <class P,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
struct Bind6 { typedef void (P::*mf_t)(T1,T2,T3,T4,T5,T6);
  Bind6 (mf_t mf, P* p, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) : m_mf (mf), m_p (p),
    m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5), m_t6(t6) { }
  void operator()() { (m_p->*m_mf)(m_t1, m_t2, m_t3, m_t4, m_t5, m_t6); } private:
  mf_t m_mf; P* m_p; T1 m_t1; T2 m_t2; T3 m_t3; T4 m_t4; T5 m_t5; T6 m_t6; };

template <class P,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
struct Bind7 { typedef void (P::*mf_t)(T1,T2,T3,T4,T5,T6,T7);
  Bind7 (mf_t mf, P* p, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) : m_mf (mf), m_p (p),
    m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5), m_t6(t6), m_t7(t7) { }
  void operator()() { (m_p->*m_mf)(m_t1, m_t2, m_t3, m_t4, m_t5, m_t6, m_t7); } private:
  mf_t m_mf; P* m_p; T1 m_t1; T2 m_t2; T3 m_t3; T4 m_t4; T5 m_t5; T6 m_t6; T7 m_t7; };

template <typename R,class P,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
struct Bind8 { typedef typename R result_type; typedef R (P::*mf_t)(T1,T2,T3,T4,T5,T6,T7,T8);
  Bind8 (mf_t mf, P* p, T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) : m_mf (mf), m_p (p),
    m_t1(t1), m_t2(t2), m_t3(t3), m_t4(t4), m_t5(t5), m_t6(t6), m_t7(t7), m_t8(t8) { }
  R operator()() { return (m_p->*m_mf)(m_t1, m_t2, m_t3, m_t4, m_t5, m_t6, m_t7, m_t8); } private:
  mf_t m_mf; P* m_p; T1 m_t1; T2 m_t2; T3 m_t3; T4 m_t4; T5 m_t5; T6 m_t6; T7 m_t7; T8 m_t8; };

}

template <typename R, class P>
detail::Bind0 <R,P> Bind (R (P::*f)(), P* p) { return
  detail::Bind0 <R, P> (f,p); }

template <typename R, class P, typename T1>
detail::Bind1 <R,P,T1> Bind (R (P::*f)(T1), P* p,
  T1 t1) { return
  detail::Bind1 <R,P,T1> (f,p,t1); }

template <class P,typename T1,typename T2>
detail::Bind2 <P,T1,T2> Bind (void (P::*f)(T1,T2), P* p,
  T1 t1, T2 t2) { return
  detail::Bind2 <P,T1,T2> (f,p,t1,t2); }

template <class P,typename T1,typename T2,typename T3>
detail::Bind3 <P,T1,T2,T3> Bind (void (P::*f)(T1,T2,T3), P* p,
  T1 t1, T2 t2, T3 t3) { return
  detail::Bind3 <P,T1,T2,T3> (f,p,t1,t2,t3); }

template <class P,typename T1,typename T2,typename T3,typename T4>
detail::Bind4 <P,T1,T2,T3,T4> Bind (void (P::*f)(T1,T2,T3,T4), P* p,
  T1 t1, T2 t2, T3 t3, T4 t4) { return
  detail::Bind4 <P,T1,T2,T3,T4> (f,p,t1,t2,t3,t4); }

template <class P,typename T1,typename T2,typename T3,typename T4,typename T5>
detail::Bind5 <P,T1,T2,T3,T4,T5> Bind (void (P::*f)(T1,T2,T3,T4,T5), P* p,
  T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) { return
  detail::Bind5 <P,T1,T2,T3,T4,T5> (f,p,t1,t2,t3,t4,t5); }

template <class P,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
detail::Bind6 <P,T1,T2,T3,T4,T5,T6> Bind (void (P::*f)(T1,T2,T3,T4,T5,T6), P* p,
  T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) { return
  detail::Bind6 <P,T1,T2,T3,T4,T5,T6> (f,p,t1,t2,t3,t4,t5,t6); }

template <class P,typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
detail::Bind7 <P,T1,T2,T3,T4,T5,T6,T7> Bind (void (P::*f)(T1,T2,T3,T4,T5,T6,T7), P* p,
  T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) { return
  detail::Bind7 <P,T1,T2,T3,T4,T5,T6,T7> (f,p,t1,t2,t3,t4,t5,t6,t7); }

template <typename R, class P, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
detail::Bind8 <R,P,T1,T2,T3,T4,T5,T6,T7,T8> Bind (R (P::*f)(T1,T2,T3,T4,T5,T6,T7,T8), P* p,
  T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8 ) { return
detail::Bind8 <R,P,T1,T2,T3,T4,T5,T6,T7,T8> (f,p,t1,t2,t3,t4,t5,t6,t7,t8); }

#endif
