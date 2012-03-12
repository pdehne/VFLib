// Copyright (C) 2008 by Vinnie Falco, this file is part of VFLib.
// See the file LICENSE.txt for licensing information.
// Based on ideas from the soci wrapper sqlite back-end.

#ifndef __VF_DB_USE_VFHEADER__
#define __VF_DB_USE_VFHEADER__

#include "vf/modules/vf_db/detail/use_type.h"
#include "vf/modules/vf_db/detail/exchange_traits.h"
#include "vf/modules/vf_db/detail/type_conversion.h"

namespace db {

inline detail::use_type_ptr use_null()
{
  return detail::do_use_null();
}

template <typename T>
detail::use_type_ptr use(T& t)
{
  return detail::do_use(t,
                        typename detail::exchange_traits<T>::type_family());
}

template <typename T>
detail::use_type_ptr use(T const& t)
{
  return detail::do_use(t,
                        typename detail::exchange_traits<T>::type_family());
}

template <typename T>
detail::use_type_ptr use(T& t, indicator& ind)
{
  return detail::do_use(t, ind,
                        typename detail::exchange_traits<T>::type_family());
}

}

#endif