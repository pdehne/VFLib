/*============================================================================*/
/*
  Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
  See the file GNU_GPL_v2.txt for full licensing terms.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 51
  Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*============================================================================*/

#ifndef VF_PARALLELFOR_VFHEADER
#define VF_PARALLELFOR_VFHEADER

#include "vf_ThreadGroup.h"

/*============================================================================*/
/**
  @ingroup vf_concurrent

  @brief Parallel for loop.

  This uses a ThreadGroup to iterate through a for loop in parallel. The
  for loop must take this form:

  @code

  for (int i = 0; i < numberOfIterations; ++i)
    function (i, ...);

  @endcode

  `function` is a caller provided functor. Convenience functions are provided
  for automatic binding to member or non member functions with up to 8
  arguments (not including the loop index).

  The first argument to function() is always the array index.

  @see ThreadGroup
*/
class ParallelFor : Uncopyable
{
public:
  explicit ParallelFor (ThreadGroup& threadGroup);

  /** Execute parallel for loop.

      Functor is called once for each value in the range
      [0, numberOfIterations), using the ThreadGroup.
  */
  template <class Functor>
  void doForEach (int numberOfIterations,
                  Functor const& f)
  {
    IterationType <Functor> iteration (f);

    doForEach (numberOfIterations, f);
  }

  template <class Fn>
  void doForEach (int n, Fn f)
  { doForEach (n, vf::bind (f, vf::_1)); }

  template <class Fn,          typename  T1>
  void doForEach (int n, Fn f, const T1& t1)
  { doForEach (n, vf::bind (f, vf::_1, 1)); }

  template <class Fn,          typename  T1, typename  T2>
  void doForEach (int n, Fn f, const T1& t1, const T2& t2)
  { doForEach (n, vf::bind (f, vf::_1, t1, t2)); }

  template <class Fn,          typename  T1, typename  T2, typename  T3>
  void doForEach (int n, Fn f, const T1& t1, const T2& t2, const T3& t3)
  { doForEach (n, vf::bind (f, vf::_1, t1, t2, t3)); }

  template <class Fn,          typename  T1, typename  T2,
                               typename  T3, typename  T4>
  void doForEach (int n, Fn f, const T1& t1, const T2& t2,
                               const T3& t3, const T4& t4)
  { doForEach (n, vf::bind (f, vf::_1, t1, t2, t3, t4)); }

  template <class Fn,          typename  T1, typename  T2, typename  T3,
                               typename  T4, typename  T5>
  void doForEach (int n, Fn f, const T1& t1, const T2& t2, const T3& t3,
                               const T4& t4, const T5& t5)
  { doForEach (n, vf::bind (f, vf::_1, t1, t2, t3, t4, t5)); }

  template <class Fn,          typename  T1, typename  T2, typename  T3,
                               typename  T4, typename  T5, typename  T6>
  void doForEach (int n, Fn f, const T1& t1, const T2& t2, const T3& t3,
                               const T4& t4, const T5& t5, const T6& t6)
  { doForEach (n, vf::bind (f, vf::_1, t1, t2, t3, t4, t5, t6)); }

  template <class Fn,          typename  T1, typename  T2, typename  T3, typename  T4,
                               typename  T5, typename  T6, typename  T7>
  void doForEach (int n, Fn f, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                               const T5& t5, const T6& t6, const T7& t7)
  { doForEach (n, vf::bind (f, vf::_1, t1, t2, t3, t4, t5, t6, t7)); }

  template <class Fn,          typename  T1, typename  T2, typename  T3, typename  T4,
                               typename  T5, typename  T6, typename  T7, typename  T8>
  void doForEach (int n, Fn f, const T1& t1, const T2& t2, const T3& t3, const T4& t4,
                               const T5& t5, const T6& t6, const T7& t7, const T8& t8)
  { doForEach (n, vf::bind (f, vf::_1, t1, t2, t3, t4, t5, t6, t7, t8)); }

private:
  class Iteration
  {
  public:
    virtual void operator () (int loopIndex) = 0;
  };

  template <class Functor>
  class IterationType : public Iteration
  {
  public:
    explicit IterationType (Functor const& f) : m_f (f)
    {
    }

    void operator () (int loopIndex)
    {
      m_f (loopIndex);
    }

  private:
    Functor m_f;
  };

private:
  void doOne (int index, Iteration& iteration);

  void doForEach (int numberOfIterations, Iteration& iteration);

private:
  ThreadGroup& m_threadGroup;
  Atomic <int> m_counter;
  WaitableEvent m_event;
};

#endif
