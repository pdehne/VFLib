// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php

#ifndef __VF_CACHELINEPADDING_VFHEADER__
#define __VF_CACHELINEPADDING_VFHEADER__

//
// Wraps an object and pads it to completely fill a CPU cache line.
//
template <typename T>
class CacheLinePadding
{
public:
  enum
  {
    CACHE_LINE_SIZE = 64
  };

  CacheLinePadding ()
    { }

  template <class T1>
  explicit CacheLinePadding (const T1& t1)
    : m_t (t1) { }

  template <class T1, class T2>
  CacheLinePadding (const T1& t1, const T2& t2)
    : m_t (t1, t2) { }

  template <class T1, class T2, class T3>
  CacheLinePadding (const T1& t1, const T2& t2, const T3& t3)
    : m_t (t1, t2, t3) { }

  template <class T1, class T2, class T3, class T4>
  CacheLinePadding (const T1& t1, const T2& t2, const T3& t3, const T4& t4)
    : m_t (t1, t2, t3, t4) { }

  template <class T1, class T2, class T3, class T4, class T5>
  CacheLinePadding (const T1& t1, const T2& t2, const T3& t3,
                    const T4& t4, const T5& t5)
    : m_t (t1, t2, t3, t4, t5) { }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  CacheLinePadding (const T1& t1, const T2& t2, const T3& t3,
                    const T4& t4, const T5& t5, const T6& t6)
    : m_t (t1, t2, t3, t4, t5, t6) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7>
  CacheLinePadding (const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                    const T5& t5, const T6& t6, const T7& t7)
    : m_t (t1, t2, t3, t4, t5, t6, t7) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7, class T8>
  CacheLinePadding (const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                    const T5& t5, const T6& t6, const T7& t7, const T8& t8)
    : m_t (t1, t2, t3, t4, t5, t6, t7, t8) { }

  void operator= (const T& other) { m_t = other; }

  T& operator* () { return m_t; }
  T* operator-> () { return &m_t; }
  operator T& () { return m_t; }
  operator T* () { return &m_t; }

  const T& operator* () const { return m_t; }
  const T* operator-> () const { return &m_t; }
  operator const T& () const { return m_t; }
  operator const T* () const { return &m_t; }

private:
  T m_t;
  char pad [CACHE_LINE_SIZE - sizeof(T)];
};

//
// Used to remove padding without changing code
//

template <typename T>
class NoCacheLinePadding
{
public:
  enum
  {
    CACHE_LINE_SIZE = 64
  };

  NoCacheLinePadding ()
    { }

  template <class T1>
  explicit NoCacheLinePadding (const T1& t1)
    : m_t (t1) { }

  template <class T1, class T2>
  NoCacheLinePadding (const T1& t1, const T2& t2)
    : m_t (t1, t2) { }

  template <class T1, class T2, class T3>
  NoCacheLinePadding (const T1& t1, const T2& t2, const T3& t3)
    : m_t (t1, t2, t3) { }

  template <class T1, class T2, class T3, class T4>
  NoCacheLinePadding (const T1& t1, const T2& t2, const T3& t3, const T4& t4)
    : m_t (t1, t2, t3, t4) { }

  template <class T1, class T2, class T3, class T4, class T5>
  NoCacheLinePadding (const T1& t1, const T2& t2, const T3& t3,
                    const T4& t4, const T5& t5)
    : m_t (t1, t2, t3, t4, t5) { }

  template <class T1, class T2, class T3, class T4, class T5, class T6>
  NoCacheLinePadding (const T1& t1, const T2& t2, const T3& t3,
                    const T4& t4, const T5& t5, const T6& t6)
    : m_t (t1, t2, t3, t4, t5, t6) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7>
  NoCacheLinePadding (const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                    const T5& t5, const T6& t6, const T7& t7)
    : m_t (t1, t2, t3, t4, t5, t6, t7) { }

  template <class T1, class T2, class T3, class T4,
            class T5, class T6, class T7, class T8>
  NoCacheLinePadding (const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                    const T5& t5, const T6& t6, const T7& t7, const T8& t8)
    : m_t (t1, t2, t3, t4, t5, t6, t7, t8) { }

  void operator= (const T& other) { m_t = other; }

  T& operator* () { return m_t; }
  T* operator-> () { return &m_t; }
  operator T& () { return m_t; }
  operator T* () { return &m_t; }

  const T& operator* () const { return m_t; }
  const T* operator-> () const { return &m_t; }
  operator const T& () const { return m_t; }
  operator const T* () const { return &m_t; }

private:
  T m_t;
};

#endif
