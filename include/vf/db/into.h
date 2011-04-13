// Copyright (C) 2008-2011 by Vincent Falco, All rights reserved worldwide.
// This file is released under the MIT License:
// http://www.opensource.org/licenses/mit-license.php
// Based on ideas from the soci wrapper sqlite back-end

#ifndef __VF_DB_INTO_VFHEADER__
#define __VF_DB_INTO_VFHEADER__

#include "vf/db/detail/into_type.h"
#include "vf/db/detail/exchange_traits.h"
#include "vf/db/detail/type_conversion.h"

namespace db {

template<typename T>
detail::into_type_ptr into(T& t)
{
  return detail::do_into(t,
                         typename detail::exchange_traits<T>::type_family());
}

template<typename T>
detail::into_type_ptr into(T& t, indicator& ind)
{
  return detail::do_into(t, ind,
                         typename detail::exchange_traits<T>::type_family());
}

}

#endif