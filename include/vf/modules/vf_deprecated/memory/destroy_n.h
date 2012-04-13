// Copyright (C) 2008 by One Guy Group, Inc., All rights reserved worldwide..

#ifndef VF_DESTROY_N_H
#define VF_DESTROY_N_H

#ifdef _MSC_VER
namespace tr1 = std::tr1;
#else
namespace tr1 = std;
#endif

//! destroy_n
//!
//! destroy an array of objects, only if the destructor is non trivial

namespace detail {

template<class Ty, bool hasTrivialDestructor>
struct destroyer
{
  static inline void destroy (Ty* p, std::size_t n)
  {
    while (n--)
      (*p++).~Ty();
  }
};

template<class Ty>
struct destroyer<Ty, true>
{
  static inline void destroy (Ty* p, std::size_t n)
  {
    // do nothing
  }
};

}

template<class Ty>
void destroy_n( Ty* p, std::size_t n )
{
  detail::destroyer<Ty, tr1::has_trivial_destructor<Ty>::value>::destroy (p, n);
}

#endif

