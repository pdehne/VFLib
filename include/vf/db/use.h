// Copyright (C) 2008 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_USE_VFHEADER__
#define __VF_DB_USE_VFHEADER__

#include "vf/db/detail/use_type.h"
#include "vf/db/detail/exchange_traits.h"
#include "vf/db/detail/type_conversion.h"

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