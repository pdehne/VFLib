// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_ALLOCATOR_VFHEADER__
#define __VF_ALLOCATOR_VFHEADER__

// Utility to bolt on New/Delete for class C using class P to obtain memory.
//
//  class P
//  {
//  public:
//    void* alloc (size_t bytes);
//    void free (void* p);
//  };
//
template <class P, class C>
class Allocator
{
public:
  explicit Allocator (P& p) : m_p (&p) { }
  Allocator (const Allocator& other) : m_p (other.m_p) { }
  Allocator& operator= (const Allocator& other) { m_p = other.m_p; }

  C* New ()
    { return new (m_p->alloc<sizeof(C)> ()) C; }

  template <class T1>
  C* New (const T1& t1)
    { return new (m_p->alloc <sizeof (C)>()) C (t1); }

  template <class T1, class T2>
  C* New (const T1& t1, const T2& t2)
    { return new (m_p->alloc <sizeof (C)>()) C (t1, t2); }

  template <class T1, class T2, class T3>
  C* New (const T1& t1, const T2& t2, const T3& t3)
    { return new (m_p->alloc <sizeof (C)>()) C (t1, t2, t3); }

  template <class T1, class T2, class T3, class T4>
  C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4)
    { return new (m_p->alloc <sizeof (C)>()) C (t1, t2, t3, t4); }

  template <class T1, class T2, class T3, class T4, class T5>
  C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
    { return new (m_p->alloc <sizeof (C)>()) C (t1, t2, t3, t4, t5); }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  C* New (const T1& t1, const T2& t2, const T3& t3,
          const T4& t4, const T5& t5, const T6& t6)
    { return new (m_p->alloc <sizeof (C)>()) C (t1, t2, t3, t4, t5, t6); }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
  C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
          const T5& t5, const T6& t6, const T7& t7)
    { return new (m_p->alloc <sizeof (C)>()) C (t1, t2, t3, t4, t5, t6, t7); }

  template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
  C* New (const T1& t1, const T2& t2, const T3& t3, const T4& t4, 
          const T5& t5, const T6& t6, const T7& t7, const T8& t8)
    { return new (m_p->alloc <sizeof (C)>()) C (t1, t2, t3, t4, t5, t6, t7, t8); }

  void Delete (C* c)
  {
    c->~C();
    m_p->free (c);
  }

private:
  P* m_p;
};

#endif
